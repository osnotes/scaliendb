#include "ContextTransport.h"
#include "ReplicationConfig.h"
#include "Quorums/QuorumContext.h"
#include "System/Config.h"

static uint64_t Hash(uint64_t ID)
{
	return ID;
}

ContextTransport* replicationTransport = NULL;

ContextTransport* ContextTransport::Get()
{
	if (replicationTransport == NULL)
		replicationTransport = new ContextTransport();
	
	return replicationTransport;
}

ContextTransport::ContextTransport()
{
	clusterContext = NULL;
}

void ContextTransport::SetClusterContext(ClusterContext* context)
{
	clusterContext = context;
}

ClusterContext* ContextTransport::GetClusterContext()
{
	return clusterContext;
}

void ContextTransport::AddQuorumContext(QuorumContext* context)
{
	uint64_t contextID = context->GetContextID();
	contextMap.Set(contextID, context);
}

QuorumContext* ContextTransport::GetQuorumContext(uint64_t contextID)
{
	QuorumContext* pcontext;
	
	assert(contextMap.HasKey(contextID));
	pcontext = NULL;
	contextMap.Get(contextID, pcontext);
	
	return pcontext;
}

void ContextTransport::SendClusterMessage(uint64_t nodeID, Message& msg)
{
	Buffer prefix;
	
	prefix.Writef("%c:", PROTOCOL_CLUSTER);
	ClusterTransport::SendMessage(nodeID, prefix, msg);
}

void ContextTransport::SendMessage(uint64_t nodeID, uint64_t contextID, Message& msg)
{
	Buffer prefix;
	
	prefix.Writef("%c:%U", PROTOCOL_QUORUM, contextID);
	ClusterTransport::SendMessage(nodeID, prefix, msg);
}

void ContextTransport::SendPriorityMessage(uint64_t nodeID, uint64_t contextID, Message& msg)
{
	Buffer prefix;
	
	prefix.Writef("%c:%U", PROTOCOL_QUORUM, contextID);
	ClusterTransport::SendPriorityMessage(nodeID, prefix, msg);
}

void ContextTransport::OnConnectionReady(uint64_t nodeID, Endpoint endpoint)
{
	if (clusterContext)
		clusterContext->OnIncomingConnectionReady(nodeID, endpoint);
}

void ContextTransport::OnAwaitingNodeID(Endpoint endpoint)
{
	if (clusterContext)
		clusterContext->OnAwaitingNodeID(endpoint);
}

void ContextTransport::OnMessage(ReadBuffer msg)
{
	int			nread;\
	char		proto;
	
	Log_Trace("%.*s", P(&msg));
	
	if (msg.GetLength() < 2)
		ASSERT_FAIL();

	nread = msg.Readf("%c:", &proto);
	if (nread < 2)
		ASSERT_FAIL();

	msg.Advance(2);
	
	switch (proto)
	{
		case PROTOCOL_CLUSTER:
			OnClusterMessage(msg);
			break;
		case PROTOCOL_QUORUM:
			OnQuorumMessage(msg);
			break;
		default:
			ASSERT_FAIL();
			break;
	}
}

void ContextTransport::OnClusterMessage(ReadBuffer& msg)
{
	if (clusterContext)
		clusterContext->OnClusterMessage(msg);
}

void ContextTransport::OnQuorumMessage(ReadBuffer& msg)
{
	int			nread;
	uint64_t	contextID;

	nread = msg.Readf("%U:", &contextID);
	if (nread < 2)
		ASSERT_FAIL();
	
	msg.Advance(nread);

	GetQuorumContext(contextID)->OnMessage(msg);
}
