#ifndef REPLICATEDLOG_H
#define REPLICATEDLOG_H

#include "Framework/Replication/Quorums/QuorumContext.h"
#include "Framework/Replication/Paxos/PaxosProposer.h"
#include "Framework/Replication/Paxos/PaxosAcceptor.h"

#define REQUEST_CHOSEN_TIMEOUT  7000

/*
===============================================================================

 ReplicatedLog

===============================================================================
*/

class ReplicatedLog
{
public:
    void                    Init(QuorumContext* context);

    bool                    IsMultiPaxosEnabled();

    void                    TryAppendNextValue();
    
    void                    OnMessage(PaxosMessage& msg);
    void                    SetPaxosID(uint64_t paxosID);
    uint64_t                GetPaxosID();
    
    void                    RegisterPaxosID(uint64_t paxosID, uint64_t nodeID);
    void                    OnMessage();
    bool                    IsAppending();
    
    void                    OnCatchupComplete(uint64_t paxosID);
    void                    OnLearnLease();
    void                    OnLeaseTimeout();

private:
    void                    Append(Buffer& value);

    void                    OnPrepareRequest(PaxosMessage& msg);
    void                    OnPrepareResponse(PaxosMessage& msg);
    void                    OnProposeRequest(PaxosMessage& msg);
    void                    OnProposeResponse(PaxosMessage& msg);
    void                    OnLearnChosen(PaxosMessage& msg);
    void                    OnRequestChosen(PaxosMessage& msg);
    void                    OnStartCatchup(PaxosMessage& msg);

    void                    ProcessLearnChosen(uint64_t nodeID, uint64_t runID, ReadBuffer value);

    void                    OnRequest(PaxosMessage& msg);
    void                    NewPaxosRound();
    void                    RequestChosen(uint64_t nodeID);

    QuorumContext*          context;

    uint64_t                paxosID;
    PaxosProposer           proposer;
    PaxosAcceptor           acceptor;
    
    uint64_t                lastRequestChosenTime;
    uint64_t                lastRequestChosenPaxosID;
};
#endif
