#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "ConfigMessage.h"
#include "ConfigContext.h"
#include "SDBPControllerContext.h"
#include "System/Containers/InList.h"
#include "System/Containers/InSortedList.h"
#include "System/Events/Timer.h"
#include "Application/Common/ClusterContext.h"
#include "Application/Common/ClientRequest.h"
#include "State/ConfigState.h"

class ClientSession;			// forward
class PrimaryLease;				// forward

/*
===============================================================================================

 Controller

===============================================================================================
*/

class Controller : public ClusterContext, public SDBPContext
{
public:
	typedef InList<ConfigMessage>		MessageList;
	typedef InSortedList<PrimaryLease>	LeaseList;
	typedef InList<ClientRequest>		RequestList;

	void			Init();
	
	// ========================================================================================
	// SDBPContext interface:
	//
	bool			IsValidClientRequest(ClientRequest* request);
	void			OnClientRequest(ClientRequest* request);
	void			OnClientClose(ClientSession* session);
	// ========================================================================================

	// For ConfigContext
	void			OnLearnLease();
	void			OnAppend(ConfigMessage& message, bool ownAppend);

	// ========================================================================================
	// ClusterContext interface:
	//
	void			OnClusterMessage(uint64_t nodeID, ClusterMessage& msg);
	void			OnIncomingConnectionReady(uint64_t nodeID, Endpoint endpoint);
	void			OnAwaitingNodeID(Endpoint endpoint);
	// ========================================================================================

private:
	void			FromRequest(ClientRequest* request, ConfigMessage* message);
	void			ToResponse(ConfigMessage* message, ClientResponse* response);
	void			OnPrimaryLeaseTimeout();
	void			InitConfigContext();
	void			TryRegisterShardServer(Endpoint& endpoint);
	void			SendClientReply(ConfigMessage& message);
	void			OnRequestLease(ClusterMessage& message);
	void			AssignPrimaryLease(ConfigQuorum& quorum, ClusterMessage& message);
	void			ExtendPrimaryLease(ConfigQuorum& quorum, ClusterMessage& message);
	void			UpdatePrimaryLeaseTimer();
	
	ConfigContext	configContext;
	MessageList		configMessages;
	ConfigState		configState;
	LeaseList		primaryLeases;
	Timer			primaryLeaseTimeout;
	RequestList		requests;
	RequestList		listenRequests;
};

class PrimaryLease
{
public:
	PrimaryLease()	{ prev = next = this; }
	uint64_t		quorumID;
	uint64_t		nodeID;
	uint64_t		expireTime;
	
	PrimaryLease*	prev;
	PrimaryLease*	next;
};

inline bool LessThan(PrimaryLease &a, PrimaryLease &b)
{
	return (a.expireTime < b.expireTime);
}

#endif
