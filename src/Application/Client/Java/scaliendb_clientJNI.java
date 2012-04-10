/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.31
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.scalien.scaliendb;

class scaliendb_clientJNI {
  public final static native void imaxdiv_t_quot_set(long jarg1, imaxdiv_t jarg1_, long jarg2);
  public final static native long imaxdiv_t_quot_get(long jarg1, imaxdiv_t jarg1_);
  public final static native void imaxdiv_t_rem_set(long jarg1, imaxdiv_t jarg1_, long jarg2);
  public final static native long imaxdiv_t_rem_get(long jarg1, imaxdiv_t jarg1_);
  public final static native long new_imaxdiv_t();
  public final static native void delete_imaxdiv_t(long jarg1);
  public final static native long imaxabs(long jarg1);
  public final static native long imaxdiv(long jarg1, long jarg2);
  public final static native long strtoimax(String jarg1, long jarg2, int jarg3);
  public final static native java.math.BigInteger strtoumax(String jarg1, long jarg2, int jarg3);
  public final static native long new_SDBP_NodeParams(int jarg1);
  public final static native void delete_SDBP_NodeParams(long jarg1);
  public final static native void SDBP_NodeParams_Close(long jarg1, SDBP_NodeParams jarg1_);
  public final static native void SDBP_NodeParams_AddNode(long jarg1, SDBP_NodeParams jarg1_, String jarg2);
  public final static native void SDBP_NodeParams_nodec_set(long jarg1, SDBP_NodeParams jarg1_, int jarg2);
  public final static native int SDBP_NodeParams_nodec_get(long jarg1, SDBP_NodeParams jarg1_);
  public final static native void SDBP_NodeParams_nodes_set(long jarg1, SDBP_NodeParams jarg1_, long jarg2);
  public final static native long SDBP_NodeParams_nodes_get(long jarg1, SDBP_NodeParams jarg1_);
  public final static native void SDBP_NodeParams_num_set(long jarg1, SDBP_NodeParams jarg1_, int jarg2);
  public final static native int SDBP_NodeParams_num_get(long jarg1, SDBP_NodeParams jarg1_);
  public final static native long new_SDBP_Buffer();
  public final static native void SDBP_Buffer_SetBuffer(long jarg1, SDBP_Buffer jarg1_, byte[] jarg2, int jarg3);
  public final static native void SDBP_Buffer_data_set(long jarg1, SDBP_Buffer jarg1_, int jarg2);
  public final static native int SDBP_Buffer_data_get(long jarg1, SDBP_Buffer jarg1_);
  public final static native void SDBP_Buffer_len_set(long jarg1, SDBP_Buffer jarg1_, int jarg2);
  public final static native int SDBP_Buffer_len_get(long jarg1, SDBP_Buffer jarg1_);
  public final static native void delete_SDBP_Buffer(long jarg1);
  public final static native void SDBP_ResultClose(long jarg1);
  public final static native String SDBP_ResultKey(long jarg1);
  public final static native String SDBP_ResultValue(long jarg1);
  public final static native byte[] SDBP_ResultKeyBuffer(long jarg1);
  public final static native byte[] SDBP_ResultValueBuffer(long jarg1);
  public final static native long SDBP_ResultSignedNumber(long jarg1);
  public final static native long SDBP_ResultNumber(long jarg1);
  public final static native boolean SDBP_ResultIsConditionalSuccess(long jarg1);
  public final static native long SDBP_ResultDatabaseID(long jarg1);
  public final static native long SDBP_ResultTableID(long jarg1);
  public final static native void SDBP_ResultBegin(long jarg1);
  public final static native void SDBP_ResultNext(long jarg1);
  public final static native boolean SDBP_ResultIsEnd(long jarg1);
  public final static native boolean SDBP_ResultIsFinished(long jarg1);
  public final static native int SDBP_ResultTransportStatus(long jarg1);
  public final static native int SDBP_ResultCommandStatus(long jarg1);
  public final static native long SDBP_ResultNumNodes(long jarg1);
  public final static native long SDBP_ResultNodeID(long jarg1, long jarg2);
  public final static native long SDBP_ResultElapsedTime(long jarg1);
  public final static native long SDBP_Create();
  public final static native int SDBP_Init(long jarg1, long jarg2, SDBP_NodeParams jarg2_);
  public final static native void SDBP_Destroy(long jarg1);
  public final static native long SDBP_GetResult(long jarg1);
  public final static native void SDBP_SetGlobalTimeout(long jarg1, long jarg2);
  public final static native void SDBP_SetMasterTimeout(long jarg1, long jarg2);
  public final static native long SDBP_GetGlobalTimeout(long jarg1);
  public final static native long SDBP_GetMasterTimeout(long jarg1);
  public final static native String SDBP_GetJSONConfigState(long jarg1);
  public final static native void SDBP_WaitConfigState(long jarg1);
  public final static native void SDBP_SetConsistencyMode(long jarg1, int jarg2);
  public final static native void SDBP_SetBatchMode(long jarg1, int jarg2);
  public final static native void SDBP_SetBatchLimit(long jarg1, long jarg2);
  public final static native int SDBP_CreateDatabase(long jarg1, String jarg2);
  public final static native int SDBP_RenameDatabase(long jarg1, long jarg2, String jarg3);
  public final static native int SDBP_DeleteDatabase(long jarg1, long jarg2);
  public final static native int SDBP_CreateTable(long jarg1, long jarg2, long jarg3, String jarg4);
  public final static native int SDBP_RenameTable(long jarg1, long jarg2, String jarg3);
  public final static native int SDBP_DeleteTable(long jarg1, long jarg2);
  public final static native int SDBP_TruncateTable(long jarg1, long jarg2);
  public final static native long SDBP_GetNumQuorums(long jarg1);
  public final static native long SDBP_GetQuorumIDAt(long jarg1, long jarg2);
  public final static native String SDBP_GetQuorumNameAt(long jarg1, long jarg2);
  public final static native long SDBP_GetQuorumIDByName(long jarg1, String jarg2);
  public final static native long SDBP_GetNumDatabases(long jarg1);
  public final static native long SDBP_GetDatabaseIDAt(long jarg1, long jarg2);
  public final static native String SDBP_GetDatabaseNameAt(long jarg1, long jarg2);
  public final static native long SDBP_GetDatabaseIDByName(long jarg1, String jarg2);
  public final static native long SDBP_GetNumTables(long jarg1, long jarg2);
  public final static native long SDBP_GetTableIDAt(long jarg1, long jarg2, long jarg3);
  public final static native String SDBP_GetTableNameAt(long jarg1, long jarg2, long jarg3);
  public final static native long SDBP_GetTableIDByName(long jarg1, long jarg2, String jarg3);
  public final static native int SDBP_Get(long jarg1, long jarg2, String jarg3);
  public final static native int SDBP_GetCStr(long jarg1, long jarg2, byte[] jarg3, int jarg4);
  public final static native int SDBP_Set(long jarg1, long jarg2, String jarg3, String jarg4);
  public final static native int SDBP_SetCStr(long jarg1, long jarg2, byte[] jarg3, int jarg4, byte[] jarg5, int jarg6);
  public final static native int SDBP_Add(long jarg1, long jarg2, String jarg3, long jarg4);
  public final static native int SDBP_AddCStr(long jarg1, long jarg2, byte[] jarg3, int jarg4, long jarg5);
  public final static native int SDBP_Delete(long jarg1, long jarg2, String jarg3);
  public final static native int SDBP_DeleteCStr(long jarg1, long jarg2, byte[] jarg3, int jarg4);
  public final static native int SDBP_SequenceSet(long jarg1, long jarg2, String jarg3, long jarg4);
  public final static native int SDBP_SequenceSetCStr(long jarg1, long jarg2, byte[] jarg3, int jarg4, long jarg5);
  public final static native int SDBP_SequenceNext(long jarg1, long jarg2, String jarg3);
  public final static native int SDBP_SequenceNextCStr(long jarg1, long jarg2, byte[] jarg3, int jarg4);
  public final static native int SDBP_ListKeys(long jarg1, long jarg2, String jarg3, String jarg4, String jarg5, long jarg6, boolean jarg7, boolean jarg8);
  public final static native int SDBP_ListKeysCStr(long jarg1, long jarg2, byte[] jarg3, int jarg4, byte[] jarg5, int jarg6, byte[] jarg7, int jarg8, long jarg9, boolean jarg10, boolean jarg11);
  public final static native int SDBP_ListKeyValues(long jarg1, long jarg2, String jarg3, String jarg4, String jarg5, long jarg6, boolean jarg7, boolean jarg8);
  public final static native int SDBP_ListKeyValuesCStr(long jarg1, long jarg2, byte[] jarg3, int jarg4, byte[] jarg5, int jarg6, byte[] jarg7, int jarg8, long jarg9, boolean jarg10, boolean jarg11);
  public final static native int SDBP_Count(long jarg1, long jarg2, String jarg3, String jarg4, String jarg5, boolean jarg6);
  public final static native int SDBP_CountCStr(long jarg1, long jarg2, byte[] jarg3, int jarg4, byte[] jarg5, int jarg6, byte[] jarg7, int jarg8, boolean jarg9);
  public final static native int SDBP_Begin(long jarg1);
  public final static native int SDBP_Submit(long jarg1);
  public final static native int SDBP_Cancel(long jarg1);
  public final static native int SDBP_StartTransaction(long jarg1, long jarg2, String jarg3);
  public final static native int SDBP_StartTransactionCStr(long jarg1, long jarg2, byte[] jarg3, int jarg4);
  public final static native int SDBP_CommitTransaction(long jarg1);
  public final static native int SDBP_RollbackTransaction(long jarg1);
  public final static native void SDBP_SetTrace(boolean jarg1);
  public final static native void SDBP_SetDebug(boolean jarg1);
  public final static native void SDBP_SetLogFile(String jarg1);
  public final static native void SDBP_SetTraceBufferSize(long jarg1);
  public final static native void SDBP_LogTrace(String jarg1);
  public final static native void SDBP_SetCrashReporter(boolean jarg1);
  public final static native void SDBP_SetShardPoolSize(long jarg1);
  public final static native void SDBP_SetMaxConnections(long jarg1);
  public final static native String SDBP_GetVersion();
  public final static native String SDBP_GetDebugString();
}
