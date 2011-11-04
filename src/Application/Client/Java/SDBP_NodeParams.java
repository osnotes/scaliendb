/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.scalien.scaliendb;

public class SDBP_NodeParams {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected SDBP_NodeParams(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(SDBP_NodeParams obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        scaliendb_clientJNI.delete_SDBP_NodeParams(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public SDBP_NodeParams(int nodec_) {
    this(scaliendb_clientJNI.new_SDBP_NodeParams(nodec_), true);
  }

  public void Close() {
    scaliendb_clientJNI.SDBP_NodeParams_Close(swigCPtr, this);
  }

  public void AddNode(String node) {
    scaliendb_clientJNI.SDBP_NodeParams_AddNode(swigCPtr, this, node);
  }

  public void setNodec(int value) {
    scaliendb_clientJNI.SDBP_NodeParams_nodec_set(swigCPtr, this, value);
  }

  public int getNodec() {
    return scaliendb_clientJNI.SDBP_NodeParams_nodec_get(swigCPtr, this);
  }

  public void setNodes(SWIGTYPE_p_p_char value) {
    scaliendb_clientJNI.SDBP_NodeParams_nodes_set(swigCPtr, this, SWIGTYPE_p_p_char.getCPtr(value));
  }

  public SWIGTYPE_p_p_char getNodes() {
    long cPtr = scaliendb_clientJNI.SDBP_NodeParams_nodes_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_p_char(cPtr, false);
  }

  public void setNum(int value) {
    scaliendb_clientJNI.SDBP_NodeParams_num_set(swigCPtr, this, value);
  }

  public int getNum() {
    return scaliendb_clientJNI.SDBP_NodeParams_num_get(swigCPtr, this);
  }

}
