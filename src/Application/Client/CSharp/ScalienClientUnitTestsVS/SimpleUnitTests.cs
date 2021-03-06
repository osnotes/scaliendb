﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;

#if !SCALIEN_UNIT_TEST_FRAMEWORK
using Microsoft.VisualStudio.TestTools.UnitTesting;
#endif

using Scalien;

namespace ScalienClientUnitTesting
{
    [TestClass]
    public class SimpleUnitTests
    {
        //[TestMethod]
        public void SetGetMP3()
        {
            string dbName = "test_mp3_db";
            string tableName = "test_mp3_table";

            Client client = new Client(Utils.GetConfigNodes());
            Table tbl = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableName);

            // SET MP3 (you will need a valdi path to test.mp3
            byte[] payload = Utils.ReadFile("f:/test.mp3");
            System.Console.WriteLine("mp3 buffer: {0}", payload.GetLength(0));
            tbl.Set(System.Text.Encoding.UTF8.GetBytes("mp3"), payload);

            client.Submit();

            byte[] res = tbl.Get(System.Text.Encoding.UTF8.GetBytes("mp3"));
            System.Console.WriteLine("mp3 buffer: {0}", res.GetLength(0));

            Assert.IsTrue(Utils.ByteArraysEqual(payload, res));
        }

        [TestMethod]
        public void GetSetSubmit()
        {
            string dbName = "get_set_db";
            string tableName = "get_set_db_table";

            Client client = new Client(Utils.GetConfigNodes());
            Table tbl = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableName);

            //client.Submit();

            tbl.Get("0000000000001");
            tbl.Set("0000000000001", "test");
            tbl.Get("0000000000002");
            tbl.Set("0000000000002", "test");

            client.Submit();

            var i = tbl.Count(new ByteRangeParams());
            Assert.IsTrue(i == 2);
        }

        [TestMethod]
        public void CreateAndCloseClients()
        {
            string dbName = "create_and_close_clients_db";
            string tableName = "create_and_close_clients_table";

            Client client1 = new Client(Utils.GetConfigNodes());
            Client client2 = new Client(Utils.GetConfigNodes());

            Table tbl1 = Utils.GetOrCreateEmptyTableAndDatabase(client1, dbName, tableName);
            Table tbl2 = Utils.GetOrCreateEmptyTableAndDatabase(client2, dbName + "_2", tableName);

            tbl1.TruncateTable();

            //client.Submit();

            tbl1.Get("0000000000001");
            tbl1.Set("0000000000001", "test");
            tbl1.Get("0000000000002");
            tbl1.Set("0000000000002", "test");

            client1.Submit();

            tbl1.Get("0000000000001");
            tbl1.Set("0000000000001", "test");
            tbl1.Get("0000000000002");
            tbl1.Set("0000000000002", "test");

            client1.Submit();

            tbl2.Get("0000000000001");
            tbl2.Set("0000000000001", "test");
            tbl2.Get("0000000000002");
            tbl2.Set("0000000000002", "test");

            //client1.Close();
            //client2.Close();
        }

        [TestMethod]
        public void TruncateAfterSet()
        {
            string dbName = "get_set_db";
            string tableName = "get_set_db_table";
            Client.SetLogFile("f:\\log.txt");
            Client.SetTrace(true);
            Client client = new Client(Utils.GetConfigNodes());
            Table tbl = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableName);

            try
            {

                tbl.Set("0000000000001", "test");
                tbl.Set("0000000000002", "test");

                tbl.TruncateTable();

                client.Submit();

                Assert.Fail("No SDBPException!");
            }
            catch (SDBPException)
            {
            }

            //client.Close();
        }

        [TestMethod]
        public void SequenceResetAndTruncate()
        {
            string dbName = "seq_and_trunc_db";
            string tableNameSeq = "seq_table";
            string tableNameTrunc = "trunc_table";

            Client client = new Client(Utils.GetConfigNodes());
            Table tblSeq = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableNameSeq);
            Table tblTrunc = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableNameTrunc);

            Sequence testSeq = tblSeq.GetSequence("seqIDs");

            try
            {
                testSeq.Reset();

                tblTrunc.TruncateTable();

                client.Submit();

                Assert.Fail("No SDBPException!");
            }
            catch (SDBPException)
            {
            }

            //client.Close();
        }

        [TestMethod]
        public void SequenceAddAfterSet()
        {
            string dbName = "seq_and_set_db";
            string tableNameSeq = "seq_table";
            string tableNameSet = "set_table";

            Client client = new Client(Utils.GetConfigNodes());
            Table tblSeq = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableNameSeq);
            Table tblSet = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableNameSet);

            Sequence testSeq = tblSeq.GetSequence("seqIDs");

            tblSet.Set("0000000000001", "test");
            tblSet.Set("0000000000002", "test");
            tblSet.Set("0000000000003", "test");

            Console.WriteLine("Next: " + testSeq.GetNext);

            client.Submit();

            client.Close();
        }

        [TestMethod]
        public void CountBeforeSubmit()
        {
            string dbName = "test_db";
            string tableName = "test_table";

            Client client = new Client(Utils.GetConfigNodes());
            Table tbl = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableName);

            for(ulong i = 0; i < 1000; i++)
                tbl.Set(Utils.Id(i), "test");

            var cnt1 = tbl.Count(new ByteRangeParams());

            client.Submit();

            var cnt2 = tbl.Count(new StringRangeParams());

            Console.WriteLine(cnt1 + " - " + cnt2);

            Assert.IsTrue(cnt2 == 1000);

            client.Close();
        }

        [TestMethod]
        public void ClientInactivityTest()
        {
            Client.SetTrace(true);
            Client.SetLogFile("c:\\Users\\zszabo\\logs\\no_service_trace.txt");

            string dbName = "test_db";
            string tableName = "test_table";

            Client client = new Client(Utils.GetConfigNodes());
            Table tbl = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableName);

            for (ulong i = 0; i < 1000; i++)
                tbl.Set(Utils.Id(i), "test");

            client.Submit();

            client.SetGlobalTimeout(30000);
            var timeout = 5 * client.GetGlobalTimeout();
            Console.WriteLine("Now sleeping for " + timeout / 1000 + " seconds");
            Thread.Sleep((int)timeout);

            foreach (string key in tbl.GetKeyIterator(new StringRangeParams()))
                Console.Write(key);
            
            client.Close();
        }

        [TestMethod]
        public void TestListTable()
        {
            var dbName = "Benchmark";
            var tableName = "fileContentLockStartDateIndex";
            Client client = new Client(Utils.GetConfigNodes());

            Database db = client.GetDatabase(dbName);
            Table tbl = db.GetTable(tableName);

            var counter = 0;
            foreach (var kv in tbl.GetKeyValueIterator(new ByteRangeParams()))
            {
                counter++;
            }

            client.Close();
        }

        [TestMethod]
        public void ListTestsWithoutProxies()
        {
            var dbName = "test_db";
            var tableName = "test_table";
            int length = 10 * 1000;
            uint num = 22 * 1000;

            Client client = new Client(Utils.GetConfigNodes());
            Table tbl = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableName);

            var value = Utils.RandomString(length);

            for (ulong i = 0; i < num; i++)
                tbl.Set(Utils.Id(i), value);

            // Submit, no proxied values left
            client.Submit();

            client.SetConsistencyMode(Client.CONSISTENCY_STRICT);
            ListTests(tbl, num);

            client.SetConsistencyMode(Client.CONSISTENCY_RYW);
            ListTests(tbl, num);
        }

        [TestMethod]
        public void ListTestsWithProxies()
        {
            var dbName = "test_db";
            var tableName = "test_table";
            int length = 10 * 1000;
            uint num = 22 * 1000;

            Client client = new Client(Utils.GetConfigNodes());
            Table tbl = Utils.GetOrCreateEmptyTableAndDatabase(client, dbName, tableName);

            var value = Utils.RandomString(length);

            for (ulong i = 0; i < num; i++)
                tbl.Set(Utils.Id(i), value);

            // don't Submit, proxied values
            //client.Submit();

            ListTests(tbl, num);
        }


        public void ListTests(Table tbl, uint num)
        {
            ListTest1(tbl, num);
            ListTest2(tbl, num);
            ListTest3(tbl, num);
            ListTest4(tbl, num);
            ListTest5(tbl, num);
            ListTest6(tbl, num);
            ListTest7(tbl, num);
            ListTest8(tbl, num);
            ListTest9(tbl, num);
            ListTest10(tbl, num);
            ListTest11(tbl, num);
            ListTest12(tbl, num);
            ListTest13(tbl, num);
            ListTest14(tbl, num);
            ListTest15(tbl, num);
            ListTest16(tbl, num);
            ListTest17(tbl, num);
            ListTest18(tbl, num);
            ListTest19(tbl, num);
            ListTest20(tbl, num);
            ListTest21(tbl, num);
            ListTest22(tbl, num);
            ListTest23(tbl, num);
            ListTest24(tbl, num);
            ListTest25(tbl, num);
            ListTest26(tbl, num);
            ListTest27(tbl, num);
            ListTest28(tbl, num);
            ListTest29(tbl, num);
        }

        public void PerformListTest(Table tbl, StringRangeParams ps, uint expected)
        {
            int cnt;
            
            cnt = 0;
            foreach (String key in tbl.GetKeyIterator(ps))
            {
                //Console.WriteLine(key);
                cnt++;
            }
            Console.WriteLine("Expecting {0}, received {1}", expected, cnt);
            Assert.IsTrue(expected == cnt);

            cnt = 0;
            foreach (KeyValuePair<string, string> kv in tbl.GetKeyValueIterator(ps))
            {
                cnt++;
            }
            Console.WriteLine("Expecting {0}, received {1}", expected, cnt);
            Assert.IsTrue(expected == cnt);
        }

        public void ListTest1(Table tbl, uint num)
        {
            uint count = 111;

            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(100));
            ps.Count(count);
            
            uint expected = count;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest2(Table tbl, uint num)
        {
            uint count = 111;

            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(100));
            ps.Count(count);
            ps.Backward();
            
            uint expected = 101;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest3(Table tbl, uint num)
        {
            uint count = 100;

            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(100));
            ps.Count(count);
            ps.Backward();
            
            uint expected = count;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest4(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            PerformListTest(tbl, ps, num);
        }

        public void ListTest5(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.Backward();
            PerformListTest(tbl, ps, num);
        }

        public void ListTest6(Table tbl, uint num)
        {
            uint count = 1234;

            StringRangeParams ps = new StringRangeParams();
            ps.Count(count);

            uint expected = count;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest7(Table tbl, uint num)
        {
            uint count = 1234;

            StringRangeParams ps = new StringRangeParams();
            ps.Backward();
            ps.Count(count);

            uint expected = count;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest8(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.EndKey(Utils.Id(15000));
            
            uint expected = 15000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest9(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.EndKey(Utils.Id(15000));
            ps.Backward();

            uint expected = num - 15000 - 1;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest10(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(10000));
            ps.EndKey(Utils.Id(15000));

            uint expected = 5000;
            PerformListTest(tbl, ps, expected);            
        }

        public void ListTest11(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(15000));
            ps.EndKey(Utils.Id(10000));
            ps.Backward();

            uint expected = 5000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest12(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(10000));
            ps.EndKey(Utils.Id(15000));
            ps.Prefix("0000000012");

            uint expected = 1000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest13(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(15000));
            ps.EndKey(Utils.Id(10000));
            ps.Prefix("00000000120");
            ps.Backward();

            uint expected = 100;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest14(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.Prefix("0000000012");

            uint expected = 1000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest15(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.Prefix("00000000120");
            ps.Backward();

            uint expected = 100;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest16(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.Prefix("000000002");

            uint expected = 2000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest17(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.Prefix("000000002");
            ps.Backward();

            uint expected = 2000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest18(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(13000));
            ps.Prefix("0000000012");

            uint expected = 0;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest19(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(12000));
            ps.Prefix("0000000013");
            ps.Backward();

            uint expected = 0;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest20(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(4000));
            ps.EndKey(Utils.Id(7000));

            uint expected = 3000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest21(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(7000));
            ps.EndKey(Utils.Id(4000));
            ps.Backward();

            uint expected = 3000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest22(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(10000));
            ps.EndKey(Utils.Id(13000));

            uint expected = 3000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest23(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(13000));
            ps.EndKey(Utils.Id(10000));
            ps.Backward();

            uint expected = 3000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest24(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(18000));
            ps.EndKey(Utils.Id(22000));

            uint expected = 4000;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest25(Table tbl, uint num)
        {
            StringRangeParams ps = new StringRangeParams();
            ps.StartKey(Utils.Id(22000));
            ps.EndKey(Utils.Id(18000));
            ps.Backward();
            
            uint expected = 3999;
            PerformListTest(tbl, ps, expected);
        }

        public void ListTest26(Table tbl, uint num)
        {
            for (uint i = 0; i < num; i += 1000)
            {
                StringRangeParams ps = new StringRangeParams();
                ps.StartKey(Utils.Id(i));
                ps.EndKey(Utils.Id(i + 1000));

                uint expected = 1000;
                PerformListTest(tbl, ps, expected);
            }
        }

        public void ListTest27(Table tbl, uint num)
        {
            for (uint i = num; i > 0; i -= 1000)
            {
                StringRangeParams ps = new StringRangeParams();
                ps.StartKey(Utils.Id(i));
                ps.EndKey(Utils.Id(i - 1000));
                ps.Backward();

                uint expected;
                if (i == 22000)
                    expected = 999;
                else
                    expected = 1000;
                PerformListTest(tbl, ps, expected);
            }
        }

        public void ListTest28(Table tbl, uint num)
        {
            for (uint i = 0; i < (num/1000); i++)
            {
                StringRangeParams ps = new StringRangeParams();
                ps.Prefix("00000000" + i.ToString("D2"));

                uint expected = 1000;
                PerformListTest(tbl, ps, expected);
            }
        }

        public void ListTest29(Table tbl, uint num)
        {
            for (uint i = 0; i < (num/1000); i++)
            {
                StringRangeParams ps = new StringRangeParams();
                ps.Prefix("00000000" + i.ToString("D2"));
                ps.Backward();

                uint expected = 1000;
                PerformListTest(tbl, ps, expected);
            }
        }

        [TestMethod]
        public void ListTestLinq()
        {
            Client client = new Client(Utils.GetConfigNodes());
            Table table = client.GetDatabase("Benchmark").GetTable("transactionNetworkTransaction");
            byte[] prefix = Utils.StringToByteArray("N:0000000000000|T:000000");
            byte[] startKey = Utils.StringToByteArray("N:0000000000000|T:0000001250847|");
            var rangeParams = new ByteRangeParams().StartKey(startKey).Prefix(prefix);
            ByteKeyValueIterator iterator = table.GetKeyValueIterator(rangeParams);
            
            var list = new List<KeyValuePair<byte[], byte[]>>();
            list.AddRange(iterator.Select(kv => kv));
        }
    }
}
