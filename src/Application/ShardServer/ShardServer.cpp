#include "ShardServer.h"
#include "System/Config.h"
#include "Framework/Replication/ReplicationConfig.h"
#include "Application/Common/ContextTransport.h"
#include "Application/Common/DatabaseConsts.h"
#include "Application/Common/CatchupMessage.h"

#define REQUEST_TIMEOUT 1000
#define DATABASE_NAME   "system"

static size_t Hash(uint64_t h)
{
    return h;
}

void ShardServer::Init()
{
    unsigned        numControllers;
    uint64_t        nodeID;
    uint64_t        runID;
    const char*     str;
    Endpoint        endpoint;

    databaseEnv.Open(configFile.GetValue("database.dir", "db"));
    systemDatabase = databaseEnv.GetDatabase(DATABASE_NAME);
    REPLICATION_CONFIG->Init(systemDatabase->GetTable("system"));

    runID = REPLICATION_CONFIG->GetRunID();
    runID += 1;
    REPLICATION_CONFIG->SetRunID(runID);
    REPLICATION_CONFIG->Commit();
    Log_Trace("rundID: %" PRIu64, runID);

    if (REPLICATION_CONFIG->GetNodeID() > 0)
    {
        isAwaitingNodeID = false;
        CONTEXT_TRANSPORT->SetSelfNodeID(REPLICATION_CONFIG->GetNodeID());
    }
    else
        isAwaitingNodeID = true;
    
    // connect to the controller nodes
    numControllers = (unsigned) configFile.GetListNum("controllers");
    for (nodeID = 0; nodeID < numControllers; nodeID++)
    {
        str = configFile.GetListValue("controllers", nodeID, "");
        endpoint.Set(str);
        CONTEXT_TRANSPORT->AddNode(nodeID, endpoint);
        controllers.Append(nodeID);
        // this will cause the node to connect to the controllers
        // and if my nodeID is not set the MASTER will automatically send
        // me a SetNodeID cluster message
    }

    CONTEXT_TRANSPORT->SetClusterContext(this);
    
    isSendingCatchup = false;
    isCatchingUp = false;
    catchupCursor = NULL;
    
    requestTimer.SetDelay(REQUEST_TIMEOUT);
    requestTimer.SetCallable(MFUNC(ShardServer, OnRequestLeaseTimeout));    
}

void ShardServer::Shutdown()
{
    DatabaseMap::Node*  dbNode;
    StorageDatabase*    database;

    CONTEXT_TRANSPORT->Shutdown();
    REPLICATION_CONFIG->Shutdown();
    
    for (dbNode = databases.First(); dbNode != NULL; dbNode = databases.Next(dbNode))
    {
        database = dbNode->Value();
        delete database;
    }

    databaseEnv.Close();
}

ShardServer::QuorumList* ShardServer::GetQuorums()
{
    return &quorums;
}

StorageEnvironment& ShardServer::GetEnvironment()
{
    return databaseEnv;
}

void ShardServer::ProcessMessage(QuorumData* quorumData, DataMessage& message, bool ownAppend)
{
    StorageTable*       table;
    ClientRequest*      request;

    table = LocateTable(message.tableID);
    if (!table)
        ASSERT_FAIL();
    
    request = NULL;
    if (ownAppend)
    {
        assert(quorumData->dataMessages.GetLength() > 0);
        assert(quorumData->dataMessages.First()->type == message.type);
        assert(quorumData->requests.GetLength() > 0);
        request = quorumData->requests.First();
        request->response.OK();
    }
    
    // TODO: differentiate return status (FAILED, NOSERVICE)
    switch (message.type)
    {
        case CLIENTREQUEST_SET:
            if (!table->Set(message.key, message.value))
            {
                if (request)
                    request->response.Failed();
            }
            break;
        case CLIENTREQUEST_DELETE:
            if (!table->Delete(message.key))
            {
                if (request)
                    request->response.Failed();
            }
            break;
        default:
            ASSERT_FAIL();
            break;
    }

    if (ownAppend)
    {
        quorumData->requests.Remove(request);
        request->OnComplete(); // request deletes itself
        quorumData->dataMessages.Delete(quorumData->dataMessages.First());
    }
}

void ShardServer::OnWriteReadyness()
{
    uint64_t            *it;
    CatchupMessage      msg;
    QuorumData*         quorumData;
    StorageShard*       shard;
    StorageKeyValue*    kv;

    kv = catchupCursor->Next();
    if (kv)
    {
        msg.KeyValue(kv->key, kv->value);
        CONTEXT_TRANSPORT->SendQuorumMessage(
         catchupRequest.nodeID, catchupRequest.quorumID, msg);
        return;
    }

    // kv was NULL, at end of current shard
    assert(catchupCursor != NULL);
    delete catchupCursor;

    // find next shard
    quorumData = LocateQuorum(catchupRequest.quorumID);
    if (!quorumData)
        ASSERT_FAIL();

    // iterate to current, which is catchupShardID
    FOREACH(it, quorumData->shards)
    {
        if (*it == catchupShardID)
            break;
    }
    assert(it != NULL);
    // move to next
    it = quorumData->shards.Next(it);

    if (it)
    {
        // there is a next shard, start sending it now
        catchupShardID = *it;
        shard = LocateShard(catchupShardID);
        assert(shard != NULL);
        catchupCursor = new StorageCursor(shard);
        msg.BeginShard(catchupShardID);
        CONTEXT_TRANSPORT->SendQuorumMessage(
        catchupRequest.nodeID, catchupRequest.quorumID, msg);
        // send first KV
        kv = catchupCursor->Begin(ReadBuffer());
        if (kv)
        {
            msg.KeyValue(kv->key, kv->value);
            CONTEXT_TRANSPORT->SendQuorumMessage(
             catchupRequest.nodeID, catchupRequest.quorumID, msg);
        }
    }
    else
    {
        // there is no next shard, we're all done, send commit
        msg.Commit(catchupPaxosID);
        // send the paxosID whose value is in the db
        CONTEXT_TRANSPORT->SendQuorumMessage(
         catchupRequest.nodeID, catchupRequest.quorumID, msg);
        
        CONTEXT_TRANSPORT->UnregisterWriteReadyness(
         catchupRequest.nodeID, MFUNC(ShardServer, OnWriteReadyness));
    }    
}

bool ShardServer::IsLeaseKnown(uint64_t quorumID)
{
    QuorumData*     quorumData;
    ConfigQuorum*   configQuorum;
    
    quorumData = LocateQuorum(quorumID);
    if (quorumData == NULL)
        return false;

    if (quorumData->isPrimary)
        return true;
    
    configQuorum = configState.GetQuorum(quorumID);
    if (configQuorum == NULL)
        return false;
    
    if (!configQuorum->hasPrimary)
        return false;
    
    // we already checked this case in quorumData earlier
    if (configQuorum->primaryID == REPLICATION_CONFIG->GetNodeID())
        return false;
    
    return true;
}

bool ShardServer::IsLeaseOwner(uint64_t quorumID)
{
    QuorumData*     quorumData;
    
    quorumData = LocateQuorum(quorumID);
    if (quorumData == NULL)
        return false;
     
    return quorumData->isPrimary;
}

uint64_t ShardServer::GetLeaseOwner(uint64_t quorumID)
{
    QuorumData*     quorumData;
    ConfigQuorum*   configQuorum;
    
    quorumData = LocateQuorum(quorumID);
    if (quorumData == NULL)
        return 0;

    if (quorumData->isPrimary)
        return REPLICATION_CONFIG->GetNodeID();
    
    configQuorum = configState.GetQuorum(quorumID);
    if (configQuorum == NULL)
        return 0;
    
    if (!configQuorum->hasPrimary)
        return 0;
    
    // we already checked this case in quorumData earlier
    if (configQuorum->primaryID == REPLICATION_CONFIG->GetNodeID())
        return 0;
    
    return configQuorum->primaryID;
}

void ShardServer::OnAppend(uint64_t quorumID, ReadBuffer& value, bool ownAppend)
{
    int                 read;
    QuorumData*         quorumData;
    DataMessage         message;
    
    Log_Trace();
    
    quorumData = LocateQuorum(quorumID);
    if (!quorumData)
        ASSERT_FAIL();
        
    while (value.GetLength() > 0)
    {
        read = message.Read(value);
        if (read <= 0)
            ASSERT_FAIL();

        ProcessMessage(quorumData, message, ownAppend);
    }
    
    if (quorumData->dataMessages.GetLength() > 0)
        TryAppend(quorumData);
}

void ShardServer::OnStartCatchup(uint64_t quorumID)
{
    QuorumData*     quorumData;
    CatchupMessage  msg;

    quorumData = LocateQuorum(quorumID);
    if (!quorumData)
        ASSERT_FAIL();

    if (isCatchingUp || !quorumData->context.IsLeaseKnown())
        return;
    
    quorumData->context.StopReplication();
    
    msg.CatchupRequest(REPLICATION_CONFIG->GetNodeID(), quorumID);
    
    CONTEXT_TRANSPORT->SendQuorumMessage(
     quorumData->context.GetLeaseOwner(), quorumID, msg);
     
    isCatchingUp = true;
    
    Log_Message("Catchup started from node %" PRIu64 "", quorumData->context.GetLeaseOwner());
}

void ShardServer::OnCatchupMessage(CatchupMessage& imsg)
{
    CatchupMessage      omsg;
    Buffer              buffer;
    ReadBuffer          key;
    ReadBuffer          value;
    QuorumData*         quorumData;
    StorageShard*       shard;
    StorageKeyValue*    kv;
    
    switch (imsg.type)
    {
        case CATCHUPMESSAGE_REQUEST:
            if (isSendingCatchup)
                return;
            catchupRequest = imsg;
            catchupPaxosID = quorumData->context.GetPaxosID() - 1;

            quorumData = LocateQuorum(catchupRequest.quorumID);
            if (!quorumData)
                ASSERT_FAIL();

            if (!quorumData->context.IsLeader())
                return;

            if (quorumData->shards.GetLength() == 0)
            {
                // no shards in quorums, send Commit
                omsg.Commit(catchupPaxosID);
                // send the paxosID whose value is in the db
                CONTEXT_TRANSPORT->SendQuorumMessage(
                 catchupRequest.nodeID, catchupRequest.quorumID, omsg);
                break;
            }
            
            // use the WriteReadyness mechanism to send the KVs
            // OnWriteReadyness() will be called when the next KVs can be sent
            CONTEXT_TRANSPORT->RegisterWriteReadyness(
             catchupRequest.nodeID, MFUNC(ShardServer, OnWriteReadyness));

            catchupShardID = *quorumData->shards.First();
            shard = LocateShard(catchupShardID);
            assert(shard != NULL);
            catchupCursor = new StorageCursor(shard);
            omsg.BeginShard(catchupShardID);
            CONTEXT_TRANSPORT->SendQuorumMessage(
                 catchupRequest.nodeID, catchupRequest.quorumID, omsg);
            // send first KV
            kv = catchupCursor->Begin(ReadBuffer());
            if (kv)
            {
                omsg.KeyValue(kv->key, kv->value);
                CONTEXT_TRANSPORT->SendQuorumMessage(
                 catchupRequest.nodeID, catchupRequest.quorumID, omsg);
            }
            isSendingCatchup = true;
            break;
        case CATCHUPMESSAGE_BEGIN_SHARD:
            if (!isCatchingUp)
                return;
            // TODO: xxx
            break;
        case CATCHUPMESSAGE_KEYVALUE:
            if (!isCatchingUp)
                return;
            // TODO: xxx
            break;
        case CATCHUPMESSAGE_COMMIT:
            if (!isCatchingUp)
                return;
            // TODO: xxx
            break;
        default:
            ASSERT_FAIL();
    }
}

bool ShardServer::IsValidClientRequest(ClientRequest* request)
{
     return request->IsShardServerRequest();
}

void ShardServer::OnClientRequest(ClientRequest* request)
{
    DataMessage*    message;
    ConfigShard*    shard;
    QuorumData*     quorumData;
    
    shard = configState.GetShard(request->tableID, ReadBuffer(request->key));
    if (!shard)
    {
        request->response.Failed();
        request->OnComplete();
        return;
    }

    quorumData = LocateQuorum(shard->quorumID);
    if (!quorumData)
    {
        request->response.NoService();
        request->OnComplete();
        return;
    }
    
    if (request->type == CLIENTREQUEST_GET)
        return OnClientRequestGet(request);

    message = new DataMessage;
    FromClientRequest(request, message);
    
    quorumData->requests.Append(request);
    quorumData->dataMessages.Append(message);
    TryAppend(quorumData);
}

void ShardServer::OnClientClose(ClientSession* /*session*/)
{
    // nothing
}

void ShardServer::OnClusterMessage(uint64_t /*nodeID*/, ClusterMessage& msg)
{
    Log_Trace();
    
    switch (msg.type)
    {
        case CLUSTERMESSAGE_SET_NODEID:
            if (!isAwaitingNodeID)
                return;
            isAwaitingNodeID = false;
            CONTEXT_TRANSPORT->SetSelfNodeID(msg.nodeID);
            REPLICATION_CONFIG->SetNodeID(msg.nodeID);
            REPLICATION_CONFIG->Commit();
            Log_Trace("my nodeID is %" PRIu64 "", msg.nodeID);
            break;
        case CLUSTERMESSAGE_SET_CONFIG_STATE:
            OnSetConfigState(msg.configState);
            Log_Trace("got new configState, master is %d", 
             configState.hasMaster ? (int) configState.masterID : -1);
            break;
        case CLUSTERMESSAGE_RECEIVE_LEASE:
            OnReceiveLease(msg.quorumID, msg.proposalID);
            Log_Trace("recieved lease, quorumID = %" PRIu64 ", proposalID =  %" PRIu64,
             msg.quorumID, msg.proposalID);
            break;
        default:
            ASSERT_FAIL();
    }
}

void ShardServer::OnIncomingConnectionReady(uint64_t /*nodeID*/, Endpoint /*endpoint*/)
{
    // nothing
}

bool ShardServer::OnAwaitingNodeID(Endpoint /*endpoint*/)
{
    // always drop
    return true;
}

QuorumData* ShardServer::LocateQuorum(uint64_t quorumID)
{
    QuorumData* quorum;
    
    for (quorum = quorums.First(); quorum != NULL; quorum = quorums.Next(quorum))
    {
        if (quorum->quorumID == quorumID)
            return quorum;
    }
    
    return NULL;
}

void ShardServer::TryAppend(QuorumData* quorumData)
{
    Buffer          singleBuffer;
    DataMessage*    it;
    
    assert(quorumData->dataMessages.GetLength() > 0);
    
    if (!quorumData->context.IsAppending() && quorumData->dataMessages.GetLength() > 0)
    {
        Buffer& value = quorumData->context.GetNextValue();
        FOREACH(it, quorumData->dataMessages)
        {
            singleBuffer.Clear();
            it->Write(singleBuffer);
            if (value.GetLength() + 1 + singleBuffer.GetLength() < DATABASE_REPLICATION_SIZE)
                value.Appendf("%B", &singleBuffer);
            else
                break;
        }
        
        assert(value.GetLength() > 0);
        
        quorumData->context.Append();
//        quorumData->context.Append(quorumData->dataMessages.First());
    }
}

void ShardServer::FromClientRequest(ClientRequest* request, DataMessage* message)
{
    switch (request->type)
    {
        case CLIENTREQUEST_SET:
            message->type = DATAMESSAGE_SET;
            message->tableID = request->tableID;
            message->key.Wrap(request->key);
            message->value.Wrap(request->value);
            return;
        case CLIENTREQUEST_DELETE:
            message->type = DATAMESSAGE_DELETE;
            message->tableID = request->tableID;
            message->key.Wrap(request->key);
            return;
        default:
            ASSERT_FAIL();
    }
}

void ShardServer::OnRequestLeaseTimeout()
{
    QuorumData*     quorum;
    ClusterMessage  msg;
    uint64_t        nodeID;
    uint64_t        *nit;
    
    Log_Trace();
    
    nodeID = REPLICATION_CONFIG->GetNodeID();
    for (quorum = quorums.First(); quorum != NULL; quorum = quorums.Next(quorum))
    {
        quorum->proposalID = REPLICATION_CONFIG->NextProposalID(quorum->proposalID);
        msg.RequestLease(nodeID, quorum->quorumID, quorum->proposalID, PAXOSLEASE_MAX_LEASE_TIME);
        
        // send to all controllers
        for (nit = controllers.First(); nit != NULL; nit = controllers.Next(nit))
            CONTEXT_TRANSPORT->SendClusterMessage(*nit, msg);

        quorum->requestedLeaseExpireTime = EventLoop::Now() + PAXOSLEASE_MAX_LEASE_TIME;
    }
    
    if (!requestTimer.IsActive())
        EventLoop::Add(&requestTimer);
}

void ShardServer::OnPrimaryLeaseTimeout()
{
    uint64_t        now;
    QuorumData*     quorum;
    
    now = EventLoop::Now();

    for (quorum = quorums.First(); quorum != NULL; quorum = quorums.Next(quorum))
    {
        if (now < quorum->leaseTimeout.GetExpireTime())
            continue;

        quorum->isPrimary = false;
        EventLoop::Remove(&quorum->leaseTimeout);
        quorum->context.OnLeaseTimeout();
    }
}

void ShardServer::OnSetConfigState(ConfigState& configState_)
{
    QuorumData*             quorum;
    QuorumData*             next;
    ConfigQuorum*           configQuorum;
    ConfigQuorum::NodeList* nodes;
    uint64_t*               nit;
    uint64_t                nodeID;
    bool                    found;

    configState = configState_;
    
    nodeID = REPLICATION_CONFIG->GetNodeID();

    // look for removal
    for (quorum = quorums.First(); quorum != NULL; quorum = next)
    {
        configQuorum = configState.GetQuorum(quorum->quorumID);
        if (configQuorum == NULL)
        {
            // TODO: quorum has disappeared?
            ASSERT_FAIL();
        }
        
        found = false;
        next = quorums.Next(quorum);
        
        nodes = &configQuorum->activeNodes;
        for (nit = nodes->First(); nit != NULL; nit = nodes->Next(nit))
        {
            if (*nit == nodeID)
            {
                found = true;
                break;
            }
        }
        if (found)
            continue;

        nodes = &configQuorum->inactiveNodes;
        for (nit = nodes->First(); nit != NULL; nit = nodes->Next(nit))
        {
            if (*nit == nodeID)
            {
                found = true;
                break;
            }
        }
        if (found)
            continue;
        
        next = quorums.Remove(quorum);
        delete quorum;
    }
    
    // check changes in active or inactive node list
    for (configQuorum = configState.quorums.First();
     configQuorum != NULL;
     configQuorum = configState.quorums.Next(configQuorum))
    {
        nodes = &configQuorum->activeNodes;
        for (nit = nodes->First(); nit != NULL; nit = nodes->Next(nit))
        {
            if (*nit != nodeID)
                continue;
            ConfigureQuorum(configQuorum, true);
        }

        nodes = &configQuorum->inactiveNodes;
        for (nit = nodes->First(); nit != NULL; nit = nodes->Next(nit))
        {
            if (*nit != nodeID)
                continue;
            ConfigureQuorum(configQuorum, false);
        }
    }
    
    OnRequestLeaseTimeout();
}

void ShardServer::ConfigureQuorum(ConfigQuorum* configQuorum, bool active)
{
    QuorumData*         quorumData;
    ConfigShardServer*  shardServer;
    uint64_t*           nit;
    uint64_t            quorumID;
    uint64_t            nodeID;
    uint64_t            logCacheSize;
    
    nodeID = REPLICATION_CONFIG->GetNodeID();
    quorumID = configQuorum->quorumID;
    
    logCacheSize = configFile.GetIntValue("rlog.cacheSize", DEFAULT_RLOG_CACHE_SIZE);

    quorumData = LocateQuorum(quorumID);
    if (active && quorumData == NULL)
    {
        quorumData = new QuorumData;
        quorumData->quorumID = quorumID;
        quorumData->proposalID = 0;
        quorumData->context.Init(this, configQuorum, GetQuorumTable(quorumID), logCacheSize);
        quorumData->leaseTimeout.SetCallable(MFUNC(ShardServer, OnPrimaryLeaseTimeout));
        quorumData->isPrimary = false;
        quorumData->isActive = true;

        quorums.Append(quorumData);
        for (nit = configQuorum->activeNodes.First(); nit != NULL; nit = configQuorum->activeNodes.Next(nit))
        {
            shardServer = configState.GetShardServer(*nit);
            assert(shardServer != NULL);
            CONTEXT_TRANSPORT->AddNode(*nit, shardServer->endpoint);
        }
        CONTEXT_TRANSPORT->AddQuorumContext(&quorumData->context);
    }
    // TODO: add nodes to CONTEXT_TRANSPORT
//    else if (quorumData != NULL)
//        quorumData->context.UpdateConfig(configQuorum);
    
    UpdateShards(configQuorum->shards);
}

void ShardServer::OnReceiveLease(uint64_t quorumID, uint64_t proposalID)
{
    QuorumData*     quorum;
    
    quorum = LocateQuorum(quorumID);
    if (!quorum)
        return;
    
    if (quorum->proposalID != proposalID)
        return;
    
    quorum->isPrimary = true;
    quorum->leaseTimeout.SetExpireTime(quorum->requestedLeaseExpireTime);
    EventLoop::Reset(&quorum->leaseTimeout);
    
    quorum->context.OnLearnLease();
}

void ShardServer::UpdateShards(List<uint64_t>& shards)
{
    uint64_t*           sit;
    ConfigShard*        shard;
    StorageDatabase*    database;
    StorageTable*       table;
    Buffer              name;
    ReadBuffer          firstKey;

    for (sit = shards.First(); sit != NULL; sit = shards.Next(sit))
    {
        shard = configState.GetShard(*sit);
        assert(shard != NULL);
        
        if (!databases.Get(shard->databaseID, database))
        {
            name.Writef("%U", shard->databaseID);
            name.NullTerminate();

            database = databaseEnv.GetDatabase(name.GetBuffer());
            databases.Set(shard->databaseID, database);
        }
        
        if (!tables.Get(shard->tableID, table))
        {
            name.Writef("%U", shard->tableID);
            name.NullTerminate();
            
            table = database->GetTable(name.GetBuffer());
            assert(table != NULL);
            tables.Set(shard->tableID, table);
        }
        
        // check if key already exists
        firstKey.Wrap(shard->firstKey);
        if (!table->ShardExists(firstKey))
            table->CreateShard(*sit, firstKey);
    }
}

StorageTable* ShardServer::LocateTable(uint64_t tableID)
{
    StorageTable*   table;
    
    if (!tables.Get(tableID, table))
        return NULL;
    return table;
}

StorageShard* ShardServer::LocateShard(uint64_t /*shardID*/)
{
    // TODO: xxx
    return NULL;
}

StorageTable* ShardServer::GetQuorumTable(uint64_t quorumID)
{
    Buffer          qname;
    
    qname.Writef("%U", quorumID);
    qname.NullTerminate();
    return systemDatabase->GetTable(qname.GetBuffer());
}

void ShardServer::OnClientRequestGet(ClientRequest* request)
{
    StorageTable*   table;
    ReadBuffer      key;
    ReadBuffer      value;

    table = LocateTable(request->tableID);
    if (!table)
    {
        request->response.Failed();
        request->OnComplete();
        return;            
    }
    
    key.Wrap(request->key);
    if (!table->Get(key, value))
    {
        request->response.Failed();
        request->OnComplete();
        return;
    }
    
    request->response.Value(value);
    request->OnComplete();
}

QuorumData::QuorumData()
{
    prev = next = this;
    isPrimary = false;
    requestedLeaseExpireTime = 0;
}

