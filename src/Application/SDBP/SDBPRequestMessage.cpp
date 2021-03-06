#include "SDBPRequestMessage.h"

bool SDBPRequestMessage::Read(ReadBuffer& buffer)
{
    int         read;
    char        transactional;
    unsigned    i, numNodes;
    uint64_t    nodeID;
    ReadBuffer  optional;
        
    if (buffer.GetLength() < 1)
        return false;
    
    read = buffer.Readf("%c:", &transactional);
    if (read == 2 && transactional == CLIENTREQUEST_TRANSACTIONAL)
    {
        request->transactional = true;
        buffer.Advance(read);
    }

    switch (buffer.GetCharAt(0))
    {
        /* Master query */
        case CLIENTREQUEST_GET_MASTER:
            read = buffer.Readf("%c:%U",
             &request->type, &request->commandID);
            break;

        /* Get config state: databases, tables, shards, quora */
        case CLIENTREQUEST_GET_CONFIG_STATE:
            read = buffer.Readf("%c:%U",
             &request->type, &request->commandID);
            break;

        /* Shard servers */
        case CLIENTREQUEST_UNREGISTER_SHARDSERVER:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID, &request->nodeID);
            break;        

        /* Quorum management */
        case CLIENTREQUEST_CREATE_QUORUM:
            read = buffer.Readf("%c:%U:%#B:%u",
             &request->type, &request->commandID, &request->name, &numNodes);
            if (read < 0 || read == (signed)buffer.GetLength())
                return false;
            buffer.Advance(read);
            for (i = 0; i < numNodes; i++)
            {
                read = buffer.Readf(":%U", &nodeID);
                if (read < 0)
                    return false;
                buffer.Advance(read);
                request->nodes.Append(nodeID);
            }
            if (buffer.GetLength() == 0)
                return true;
            else
                return false;
            break;
        case CLIENTREQUEST_RENAME_QUORUM:
            read = buffer.Readf("%c:%U:%U:%#B",
             &request->type, &request->commandID, &request->quorumID, &request->name);
            break;
        case CLIENTREQUEST_DELETE_QUORUM:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID, &request->quorumID);
            break;
        case CLIENTREQUEST_ADD_SHARDSERVER_TO_QUORUM:
            read = buffer.Readf("%c:%U:%U:%U",
             &request->type, &request->commandID, &request->quorumID, &request->nodeID);
            break;
        case CLIENTREQUEST_REMOVE_SHARDSERVER_FROM_QUORUM:
            read = buffer.Readf("%c:%U:%U:%U",
             &request->type, &request->commandID, &request->quorumID, &request->nodeID);
            break;
        case CLIENTREQUEST_ACTIVATE_SHARDSERVER:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID, &request->nodeID);
            break;

        /* Database management */
        case CLIENTREQUEST_CREATE_DATABASE:
            read = buffer.Readf("%c:%U:%#B",
             &request->type, &request->commandID, &request->name);
            break;
        case CLIENTREQUEST_RENAME_DATABASE:
            read = buffer.Readf("%c:%U:%U:%#B",
             &request->type, &request->commandID, &request->databaseID,
             &request->name);
            break;
        case CLIENTREQUEST_DELETE_DATABASE:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID, &request->databaseID);
            break;
        case CLIENTREQUEST_SPLIT_SHARD:
            read = buffer.Readf("%c:%U:%U:%#B",
             &request->type, &request->commandID,
             &request->shardID, &request->key);
            break;
        case CLIENTREQUEST_MIGRATE_SHARD:
            read = buffer.Readf("%c:%U:%U:%U",
             &request->type, &request->commandID, &request->shardID, &request->quorumID);
            return true;
            
        /* Table management */
        case CLIENTREQUEST_CREATE_TABLE:
            read = buffer.Readf("%c:%U:%U:%U:%#B",
             &request->type, &request->commandID, &request->databaseID,
             &request->quorumID, &request->name);
            break;
        case CLIENTREQUEST_RENAME_TABLE:
            read = buffer.Readf("%c:%U:%U:%#B",
             &request->type, &request->commandID,
             &request->tableID, &request->name);
            break;
        case CLIENTREQUEST_DELETE_TABLE:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID,
             &request->tableID);
            break;
        case CLIENTREQUEST_TRUNCATE_TABLE:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID,
             &request->tableID);
            break;
        case CLIENTREQUEST_FREEZE_TABLE:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID,
             &request->tableID);
            break;
        case CLIENTREQUEST_UNFREEZE_TABLE:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID,
             &request->tableID);
            break;
        case CLIENTREQUEST_FREEZE_DATABASE:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID,
             &request->databaseID);
            break;
        case CLIENTREQUEST_UNFREEZE_DATABASE:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID,
             &request->databaseID);
            break;            
        /* Data operations */
        case CLIENTREQUEST_GET:
            read = buffer.Readf("%c:%U:%U:%U:%U:%#B",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->tableID, &request->paxosID,
             &request->key);
            break;
        case CLIENTREQUEST_SET:
        case CLIENTREQUEST_SET_IF_NOT_EXISTS:
        case CLIENTREQUEST_GET_AND_SET:
            read = buffer.Readf("%c:%U:%U:%U:%#B:%#B",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->tableID, &request->key, &request->value);
            break;
        case CLIENTREQUEST_TEST_AND_SET:
            read = buffer.Readf("%c:%U:%U:%U:%#B:%#B:%#B",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->tableID, &request->key, &request->test, &request->value);
            break;
        case CLIENTREQUEST_TEST_AND_DELETE:
            read = buffer.Readf("%c:%U:%U:%U:%#B:%#B",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->tableID, &request->key, &request->test);
            break;
        case CLIENTREQUEST_ADD:
            read = buffer.Readf("%c:%U:%U:%U:%#B:%I",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->tableID, &request->key, &request->number);
            break;
        case CLIENTREQUEST_APPEND:
            read = buffer.Readf("%c:%U:%U:%U:%#B:%#B",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->tableID, &request->key, &request->value);
            break;
        case CLIENTREQUEST_DELETE:
        case CLIENTREQUEST_REMOVE:
            read = buffer.Readf("%c:%U:%U:%U:%#B",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->tableID, &request->key);
            break;
        case CLIENTREQUEST_SEQUENCE_SET:
            read = buffer.Readf("%c:%U:%U:%U:%#B:%U",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->tableID, &request->key, &request->sequence);
            break;
        case CLIENTREQUEST_SEQUENCE_NEXT:
            read = buffer.Readf("%c:%U:%U:%U:%#B",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->tableID, &request->key);
            break;
        case CLIENTREQUEST_LIST_KEYS:
        case CLIENTREQUEST_LIST_KEYVALUES:
            read = buffer.Readf("%c:%U:%U:%#B:%#B:%#B:%U:%b",
             &request->type, &request->commandID,
             &request->tableID, &request->key, &request->endKey, &request->prefix,
             &request->count, &request->forwardDirection);
            break;
        case CLIENTREQUEST_COUNT:
            read = buffer.Readf("%c:%U:%U:%U:%#B:%#B:%#B:%b",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->tableID, &request->key, &request->endKey, &request->prefix,
             &request->forwardDirection);
            break;
        
        /* Transactions */
        case CLIENTREQUEST_START_TRANSACTION:
            read = buffer.Readf("%c:%U:%U:%U:%#B",
             &request->type, &request->commandID, &request->configPaxosID,
             &request->quorumID, &request->key);
            break;
        case CLIENTREQUEST_COMMIT_TRANSACTION:
        case CLIENTREQUEST_ROLLBACK_TRANSACTION:
            read = buffer.Readf("%c:%U:%U",
             &request->type, &request->commandID, &request->quorumID);
            break;
            
        default:
            return false;
    }
    
    return (read == (signed)buffer.GetLength() ? true : false);
}

bool SDBPRequestMessage::Write(Buffer& buffer)
{
    uint64_t*   it;
    
    if (request->transactional)
        buffer.Appendf("%c:", CLIENTREQUEST_TRANSACTIONAL);

    switch (request->type)
    {
        /* Master query */
        case CLIENTREQUEST_GET_MASTER:
            buffer.Appendf("%c:%U",
             request->type, request->commandID);
            return true;

        /* Get config state: databases, tables, shards, quora */
        case CLIENTREQUEST_GET_CONFIG_STATE:
            buffer.Appendf("%c:%U",
             request->type, request->commandID);
            return true;

        /* Shard servers */
        case CLIENTREQUEST_UNREGISTER_SHARDSERVER:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID, request->nodeID);
            return true;

        /* Quorum management */
        case CLIENTREQUEST_CREATE_QUORUM:
            buffer.Appendf("%c:%U:%#B:%u",
             request->type, request->commandID, &request->name, request->nodes.GetLength());
            for (it = request->nodes.First(); it != NULL; it = request->nodes.Next(it))
                buffer.Appendf(":%U", *it);
            return true;
        case CLIENTREQUEST_RENAME_QUORUM:
            buffer.Appendf("%c:%U:%U:%#B",
             request->type, request->commandID, request->quorumID, &request->name);
            return true;
        case CLIENTREQUEST_DELETE_QUORUM:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID, request->quorumID);
            return true;
        case CLIENTREQUEST_ADD_SHARDSERVER_TO_QUORUM:
            buffer.Appendf("%c:%U:%U:%U",
             request->type, request->commandID, request->quorumID, request->nodeID);
            return true;
        case CLIENTREQUEST_REMOVE_SHARDSERVER_FROM_QUORUM:
            buffer.Appendf("%c:%U:%U:%U",
             request->type, request->commandID, request->quorumID, request->nodeID);
            return true;
        case CLIENTREQUEST_ACTIVATE_SHARDSERVER:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID, request->nodeID);
            return true;

        /* Database management */
        case CLIENTREQUEST_CREATE_DATABASE:
            buffer.Appendf("%c:%U:%#B",
             request->type, request->commandID, &request->name);
            return true;
        case CLIENTREQUEST_RENAME_DATABASE:
            buffer.Appendf("%c:%U:%U:%#B",
             request->type, request->commandID, request->databaseID,
             &request->name);
            return true;
        case CLIENTREQUEST_DELETE_DATABASE:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID, request->databaseID);
            return true;
        case CLIENTREQUEST_SPLIT_SHARD:
            buffer.Appendf("%c:%U:%U:%#B",
             request->type, request->commandID, request->shardID, &request->key);
            return true;
        case CLIENTREQUEST_MIGRATE_SHARD:
            buffer.Appendf("%c:%U:%U:%U",
             request->type, request->commandID, request->shardID, request->quorumID);
            return true;

        /* Table management */
        case CLIENTREQUEST_CREATE_TABLE:
            buffer.Appendf("%c:%U:%U:%U:%#B",
             request->type, request->commandID, request->databaseID,
             request->quorumID, &request->name);
            return true;
        case CLIENTREQUEST_RENAME_TABLE:
            buffer.Appendf("%c:%U:%U:%#B",
             request->type, request->commandID,
             request->tableID, &request->name);
            return true;
        case CLIENTREQUEST_DELETE_TABLE:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID,
             request->tableID);
            return true;
        case CLIENTREQUEST_TRUNCATE_TABLE:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID,
             request->tableID);
            return true;
        case CLIENTREQUEST_FREEZE_TABLE:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID,
             request->tableID);
            return true;
        case CLIENTREQUEST_UNFREEZE_TABLE:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID,
             request->tableID);
            return true;
        case CLIENTREQUEST_FREEZE_DATABASE:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID,
             request->databaseID);
            return true;
        case CLIENTREQUEST_UNFREEZE_DATABASE:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID,
             request->databaseID);
            return true;

        /* Data operations */
        case CLIENTREQUEST_GET:
            buffer.Appendf("%c:%U:%U:%U:%U:%#B",
             request->type, request->commandID, request->configPaxosID,
             request->tableID, request->paxosID,
             &request->key);
            return true;
        case CLIENTREQUEST_SET:
        case CLIENTREQUEST_SET_IF_NOT_EXISTS:
        case CLIENTREQUEST_GET_AND_SET:
            buffer.Appendf("%c:%U:%U:%U:%#B:%#B",
             request->type, request->commandID, request->configPaxosID,
             request->tableID, &request->key, &request->value);
            return true;
        case CLIENTREQUEST_TEST_AND_SET:
            buffer.Appendf("%c:%U:%U:%U:%#B:%#B:%#B",
             request->type, request->commandID, request->configPaxosID,
             request->tableID, &request->key, &request->test, &request->value);
            return true;
        case CLIENTREQUEST_ADD:
            buffer.Appendf("%c:%U:%U:%U:%#B:%I",
             request->type, request->commandID, request->configPaxosID,
             request->tableID, &request->key, request->number);
            return true;
        case CLIENTREQUEST_APPEND:
            buffer.Appendf("%c:%U:%U:%U:%#B:%#B",
             request->type, request->commandID, request->configPaxosID,
             request->tableID, &request->key, &request->value);
            return true;            
        case CLIENTREQUEST_DELETE:
        case CLIENTREQUEST_REMOVE:
            buffer.Appendf("%c:%U:%U:%U:%#B",
             request->type, request->commandID, request->configPaxosID,
             request->tableID, &request->key);
            return true;
        case CLIENTREQUEST_TEST_AND_DELETE:
            buffer.Appendf("%c:%U:%U:%U:%#B:%#B",
             request->type, request->commandID, request->configPaxosID,
             request->tableID, &request->key, &request->test);
            return true;
        case CLIENTREQUEST_SEQUENCE_SET:
            buffer.Appendf("%c:%U:%U:%U:%#B:%U",
             request->type, request->commandID, request->configPaxosID,
             request->tableID, &request->key, request->sequence);
            return true;
        case CLIENTREQUEST_SEQUENCE_NEXT:
            buffer.Appendf("%c:%U:%U:%U:%#B",
             request->type, request->commandID, request->configPaxosID,
             request->tableID, &request->key);
            return true;
        case CLIENTREQUEST_LIST_KEYS:
        case CLIENTREQUEST_LIST_KEYVALUES:
            buffer.Appendf("%c:%U:%U:%#B:%#B:%#B:%U:%b",
             request->type, request->commandID,
             request->tableID, &request->key, &request->endKey, &request->prefix,
             request->count, request->forwardDirection);
            return true;
        case CLIENTREQUEST_COUNT:
            buffer.Appendf("%c:%U:%U:%U:%#B:%#B:%#B:%b",
             request->type, request->commandID, request->configPaxosID,
             request->tableID, &request->key, &request->endKey, &request->prefix,
             request->forwardDirection);
            return true;
        
        /* Transactions */
        case CLIENTREQUEST_START_TRANSACTION:
            buffer.Appendf("%c:%U:%U:%U:%#B",
             request->type, request->commandID, request->configPaxosID,
             request->quorumID, &request->key);
            return true;            
        case CLIENTREQUEST_COMMIT_TRANSACTION:
        case CLIENTREQUEST_ROLLBACK_TRANSACTION:
            buffer.Appendf("%c:%U:%U",
             request->type, request->commandID, request->quorumID);
            return true;            

        default:
            return false;
    }
}
