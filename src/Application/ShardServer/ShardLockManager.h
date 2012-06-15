#ifndef SHARDLOCKMANAGER_H
#define SHARDLOCKMANAGER_H

#include "System/Buffers/Buffer.h"
#include "System/Events/Countdown.h"
#include "System/Containers/InTreeMap.h"
#include "System/Containers/InNodeList.h"

#define LOCK_CHECK_FREQUENCY        (1000)      // msec
#define LOCK_EXPIRE_TIME            (3000)      // msec
#define LOCK_CACHE_TIME             (60*1000)   // msec
#define LOCK_CACHE_COUNT            (10*1000)
#define LOCK_POOL_COUNT             (10*1000)

/*
===============================================================================================

 ShardLock
 
===============================================================================================
*/

class ShardLock
{
    typedef InTreeNode<ShardLock> TreeNode;
    typedef InListNode<ShardLock> ListNode;

public:
    ShardLock();

    void            Init();
    
    bool            locked;
    uint64_t        expireTime;
    uint64_t        unlockTime;
    Buffer          key;

    TreeNode        treeNode;
    ListNode        listCacheNode;
    ListNode        listPoolNode;
    ListNode        listExpiryNode;
};

/*
===============================================================================================

 ShardLockManager
 
===============================================================================================
*/

class ShardLockManager
{
    typedef InTreeMap<ShardLock>                                LockTree;
    typedef InNodeList<ShardLock, &ShardLock::listCacheNode>    LockCacheList;
    typedef InNodeList<ShardLock, &ShardLock::listExpiryNode>   LockExpiryList;
    typedef InNodeList<ShardLock, &ShardLock::listPoolNode>     LockPoolList;

    // create ShardLock cache

public:
    ShardLockManager();

    void            Init();
    void            Shutdown(); // TODO
    
    // internal data structures stats
    unsigned        GetNumLocks();
    unsigned        GetTreeCount();
    unsigned        GetCacheListLength();
    unsigned        GetPoolListLength();
    unsigned        GetExpiryListLength();

    // set parameters
    void            SetLockExpireTime(unsigned lockExpireTime);
    void            SetMaxCacheTime(unsigned maxCacheTime);
    void            SetMaxCacheCount(unsigned maxCacheCount);
    void            SetMaxPoolCount(unsigned maxPoolCount);

    // get parameters
    unsigned        GetLockExpireTime();
    unsigned        GetMaxCacheTime();
    unsigned        GetMaxCacheCount();
    unsigned        GetMaxPoolCount();

    // locking interface
    bool            TryLock(ReadBuffer key);
    bool            IsLocked(ReadBuffer key);
    void            Unlock(ReadBuffer key);
    void            UnlockAll();
    
private:
    void            OnRemoveCachedLocks();
    void            OnExpireLocks();
    void            Unlock(ShardLock* lock);
    ShardLock*      NewLock();
    void            DeleteLock(ShardLock* lock);

    unsigned        numLocked;
    LockTree        lockTree;
    LockCacheList   lockCacheList;
    LockPoolList    lockPoolList;
    LockExpiryList  lockExpiryList;
    Countdown       removeCachedLocks;
    Countdown       expireLocks;

    unsigned        lockExpireTime;
    unsigned        maxCacheTime;
    unsigned        maxCacheCount;
    unsigned        maxPoolCount;
};

#endif
