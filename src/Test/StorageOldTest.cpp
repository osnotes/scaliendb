#include "Test.h"

#include "Framework/Storage/StorageEnvironment.h"
#include "Framework/Storage/StorageDataPage.h"
#include "Framework/Storage/StorageDataCache.h"
#include "System/FileSystem.h"
#include "System/Stopwatch.h"
#include "System/Common.h"

#define TEST_DATABASE_PATH "."
#define TEST_DATABASE "db"

#define PRINTKV() \
{ \
    v.Write(rv); v.NullTerminate(); k.NullTerminate(); \
    TEST_LOG("%s => %s", k.GetBuffer(), v.GetBuffer()); \
}

int TestStorageCursorWalk(const char *name)
{
    StorageDatabase     db;
    StorageTable*       table;
    StorageDataCache*   cache;
    StorageCursor*      cursor;
    StorageKeyValue*    kv;
    Buffer              prevKey, prevValue;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;

    cache = DCACHE;
    DCACHE->Init(100000000);

    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(name);
    cursor = new StorageCursor(table);
    
    for (kv = cursor->Begin(); kv != NULL; kv = cursor->Next())
    {
        TEST_ASSERT((prevKey.GetLength() == 0 && kv->key.GetLength() == 0) ||
         ReadBuffer::Cmp(ReadBuffer(prevKey), kv->key) < 0);
        
        prevKey.Write(kv->key);
        prevValue.Write(kv->value);
    }

    db.Close();
    DCACHE->Shutdown();
        
    return TEST_SUCCESS;
}

// This can be used for ensuring the database is in a blank state
// It is intentionally named with zero in the middle of the name in 
// order to became first when sorted alphabetically.
TEST_DEFINE(TestStorage0DeleteTestDatabase)
{
    Buffer  path;
    char    sep;
    
    sep = FS_Separator();
    path.Append(TEST_DATABASE_PATH);
    path.Append(&sep, 1);
    path.Append(TEST_DATABASE);
    path.NullTerminate();
    
    if (!FS_Exists(path.GetBuffer()))
        return TEST_SUCCESS;
    if (!FS_IsDirectory(path.GetBuffer()))
        TEST_FAIL();
        
    if (!FS_RecDeleteDir(path.GetBuffer()))
        TEST_FAIL();

    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageBasic)
{
    StorageDatabase     db;
    StorageTable*       table;
    StorageDataCache*   cache;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;
    long                elapsed;
    unsigned            len;
    char*               area;
    char*               p;
    uint64_t            clock;
    
    // Initialization ==============================================================================
    StartClock();

    const unsigned num = 100*1000;
    const unsigned ksize = 20;
    const unsigned vsize = 128;
    area = (char*) malloc(num*(ksize+vsize));

    cache = DCACHE;
    DCACHE->Init(100000000);

    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(__func__);

    //==============================================================================================
    //
    // TestStorage SET test
    //
    // This test might not work on fast machines, because it commits every 1000 msec, but if there
    // are more sets between commits than the cache can contain, then it will assert!
    //
    //==============================================================================================
    clock = NowClock();
    sw.Start();
    for (unsigned i = 0; i < num; i++)
    {
        p = area + i*(ksize+vsize);
        len = snprintf(p, ksize, "%d", i);
        rk.SetBuffer(p);
        rk.SetLength(len);
        p += ksize;
        len = snprintf(p, vsize, "%.100f", (float) i);
        rv.SetBuffer(p);
        rv.SetLength(len);
        table->Set(rk, rv, false);

        if (NowClock() - clock >= 1000)
        {
            TEST_LOG("syncing...");
            db.Commit();
            clock = NowClock();
        }
    }
    db.Commit();
    elapsed = sw.Stop();
    TEST_LOG("%u sets took %ld msec", num, elapsed);

    // GET all keys ================================================================================
    sw.Reset();
    sw.Start();
    for (unsigned i = 0; i < num; i++)
    {
        k.Writef("%d", i);
        if (table->Get(k, rv))
            ;//PRINTKV()
        else
            ASSERT_FAIL();
    }   
    elapsed = sw.Stop();
    TEST_LOG("%u gets took %ld msec", num, elapsed);

    // DELETE all keys =============================================================================
    sw.Reset();
    sw.Start();
    for (unsigned i = 0; i < num; i++)
    {
        k.Writef("%d", i);
        TEST_ASSERT(table->Delete(k));
    }
    db.Commit();
    elapsed = sw.Stop();
    TEST_LOG("%u deletes took %ld msec", num, elapsed);
    
    // Shutdown ====================================================================================
    sw.Reset();
    sw.Start();
    db.Close();
    elapsed = sw.Stop();
    TEST_LOG("Close() took %ld msec", elapsed);
    
    DCACHE->Shutdown();
    free(area);
    
    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageCapacity)
{
    StorageDatabase     db;
    StorageTable*       table;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;
    long                elapsed;
    unsigned            len;
    char*               area;
    char*               p;
    unsigned            round;

    // Initialization ==============================================================================
    round = 1000;
    const unsigned num = 100*1000;
    const unsigned ksize = 20;
    const unsigned vsize = 128;
    area = (char*) malloc(num*(ksize+vsize));

    DCACHE->Init(100000000);
    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(__func__);

    //==============================================================================================
    //
    // test the number of SETs depending on the size of DCACHE and transaction size
    // e.g. a million key-value pairs take up 248M disk space
    //
    //==============================================================================================
    for (unsigned r = 0; r < round; r++)
    {
        // Set key-values ==========================================================================
        sw.Reset();
        for (unsigned i = 0; i < num; i++)
        {
            p = area + i*(ksize+vsize);
            len = snprintf(p, ksize, "%011d", i + r * num); // takes 100 ms
            rk.SetBuffer(p);
            rk.SetLength(len);
            //printf("%s\n", p);
            p += ksize;
            len = snprintf(p, vsize, "%.100f", (float) i + r * num); // takes 100 ms
            rv.SetBuffer(p);
            rv.SetLength(len);
            sw.Start();
            table->Set(rk, rv, true);
            sw.Stop();
        }
        TEST_LOG("Round %u: %u sets took %ld msec", r, num, (long) sw.Elapsed());

        // Read back key-values ====================================================================
        sw.Restart();
        for (unsigned i = 0; i < num; i++)
        {
            char key[ksize];
            len = snprintf(key, ksize, "%011d", i + r * num); // takes 100 ms
            rk.SetBuffer(key);
            rk.SetLength(len);
            // TODO: change rk to k for crash!
            // The problem is that creating and writing files does not use recovery
            // so if the program crashes while creating new files, the result will
            // be an inconsistent database.
            if (table->Get(rk, rv))
                ;//PRINT()
            else
                ASSERT_FAIL();
        }   
        elapsed = sw.Stop();
        TEST_LOG("Round %u: %u gets took %ld msec", r, num, elapsed);

        // Commit changes ==========================================================================
        sw.Reset();
        sw.Start();
        table->Commit(true /*recovery*/, true /*flush*/);
        elapsed = sw.Stop();
        TEST_LOG("Round %u: Commit() took %ld msec", r, elapsed);
    }

    // Shutdown ====================================================================================
    db.Close();
    DCACHE->Shutdown();
    free(area);

    return TEST_SUCCESS;
}

int UInt64ToBuffer(char* buf, size_t bufsize, uint64_t value);
TEST_DEFINE(TestStorageCapacitySet)
{
    StorageDatabase     db;
    StorageTable*       table;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;
    long                elapsed;
    unsigned            num, len, ksize, vsize;
    char*               area;
    char*               p;
    unsigned            round;

    // Initialization ==============================================================================
    round = 1000;
    num = 100*1000;
    ksize = 20;
    vsize = 128;
    area = (char*) malloc(num*(ksize+vsize));

    DCACHE->Init(100000000);
    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(__func__);

    //==============================================================================================
    //
    // test the number of SETs depending on the size of DCACHE and transaction size
    // e.g. a million key-value pairs take up 248M disk space
    //
    //==============================================================================================
    for (unsigned r = 0; r < round; r++)
    {
        // Set key-values ==========================================================================
        sw.Reset();
        for (unsigned i = 0; i < num; i++)
        {
            p = area + i*(ksize+vsize);
            len = UInt64ToBuffer(p, ksize, i + r * num);
            rk.SetBuffer(p);
            rk.SetLength(len);
            //printf("%s\n", p);
            p += ksize;
            //len = snprintf(p, vsize, "%.100f", (float) i + r * num); // takes 100 ms
            len = vsize;
            rv.SetBuffer(p);
            rv.SetLength(len);
            sw.Start();
            table->Set(rk, rv, false);
            sw.Stop();
        }
        TEST_LOG("Round %u: %u sets took %ld msec", r, num, (long) sw.Elapsed());

        // Commit changes ==========================================================================
        sw.Reset();
        sw.Start();
        table->Commit(true /*recovery*/, false /*flush*/);
        elapsed = sw.Stop();
        TEST_LOG("Round %u: Commit() took %ld msec", r, elapsed);
    }

    // Shutdown ====================================================================================
    db.Close();
    DCACHE->Shutdown();
    free(area);

    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageBigTransaction)
{
    StorageDatabase     db;
    StorageTable*       table;
    StorageDataCache*   cache;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;
    long                elapsed;
    unsigned            num, len, ksize, vsize;
    char*               area;
    char*               p;

    // Initialization ==============================================================================
    num = 1000*1000;
    ksize = 20;
    vsize = 128;
    area = (char*) malloc(num*(ksize+vsize));

    DCACHE->Init((ksize + vsize) * 2 * num);
    cache = DCACHE;
    TEST_ASSERT(cache != NULL);

    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(__func__);

    // SET key-values ==============================================================================
    sw.Reset();
    for (unsigned i = 0; i < num; i++)
    {
        p = area + i*(ksize+vsize);
        len = snprintf(p, ksize, "%011d", i); // takes 100 ms
        rk.SetBuffer(p);
        rk.SetLength(len);
        p += ksize;
        len = snprintf(p, vsize, "%.100f", (float) i); // takes 100 ms
        rv.SetBuffer(p);
        rv.SetLength(len);
        sw.Start();
        table->Set(rk, rv, false);
        sw.Stop();
    }
    TEST_LOG("%u sets took %ld msec", num, (long) sw.Elapsed());

    // Commit changes ==============================================================================
    sw.Restart();
    table->Commit(true /*recovery*/, false /*flush*/);
    elapsed = sw.Stop();
    TEST_LOG("Commit() took %ld msec", elapsed);

    // Shutdown ====================================================================================
    db.Close();
    DCACHE->Shutdown();
    free(area);

    return TEST_SUCCESS;
}

/*
====================================================================================================

 TestStorageBigRandomTransaction

====================================================================================================
*/
TEST_DEFINE(TestStorageBigRandomTransaction)
{
    StorageDatabase     db;
    StorageTable*       table;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;
    long                elapsed;
    unsigned            num, ksize, vsize;
    char*               area;
    char*               p;

    // Initialization ==============================================================================
    num = 1000*1000;
    ksize = 20;
    vsize = 64;
    area = (char*) malloc(num*(ksize+vsize));

    DCACHE->Init((ksize + vsize) * 2 * num);
    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(__func__);

    // SET key-values ==============================================================================
    sw.Reset();
    for (unsigned i = 0; i < num; i++)
    {
        p = area + i*(ksize+vsize);
        RandomBuffer(p, ksize);
        rk.SetBuffer(p);
        rk.SetLength(ksize);
        p += ksize;
        RandomBuffer(p, vsize);
        rv.SetBuffer(p);
        rv.SetLength(vsize);
        sw.Start();
        table->Set(rk, rv, false);
        sw.Stop();
    }
    TEST_LOG("%u sets took %ld msec", num, (long) sw.Elapsed());

    // Commit changes ==============================================================================
    sw.Restart();
    table->Commit(true /*recovery*/, false /*flush*/);
    elapsed = sw.Stop();
    TEST_LOG("Commit() took %ld msec", elapsed);

    // Shutdown ====================================================================================
    db.Close();
    DCACHE->Shutdown();
    free(area);

    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageShardSize)
{
    StorageDatabase     db;
    StorageTable*       table;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;
    long                elapsed;
    unsigned            num, len, ksize, vsize;
    char*               area;
    char*               p;
    unsigned            round;

    // Initialization ==============================================================================
    round = 10;
    num = 1000*1000;
    ksize = 20;
    vsize = 64;
    area = (char*) malloc(num*(ksize+vsize));

    DCACHE->Init((ksize + vsize) * 4 * num);

    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(__func__);

    // SET key-values ==============================================================================
    // a million key-value pairs take up 248M disk space
    for (unsigned r = 0; r < round; r++)
    {
        sw.Reset();
        for (unsigned i = 0; i < num; i++)
        {
            p = area + i*(ksize+vsize);
            len = snprintf(p, ksize, "%011d", i + r * num); // takes 100 ms
            rk.SetBuffer(p);
            rk.SetLength(len);
            //printf("%s\n", p);
            p += ksize;
            len = snprintf(p, vsize, "%.100f", (float) i + r * num); // takes 100 ms
            rv.SetBuffer(p);
            rv.SetLength(len);
            sw.Start();
            table->Set(rk, rv, false);
            sw.Stop();
        }
        
        printf("%u sets took %ld msec\n", num, (long) sw.Elapsed());

        sw.Reset();
        sw.Start();
        table->Commit(true /*recovery*/, false /*flush*/);
        elapsed = sw.Stop();
        printf("Commit() took %ld msec\n", elapsed);
        printf("Shard size: %s\n", SI_BYTES(table->GetSize()));
    }

    // Shutdown ====================================================================================
    db.Close();
    DCACHE->Shutdown();
    free(area);

    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageShardSplit)
{
    StorageDatabase     db;
    StorageTable*       table;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;
    long                elapsed;
    unsigned            num, len, ksize, vsize;
    char*               area;
    char*               p;
    unsigned            round;
    char                splitKey[] = "00000033620";

    // Initialization ==============================================================================
    round = 10;
    num = 100*1000;
    ksize = 20;
    vsize = 128;
    area = (char*) malloc(num*(ksize+vsize));

    DCACHE->Init((ksize + vsize) * 4 * num);

    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(__func__);

    // Write to database in rounds =================================================================
    // a million key-value pairs take up 248M disk space
    for (unsigned r = 0; r < round; r++)
    {

        sw.Reset();
        for (unsigned i = 0; i < num; i++)
        {
            p = area + i*(ksize+vsize);
            len = snprintf(p, ksize, "%011d", i + r * num); // takes 100 ms
            rk.SetBuffer(p);
            rk.SetLength(len);
            //printf("%s\n", p);
            p += ksize;
            len = snprintf(p, vsize, "%.100f", (float) i + r * num); // takes 100 ms
            rv.SetBuffer(p);
            rv.SetLength(len);
            sw.Start();
            table->Set(rk, rv, false);
            sw.Stop();
        }
        
        printf("%u sets took %ld msec\n", num, (long) sw.Elapsed());

        sw.Reset();
        sw.Start();
        table->Commit(true /*recovery*/, false /*flush*/);
        elapsed = sw.Stop();
        printf("Commit() took %ld msec\n", elapsed);
        printf("Shard size: %s\n", SI_BYTES(table->GetSize()));
    }
    
    // Split on a predefined key ===================================================================    
    rk.Wrap(splitKey, sizeof(splitKey) - 1);
    
    // TODO: calculate the new shardID instead of hardcoded value
    if (!table->GetShard(1))
        table->SplitShard(0, 1, rk);
    
    // Shutdown ====================================================================================
    db.Close();
    DCACHE->Shutdown();    
    free(area);

    return TEST_SUCCESS;
}

TEST_DEFINE(TestStoragePageSplit)
{
    StorageEnvironment  env;
    StorageDatabase*    db;
    StorageTable*       table;
    char                keyvalue[DATAPAGE_MAX_KV_SIZE(DEFAULT_DATAPAGE_SIZE) / 2 + 1];
    ReadBuffer          key;
    ReadBuffer          value;

    // Initialization ==============================================================================
    env.InitCache(STORAGE_DEFAULT_CACHE_SIZE);
    env.Open(TEST_DATABASE_PATH);
    db = env.GetDatabase(TEST_DATABASE);
    table = db->GetTable(__func__);

    // key is empty
    if (!table->ShardExists(key))
        table->CreateShard(0, key);

    // init keyvalues
    for (unsigned i = 0; i < sizeof(keyvalue); i++)
        keyvalue[i] = i % 10 + '0';
    
    key.SetBuffer(keyvalue);
    key.SetLength(DEFAULT_KEY_LIMIT);
    
    value.SetBuffer(keyvalue + DEFAULT_KEY_LIMIT);
    value.SetLength(sizeof(keyvalue) - DEFAULT_KEY_LIMIT);

    // first delete any existing values
    for (unsigned char i = 1; i <= 3; i++)
    {
        memset(keyvalue, i % 10 + '0', DEFAULT_KEY_LIMIT);
        table->Delete(key);
    }
    table->Commit();
    
    // create three big keyvalue pairs that will definitely split a page
    for (int i = 1; i <= 3; i++)
    {
        memset(keyvalue, i % 10 + '0', DEFAULT_KEY_LIMIT);
        table->Set(key, value, true);
    }
    table->Commit();
    
    // Shutdown ====================================================================================
    env.Close();
    
    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageFileSplit)
{
    StorageEnvironment  env;
    StorageDatabase*    db;
    StorageTable*       table;
    char                keyvalue[100];
    ReadBuffer          key;
    ReadBuffer          value;
    int                 num;

//    TEST_ASSERT(TestStorageCursorWalk(__func__) == TEST_SUCCESS);

    // Initialization ==============================================================================
    env.InitCache(STORAGE_DEFAULT_CACHE_SIZE);
    env.Open(TEST_DATABASE_PATH);
    db = env.GetDatabase(TEST_DATABASE);
    table = db->GetTable(__func__);

    // key is empty
    if (!table->ShardExists(key))
        table->CreateShard(0, key);
    
    key.SetBuffer(keyvalue);
    key.SetLength(10);
    
    value.SetBuffer(keyvalue + key.GetLength());
    value.SetLength(sizeof(keyvalue) - key.GetLength());
    memset(value.GetBuffer(), 0, value.GetLength());
    
    num = 75*1000;
    for (int i = 1; i < num; i++)
    {
        snprintf(key.GetBuffer(), key.GetLength(), "%09d", i);
        table->Set(key, value, true);
    }
    table->Commit();
    
    for (int i = num; i < num + 5000; i++)
    {
        snprintf(key.GetBuffer(), key.GetLength(), "%09d", i);
        table->Set(key, value, true);
    }
    table->Commit();
    
    // Shutdown ====================================================================================
    env.Close();
    
//    TEST_ASSERT(TestStorageCursorWalk(__func__) == TEST_SUCCESS);

    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageFileThreeWaySplit)
{
    StorageEnvironment  env;
    StorageDatabase*    db;
    StorageTable*       table;
    char                keyvalue[DATAPAGE_MAX_KV_SIZE(DEFAULT_DATAPAGE_SIZE)];
    ReadBuffer          key;
    ReadBuffer          value;
    unsigned            i;

    // Initialization ==============================================================================
    env.InitCache(STORAGE_DEFAULT_CACHE_SIZE);
    env.Open(TEST_DATABASE_PATH);
    db = env.GetDatabase(TEST_DATABASE);
    table = db->GetTable(__func__);

    // key is empty
    if (!table->ShardExists(key))
        table->CreateShard(0, key);

    // init keyvalues
    for (i = 0; i < sizeof(keyvalue); i++)
        keyvalue[i] = i % 10 + '0';
    
    key.SetBuffer(keyvalue);
    key.SetLength(DEFAULT_KEY_LIMIT);
    
    value.SetBuffer(keyvalue + DEFAULT_KEY_LIMIT);

    // first delete any existing values
    for (unsigned char c = 1; i <= 3; i++)
    {
        memset(keyvalue, c % 10 + '0', DEFAULT_KEY_LIMIT);
        table->Delete(key);
    }
    table->Commit();

    i = 1;
    memset(keyvalue, i % 10 + '0', DEFAULT_KEY_LIMIT);
    value.SetLength((sizeof(keyvalue) / 2) - DEFAULT_KEY_LIMIT - (DATAPAGE_KV_OVERHEAD / 2));
    table->Set(key, value, true);

    i = 3;
    memset(keyvalue, i % 10 + '0', DEFAULT_KEY_LIMIT);
    value.SetLength((sizeof(keyvalue) / 2) - DEFAULT_KEY_LIMIT - (DATAPAGE_KV_OVERHEAD / 2));
    table->Set(key, value, true);

    // insert between '1' and '3' a new value which has PAGESIZE size
    i = 2;
    memset(keyvalue, i % 10 + '0', DEFAULT_KEY_LIMIT);
    value.SetLength(sizeof(keyvalue) - DEFAULT_KEY_LIMIT);
    table->Set(key, value, true);

    table->Commit();

    // Shutdown ====================================================================================
    env.Close();
    
    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageBinaryData)
{
    StorageEnvironment  env;
    StorageDatabase*    db;
    StorageTable*       table;
    char                keyvalue[DATAPAGE_MAX_KV_SIZE(DEFAULT_DATAPAGE_SIZE)];
    ReadBuffer          key;
    ReadBuffer          value;
    unsigned            i;
    unsigned            num;

    // Initialization ==============================================================================
    num = 1000;
    env.InitCache(sizeof(keyvalue) * 2 * num);
    env.Open(TEST_DATABASE_PATH);
    db = env.GetDatabase(TEST_DATABASE);
    table = db->GetTable(__func__);

    // key is empty
    if (!table->ShardExists(key))
        table->CreateShard(0, key);

    // init keyvalues
    for (i = 0; i < sizeof(keyvalue); i++)
        keyvalue[i] = i % 256;
    
    key.SetBuffer(keyvalue);
    key.SetLength(DEFAULT_KEY_LIMIT);
    value.SetBuffer(keyvalue + DEFAULT_KEY_LIMIT);
    value.SetLength((sizeof(keyvalue) / 2) - DEFAULT_KEY_LIMIT - (DATAPAGE_KV_OVERHEAD / 2));
    
    for (i = 0; i < num; i++)
    {
        snprintf(keyvalue, sizeof(keyvalue), "%u", i);
        table->Set(key, value, true);        
    }
    
    table->Commit();

    // Shutdown ====================================================================================
    env.Close();
    
    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageWindowsSync)
{
    StorageDatabase     db;
    StorageTable*       table;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;
    unsigned            num, ksize, vsize;
    char*               area;
    unsigned            round;
    bool                found;

    // Initialization ==============================================================================
    round = 1000 * 1000;
    num = 1000;
    ksize = 10;
    vsize = 10;
    area = (char*) malloc(num*(ksize+vsize));

    DCACHE->Init((ksize + vsize) * 2 * 1000 * num);
    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(__func__);

    // Get key-values ==========================================================================

    rk.SetBuffer((char*) "egyikkulcs");
    rk.SetLength(10);
    found = table->Get(rk, rv);

    rk.SetBuffer((char*) "masikkulcs");
    rk.SetLength(10);
    found = table->Get(rk, rv);

    // Set key-values ==========================================================================

    rk.SetBuffer((char*) "egyikkulcs");
    rk.SetLength(10);
    rv.SetBuffer((char*) "egyikertek");
    rv.SetLength(10);
    table->Set(rk, rv, true);

    // Commit changes ==========================================================================
    table->Commit(true /*recovery*/, true /*flush*/);
     
    // Set key-values ==========================================================================

    rk.SetBuffer((char*) "masikkulcs");
    rk.SetLength(10);
    rv.SetBuffer((char*) "masikertek");
    rv.SetLength(10);
    table->Set(rk, rv, true);

    // Commit changes ==========================================================================
    table->Commit(true /*recovery*/, true /*flush*/);

    // Shutdown ====================================================================================
    db.Close();
    DCACHE->Shutdown();
    free(area);

    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageRandomGetSetDelete)
{
    StorageDatabase     db;
    StorageTable*       table;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;
    long                elapsed;
    unsigned            num, len, ksize, vsize;
    char*               area;
    char*               p;
    unsigned            round;

    // Initialization ==============================================================================
    round = 1000 * 1000;
    num = 10000;
    ksize = 10;
    vsize = 10;
    area = (char*) malloc(num*(ksize+vsize));

    DCACHE->Init((ksize + vsize) * 2 * 10 * 1000 * num);
    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(__func__);

    //==============================================================================================
    //
    // test the number of SETs depending on the size of DCACHE and transaction size
    // e.g. a million key-value pairs take up 248M disk space
    //
    //==============================================================================================
    for (unsigned r = 0; r < round; r++)
    {
        // Set key-values ==========================================================================
        sw.Reset();
        for (unsigned i = 0; i < num; i++)
        {
            p = area + i*(ksize+vsize);
            len = snprintf(p, ksize + 1, "%010d", RandomInt(0, 10 * 1000 * num));
            rk.SetBuffer(p);
            rk.SetLength(len);
            //printf("%s\n", p);
            p += ksize;
            len = snprintf(p, vsize + 1, "%010d", i); // takes 100 ms
            rv.SetBuffer(p);
            rv.SetLength(vsize);
            sw.Start();
            switch (RandomInt(0, 1))
            {
            case 0:
            case 1:
                //TEST_LOG("Set, %.*s", rk.GetLength(), rk.GetBuffer());
                table->Set(rk, rv, true);
                break;
            case 2:
                TEST_LOG("Delete, %.*s", rk.GetLength(), rk.GetBuffer());
                table->Delete(rk);
                break;
            case 3:
                TEST_LOG("Get, %.*s", rk.GetLength(), rk.GetBuffer());
                table->Get(rk, rv);
                break;
            default:
                ASSERT_FAIL();
            }
            sw.Stop();
        }
        TEST_LOG("Round %u: %u sets took %ld msec", r, num, (long) sw.Elapsed());

        // Commit changes ==========================================================================
        sw.Reset();
        sw.Start();
        table->Commit(true /*recovery*/, true /*flush*/);
        elapsed = sw.Stop();
        TEST_LOG("Round %u: Commit() took %ld msec", r, elapsed);
    }

    // Shutdown ====================================================================================
    db.Close();
    DCACHE->Shutdown();
    free(area);

    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageAppend)
{
    StorageDatabase     db;
    StorageTable*       table;
    StorageDataCache*   cache;
    Buffer              k, v;
    ReadBuffer          rk, rv;
    Stopwatch           sw;
    long                elapsed;
    unsigned            len;
    char*               area;
    char*               p;
    uint64_t            clock;
    
    // Initialization ==============================================================================
    StartClock();

    const unsigned num = 100*1000;
    const unsigned ksize = 20;
    const unsigned vsize = 128;
    area = (char*) malloc(num*(ksize+vsize));

    cache = DCACHE;
    DCACHE->Init(100000000);

    db.Open(TEST_DATABASE_PATH, TEST_DATABASE);
    table = db.GetTable(__func__);

    //==============================================================================================
    //
    // TestStorage SET test
    //
    // This test might not work on fast machines, because it commits every 1000 msec, but if there
    // are more sets between commits than the cache can contain, then it will assert!
    //
    //==============================================================================================
    clock = NowClock();
    sw.Start();
    for (unsigned i = 0; i < num; i++)
    {
        p = area + i*(ksize+vsize);
        len = snprintf(p, ksize, "%d", i);
        rk.SetBuffer(p);
        rk.SetLength(len);
        p += ksize;
        len = snprintf(p, vsize, "%.100f", (float) i);
        rv.SetBuffer(p);
        rv.SetLength(len);
        table->Set(rk, rv, false);

        if (NowClock() - clock >= 1000)
        {
            TEST_LOG("syncing...");
            db.Commit();
            clock = NowClock();
        }
    }
    db.Commit();
    elapsed = sw.Stop();
    TEST_LOG("%u sets took %ld msec", num, elapsed);

    // APPEND "a" to all values ================================================================================
    rv.Wrap("a");
    sw.Reset();
    sw.Start();
    for (unsigned i = 0; i < num; i++)
    {
        k.Writef("%d", i);
        if (table->Append(k, rv))
            ;//PRINTKV()
        else
            ASSERT_FAIL();
    }   
    db.Commit();
    elapsed = sw.Stop();
    TEST_LOG("%u appends took %ld msec", num, elapsed);

    // GET all keys ================================================================================
    sw.Reset();
    sw.Start();
    for (unsigned i = 0; i < num; i++)
    {
        k.Writef("%d", i);
        if (table->Get(k, rv))
            ;//PRINTKV()
        else
            ASSERT_FAIL();
    }   
    elapsed = sw.Stop();
    TEST_LOG("%u gets took %ld msec", num, elapsed);

    // DELETE all keys =============================================================================
    sw.Reset();
    sw.Start();
    for (unsigned i = 0; i < num; i++)
    {
        k.Writef("%d", i);
        TEST_ASSERT(table->Delete(k));
    }
    db.Commit();
    elapsed = sw.Stop();
    TEST_LOG("%u deletes took %ld msec", num, elapsed);
    
    // Shutdown ====================================================================================
    sw.Reset();
    sw.Start();
    db.Close();
    elapsed = sw.Stop();
    TEST_LOG("Close() took %ld msec", elapsed);
    
    DCACHE->Shutdown();
    free(area);
    
    return TEST_SUCCESS;
}

TEST_DEFINE(TestStorageCursor)
{
    return TestStorageCursorWalk("TestStorageRandomGetSetDelete");
}
