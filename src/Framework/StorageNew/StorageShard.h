#ifndef STORAGESHARD_H
#define STORAGESHARD_H

#include "System/Buffers/Buffer.h"
#include "System/Containers/SortedList.h"
#include "StorageChunkMemory.h"
#include "StorageFileChunk.h"

/*
===============================================================================================

 StorageShard

===============================================================================================
*/

class StorageShard
{
    typedef SortedList<StorageChunk*> ChunkList;

public:
    uint64_t            GetTableID();
    uint64_t            GetShardID();

    ReadBuffer          GetFirstKey();
    ReadBuffer          GetLastKey();
    bool                RangeContains(ReadBuffer& key);

    StorageChunkMemory* GetMemoChunk();
    void                SetNewMemoChunk(StorageChunkMemory* memoChunk);

private:
    uint64_t            shardID;
    uint64_t            tableID;

    ChunkList           chunks; // contains all active chunks, in order, first is the write chunk

    Buffer              firstKey;
    Buffer              lastKey;
};

#endif
