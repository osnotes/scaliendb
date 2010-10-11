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
  public final static native void SDBP_ResultClose(long jarg1);
  public final static native String SDBP_ResultKey(long jarg1);
  public final static native String SDBP_ResultValue(long jarg1);
  public final static native java.math.BigInteger SDBP_ResultDatabaseID(long jarg1);
  public final static native java.math.BigInteger SDBP_ResultTableID(long jarg1);
  public final static native void SDBP_ResultBegin(long jarg1);
  public final static native void SDBP_ResultNext(long jarg1);
  public final static native boolean SDBP_ResultIsEnd(long jarg1);
  public final static native int SDBP_ResultTransportStatus(long jarg1);
  public final static native int SDBP_ResultCommandStatus(long jarg1);
  public final static native long SDBP_Create();
  public final static native int SDBP_Init(long jarg1, long jarg2, SDBP_NodeParams jarg2_);
  public final static native void SDBP_Destroy(long jarg1);
  public final static native long SDBP_GetResult(long jarg1);
  public final static native void SDBP_SetGlobalTimeout(long jarg1, java.math.BigInteger jarg2);
  public final static native void SDBP_SetMasterTimeout(long jarg1, java.math.BigInteger jarg2);
  public final static native java.math.BigInteger SDBP_GetGlobalTimeout(long jarg1);
  public final static native java.math.BigInteger SDBP_GetMasterTimeout(long jarg1);
  public final static native java.math.BigInteger SDBP_GetDatabaseID(long jarg1, String jarg2);
  public final static native java.math.BigInteger SDBP_GetTableID(long jarg1, java.math.BigInteger jarg2, String jarg3);
  public final static native int SDBP_UseDatabase(long jarg1, String jarg2);
  public final static native int SDBP_UseTable(long jarg1, String jarg2);
  public final static native int SDBP_Get(long jarg1, String jarg2);
  public final static native int SDBP_Set(long jarg1, String jarg2, String jarg3);
  public final static native int SDBP_Delete(long jarg1, String jarg2);
  public final static native boolean SDBP_IsBatched(long jarg1);
  public final static native void SDBP_SetTrace(boolean jarg1);
}
