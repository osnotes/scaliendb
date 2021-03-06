#ifndef STORAGECHUNKWRITER_H
#define STORAGECHUNKWRITER_H

#include "FDGuard.h"
#include "StorageFileChunk.h"

class StorageEnvironment;

/*
===============================================================================================

 StorageChunkWriter

===============================================================================================
*/

class StorageChunkWriter
{
public:
    bool                    Write(StorageEnvironment* env, StorageFileChunk* file);

private:
    bool                    WriteBuffer();

    bool                    WriteHeaderPage();
    bool                    WriteDataPages();
    bool                    WriteIndexPage();
    bool                    WriteBloomPage();

    StorageEnvironment*     env;
    StorageFileChunk*       file;
    FDGuard                 fd;
    Buffer                  writeBuffer;
};

#endif
