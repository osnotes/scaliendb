#include "ReplicationTransport.h"
#include "ReplicationManager.h"
#include "Quorums/QuorumContext.h"
#include "System/Config.h"

void ReplicationTransport::Init()
{
	unsigned i;
	Endpoint endpoint;
	
	if (!reader.Init(configFile.GetIntValue("port", 10000)))
		STOP_FAIL("cannot bind reader port", 1);
	reader.SetOnRead(MFUNC(ReplicationTransport, OnRead));

	// create writers for controllers only
	for (i = 0; i < (unsigned)configFile.GetListNum("controller.endpoints"); i++)
	{
		endpoint.Set(configFile.GetListValue("controller.endpoints", i, ""));
		AddNode(i, endpoint);
	}
}

void ReplicationTransport::AddNode(uint64_t nodeID, const Endpoint& endpoint)
{
	Node* node;
	
	node = new Node;
	node->endpoint = endpoint;
	node->nodeID = nodeID;
	node->writer.Init(node->endpoint);
	nodes.Append(node);
}

void ReplicationTransport::Shutdown()
{
	Node*	it;
		
	reader.Close();
	
	for (it = nodes.Head(); it != NULL; it = nodes.Delete(it))
		it->writer.Close();
}

void ReplicationTransport::SendMessage(uint64_t nodeID,
 const Buffer& prefix, const Message& msg)
{
	Node*	it;

	for (it = nodes.Head(); it != NULL; it = nodes.Next(it))
	{
		if (it->nodeID == nodeID)
		{
			it->writer.Write(prefix, msg);
			break;
		}
	}
}

void ReplicationTransport::SendPriorityMessage(uint64_t nodeID,
 const Buffer& prefix, const Message& msg)
{
	Node*	it;

	for (it = nodes.Head(); it != NULL; it = nodes.Next(it))
	{
		if (it->nodeID == nodeID)
		{
			it->writer.WritePriority(prefix, msg);
			break;
		}
	}
}

ReadBuffer ReplicationTransport::GetMessage()
{
	return readBuffer;
}

void ReplicationTransport::OnRead()
{
	uint64_t logID;
	
	// TODO: parse
	
	RMAN->GetContext(logID)->OnMessage();
}
