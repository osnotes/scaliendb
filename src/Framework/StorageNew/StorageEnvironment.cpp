#include "StorageEnvironment.h"

StorageEnvironment::StorageEnvironment()
{
    commitStatus = false;
}

void StorageEnvironment::SetStorageConfig(StorageConfig& config_)
{
    config = config_;
}

uint64_t StorageEnvironment::GetShardID(uint64_t tableID, ReadBuffer& key)
{
    StorageShard* it;

    FOREACH(it, shards)
    {
        if (it->GetTableID() == tableID)
        {
            if (it->RangeContains(key))
                return it->GetShardID();
        }
    }

    return 0;
}

bool StorageEnvironment::Get(uint64_t shardID, ReadBuffer& key, ReadBuffer& value)
{
    StorageShard*       shard;
    StorageChunk*       itChunk;    
    StorageKeyValue*    kv;
    ReadBuffer          firstKey;
    ReadBuffer          lastKey;

    shard = GetShard(shardID);
    if (shard == NULL)
        return false;

    firstKey = shard->GetFirstKey();
    lastKey = shard->GetLastKey();

    FOREACH(itChunk, shard->chunks)
    {
        kv = itChunk->Get(firstKey, lastKey, key);
        if (kv != NULL)
        {
            if (kv.IsDelete())
            {
                return false;
            }
            if (kv.IsSet())
            {
                value.Wrap(kv->GetValue());
                return true;
            }
        }
    }

    return false;
}

bool StorageEnvironment::Set(uint64_t shardID, ReadBuffer& key, ReadBuffer& value)
{
    uint64_t        commandID;
    StorageShard*   shard;
    StorageChunk*   chunk;

    shard = GetShard(shardID);
    if (shard == NULL)
        return false;

    commandID = activeLogSegment->AppendSet(shardID, key, value);
    if (commandID < 0)
        return false;

    chunk = shard->GetWriteChunk();
    assert(chunk != NULL);
    if (!chunk->Set(key, value))
    {
        activeLogSegment->Undo();
        return false;
    }
    chunk->RegisterLogCommand(activeLogSegment->GetLogSegmentID(), commandID);

    return true;
}

bool StorageEnvironment::Delete(uint64_t shardID, ReadBuffer& key)
{
    StorageShard* shard;

    shard = GetShard(shardID);
    if (shard == NULL)
        return false;

    commandID = activeLogSegment->AppendDelete(shardID, key, value);
    if (commandID < 0)
        return false;

    chunk = shard->GetWriteChunk();
    assert(chunk != NULL);
    if (!chunk->Delete(key))
    {
        activeLogSegment->Undo();
        return false;
    }
    chunk->RegisterLogCommand(activeLogSegment->GetLogSegmentID(), commandID);

    return true;
}

void StorageEnvironment::SetOnCommit(Callable& onCommit_)
{
    onCommit = onCommit_;
}

void StorageEnvironment::Commit()
{
    // start async commit
}

bool StorageEnvironment::GetCommitStatus()
{
    return commitStatus;
}

StorageShard* StorageEnvironment::GetShard(uint64_t shardID)
{
    StorageShard* it;

    FOREACH(it, shards)
    {
        if (it->GetShardID() == shardID)
            return it;
    }

    return NULL;
}

StorageChunk* StorageEnvironment::GetChunk(uint64_t chunkID)
{
    StorageChunk* it;

    FOREACH(it, chunks)
    {
        if (it->GetChunkID() == chunkID)
            return it;
    }

    return NULL;
}

void StorageEnvironment::OnCommit()
{
    // set commit status

    TryFinalizeLogSegment();
    TryFinalizeChunks();
}

void StorageEnvironment::TryFinalizeLogSegment()
{
    uint64_t    logSegmentID;
    Buffer      filename;

    if (activeLogSegment->GetSize() < config.logSegmentSize)
        return;

    activeLogSegment->Finalize();
    activeLogSegment->Close();

    logSegmentID = activeLogSegment->GetLogSegmentID();
    logSegmentID++;

    logSegments.Append(activeLogSegment);

    headLogSegment = new StorageLogSegment();
    headLogSegment->SetLogSegmentID(logSegmentID);
    buffer.Writef("log.%020U", logSegmentID);
    buffer.NullTerminate();
    headLogSegment->Open(buffer);
}

void StorageEnvironment::TryFinalizeChunks()
{
    StorageChunk*   itChunk;
    StorageShard*   itShard;
    StorageChunk*   newChunk;

    FOREACH(itChunk, chunks)
    {
        if (itChunk->GetSize() > config.chunkSize)
            itChunk->Finalize();
    }

    FOREACH(itShard, shards)
    {
        if (!itShard->GetWriteChunk()->IsFinalized())
            continue;

        newChunk = new StorageChunk;
        newChunk->SetChunkID(NextChunkID(activeChunk->GetChunkID()));
        newChunk->SetBloomFilter(itShard->UseBloomFilter());
        itShard->SetNewWriteChunk(newChunk);
        chunks.Append(newChunk);
    }
}

bool StorageEnvironment::IsWriteActive()
{

}
