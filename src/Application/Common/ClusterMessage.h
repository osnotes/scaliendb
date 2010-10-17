#ifndef CLUSTERMESSAGE_H
#define CLUSTERMESSAGE_H

#include "System/Common.h"
#include "Framework/Messaging/Message.h"
#include "Application/ConfigState/ConfigState.h"

#define CLUSTERMESSAGE_SET_NODEID       'N' // master => shard server
#define CLUSTERMESSAGE_HEARTBEAT        'H' // shard server => controllers
#define CLUSTERMESSAGE_SET_CONFIG_STATE 'C' // master => shard server
#define CLUSTERMESSAGE_REQUEST_LEASE    'R' // shard server => master, also serves as heartbeat
#define CLUSTERMESSAGE_RECEIVE_LEASE    'r' // master => shard server
#define CLUSTERMESSAGE_CONFIG_CATCHUP   'c'

/*
===============================================================================================

 ClusterMessage

===============================================================================================
*/

class ClusterMessage : public Message
{
public:
    char                    type;
    uint64_t                nodeID;
    uint64_t                quorumID;
    uint64_t                shardID;
    uint64_t                proposalID;
    unsigned                duration;
    uint64_t                configID;
    ConfigQuorum::NodeList  activeNodes;
    QuorumPaxosID::List     quorumPaxosIDs;
    ConfigState             configState;
    
    ClusterMessage();
    
    bool            SetNodeID(uint64_t nodeID);
    bool            Heartbeat(uint64_t nodeID, QuorumPaxosID::List& quorumPaxosIDs);
    bool            SetConfigState(ConfigState& configState);
    bool            RequestLease(uint64_t nodeID, uint64_t quorumID,
                     uint64_t proposalID, unsigned duration, uint64_t configID);
    bool            ReceiveLease(uint64_t nodeID, uint64_t quorumID,
                     uint64_t proposalID, unsigned duration, ConfigQuorum::NodeList activeNodes);
    
    bool            Read(ReadBuffer& buffer);
    bool            Write(Buffer& buffer);
};

#endif
