#ifndef CONFIGTABLE_H
#define CONFIGTABLE_H

#include "System/Common.h"
#include "System/Containers/List.h"
#include "System/Buffers/Buffer.h"

/*
===============================================================================================

 ConfigTable

===============================================================================================
*/

class ConfigTable
{
public:
    ConfigTable();
    ConfigTable(const ConfigTable& other);
    
    ConfigTable&    operator=(const ConfigTable& other);

    uint64_t        databaseID;
    uint64_t        tableID;
    Buffer          name;
    
    List<uint64_t>  shards;
    
    ConfigTable*    prev;
    ConfigTable*    next;
};

#endif