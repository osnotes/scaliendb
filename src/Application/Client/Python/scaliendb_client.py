# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.31
#
# Don't modify this file, modify the SWIG interface instead.

import _scaliendb_client


try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


class PySwigIterator(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError( "No constructor defined")
    __repr__ = _swig_repr
    __swig_destroy__ = _scaliendb_client.delete_PySwigIterator
    __del__ = lambda self : None;
    def value(*args): return _scaliendb_client.PySwigIterator_value(*args)
    def incr(*args): return _scaliendb_client.PySwigIterator_incr(*args)
    def decr(*args): return _scaliendb_client.PySwigIterator_decr(*args)
    def distance(*args): return _scaliendb_client.PySwigIterator_distance(*args)
    def equal(*args): return _scaliendb_client.PySwigIterator_equal(*args)
    def copy(*args): return _scaliendb_client.PySwigIterator_copy(*args)
    def next(*args): return _scaliendb_client.PySwigIterator_next(*args)
    def previous(*args): return _scaliendb_client.PySwigIterator_previous(*args)
    def advance(*args): return _scaliendb_client.PySwigIterator_advance(*args)

    def __ne__(*args): return _scaliendb_client.PySwigIterator___ne__(*args)
    def __iadd__(*args): return _scaliendb_client.PySwigIterator___iadd__(*args)
    def __isub__(*args): return _scaliendb_client.PySwigIterator___isub__(*args)
    def __add__(*args): return _scaliendb_client.PySwigIterator___add__(*args)
    def __sub__(*args): return _scaliendb_client.PySwigIterator___sub__(*args)
    def __iter__(self): return self
PySwigIterator_swigregister = _scaliendb_client.PySwigIterator_swigregister
PySwigIterator_swigregister(PySwigIterator)

class imaxdiv_t(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    quot = _swig_property(_scaliendb_client.imaxdiv_t_quot_get, _scaliendb_client.imaxdiv_t_quot_set)
    rem = _swig_property(_scaliendb_client.imaxdiv_t_rem_get, _scaliendb_client.imaxdiv_t_rem_set)
    def __init__(self, *args): 
        this = _scaliendb_client.new_imaxdiv_t(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _scaliendb_client.delete_imaxdiv_t
    __del__ = lambda self : None;
imaxdiv_t_swigregister = _scaliendb_client.imaxdiv_t_swigregister
imaxdiv_t_swigregister(imaxdiv_t)

imaxabs = _scaliendb_client.imaxabs
imaxdiv = _scaliendb_client.imaxdiv
strtoimax = _scaliendb_client.strtoimax
strtoumax = _scaliendb_client.strtoumax
wcstoimax = _scaliendb_client.wcstoimax
wcstoumax = _scaliendb_client.wcstoumax
class SDBP_NodeParams(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _scaliendb_client.new_SDBP_NodeParams(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _scaliendb_client.delete_SDBP_NodeParams
    __del__ = lambda self : None;
    def Close(*args): return _scaliendb_client.SDBP_NodeParams_Close(*args)
    def AddNode(*args): return _scaliendb_client.SDBP_NodeParams_AddNode(*args)
    nodec = _swig_property(_scaliendb_client.SDBP_NodeParams_nodec_get, _scaliendb_client.SDBP_NodeParams_nodec_set)
    nodes = _swig_property(_scaliendb_client.SDBP_NodeParams_nodes_get, _scaliendb_client.SDBP_NodeParams_nodes_set)
    num = _swig_property(_scaliendb_client.SDBP_NodeParams_num_get, _scaliendb_client.SDBP_NodeParams_num_set)
SDBP_NodeParams_swigregister = _scaliendb_client.SDBP_NodeParams_swigregister
SDBP_NodeParams_swigregister(SDBP_NodeParams)

class SDBP_Buffer(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _scaliendb_client.new_SDBP_Buffer(*args)
        try: self.this.append(this)
        except: self.this = this
    def SetBuffer(*args): return _scaliendb_client.SDBP_Buffer_SetBuffer(*args)
    data = _swig_property(_scaliendb_client.SDBP_Buffer_data_get, _scaliendb_client.SDBP_Buffer_data_set)
    len = _swig_property(_scaliendb_client.SDBP_Buffer_len_get, _scaliendb_client.SDBP_Buffer_len_set)
    __swig_destroy__ = _scaliendb_client.delete_SDBP_Buffer
    __del__ = lambda self : None;
SDBP_Buffer_swigregister = _scaliendb_client.SDBP_Buffer_swigregister
SDBP_Buffer_swigregister(SDBP_Buffer)

SDBP_ResultClose = _scaliendb_client.SDBP_ResultClose
SDBP_ResultKey = _scaliendb_client.SDBP_ResultKey
SDBP_ResultValue = _scaliendb_client.SDBP_ResultValue
SDBP_ResultKeyBuffer = _scaliendb_client.SDBP_ResultKeyBuffer
SDBP_ResultValueBuffer = _scaliendb_client.SDBP_ResultValueBuffer
SDBP_ResultSignedNumber = _scaliendb_client.SDBP_ResultSignedNumber
SDBP_ResultNumber = _scaliendb_client.SDBP_ResultNumber
SDBP_ResultIsConditionalSuccess = _scaliendb_client.SDBP_ResultIsConditionalSuccess
SDBP_ResultDatabaseID = _scaliendb_client.SDBP_ResultDatabaseID
SDBP_ResultTableID = _scaliendb_client.SDBP_ResultTableID
SDBP_ResultBegin = _scaliendb_client.SDBP_ResultBegin
SDBP_ResultNext = _scaliendb_client.SDBP_ResultNext
SDBP_ResultIsEnd = _scaliendb_client.SDBP_ResultIsEnd
SDBP_ResultIsFinished = _scaliendb_client.SDBP_ResultIsFinished
SDBP_ResultTransportStatus = _scaliendb_client.SDBP_ResultTransportStatus
SDBP_ResultCommandStatus = _scaliendb_client.SDBP_ResultCommandStatus
SDBP_ResultNumNodes = _scaliendb_client.SDBP_ResultNumNodes
SDBP_ResultNodeID = _scaliendb_client.SDBP_ResultNodeID
SDBP_ResultElapsedTime = _scaliendb_client.SDBP_ResultElapsedTime
SDBP_Create = _scaliendb_client.SDBP_Create
SDBP_Init = _scaliendb_client.SDBP_Init
SDBP_Destroy = _scaliendb_client.SDBP_Destroy
SDBP_GetResult = _scaliendb_client.SDBP_GetResult
SDBP_SetGlobalTimeout = _scaliendb_client.SDBP_SetGlobalTimeout
SDBP_SetMasterTimeout = _scaliendb_client.SDBP_SetMasterTimeout
SDBP_GetGlobalTimeout = _scaliendb_client.SDBP_GetGlobalTimeout
SDBP_GetMasterTimeout = _scaliendb_client.SDBP_GetMasterTimeout
SDBP_GetJSONConfigState = _scaliendb_client.SDBP_GetJSONConfigState
SDBP_WaitConfigState = _scaliendb_client.SDBP_WaitConfigState
SDBP_SetConsistencyMode = _scaliendb_client.SDBP_SetConsistencyMode
SDBP_SetBatchMode = _scaliendb_client.SDBP_SetBatchMode
SDBP_SetBatchLimit = _scaliendb_client.SDBP_SetBatchLimit
SDBP_CreateDatabase = _scaliendb_client.SDBP_CreateDatabase
SDBP_RenameDatabase = _scaliendb_client.SDBP_RenameDatabase
SDBP_DeleteDatabase = _scaliendb_client.SDBP_DeleteDatabase
SDBP_CreateTable = _scaliendb_client.SDBP_CreateTable
SDBP_RenameTable = _scaliendb_client.SDBP_RenameTable
SDBP_DeleteTable = _scaliendb_client.SDBP_DeleteTable
SDBP_TruncateTable = _scaliendb_client.SDBP_TruncateTable
SDBP_GetNumQuorums = _scaliendb_client.SDBP_GetNumQuorums
SDBP_GetQuorumIDAt = _scaliendb_client.SDBP_GetQuorumIDAt
SDBP_GetQuorumNameAt = _scaliendb_client.SDBP_GetQuorumNameAt
SDBP_GetNumDatabases = _scaliendb_client.SDBP_GetNumDatabases
SDBP_GetDatabaseIDAt = _scaliendb_client.SDBP_GetDatabaseIDAt
SDBP_GetDatabaseNameAt = _scaliendb_client.SDBP_GetDatabaseNameAt
SDBP_GetNumTables = _scaliendb_client.SDBP_GetNumTables
SDBP_GetTableIDAt = _scaliendb_client.SDBP_GetTableIDAt
SDBP_GetTableNameAt = _scaliendb_client.SDBP_GetTableNameAt
SDBP_Get = _scaliendb_client.SDBP_Get
SDBP_GetCStr = _scaliendb_client.SDBP_GetCStr
SDBP_Set = _scaliendb_client.SDBP_Set
SDBP_SetCStr = _scaliendb_client.SDBP_SetCStr
SDBP_Add = _scaliendb_client.SDBP_Add
SDBP_AddCStr = _scaliendb_client.SDBP_AddCStr
SDBP_Delete = _scaliendb_client.SDBP_Delete
SDBP_DeleteCStr = _scaliendb_client.SDBP_DeleteCStr
SDBP_ListKeys = _scaliendb_client.SDBP_ListKeys
SDBP_ListKeysCStr = _scaliendb_client.SDBP_ListKeysCStr
SDBP_ListKeyValues = _scaliendb_client.SDBP_ListKeyValues
SDBP_ListKeyValuesCStr = _scaliendb_client.SDBP_ListKeyValuesCStr
SDBP_Count = _scaliendb_client.SDBP_Count
SDBP_CountCStr = _scaliendb_client.SDBP_CountCStr
SDBP_Begin = _scaliendb_client.SDBP_Begin
SDBP_Submit = _scaliendb_client.SDBP_Submit
SDBP_Cancel = _scaliendb_client.SDBP_Cancel
SDBP_SetTrace = _scaliendb_client.SDBP_SetTrace
SDBP_SetLogFile = _scaliendb_client.SDBP_SetLogFile
SDBP_GetVersion = _scaliendb_client.SDBP_GetVersion
SDBP_GetDebugString = _scaliendb_client.SDBP_GetDebugString


