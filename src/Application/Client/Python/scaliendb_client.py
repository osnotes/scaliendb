# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.31
#
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _scaliendb_client
import new
new_instancemethod = new.instancemethod
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
    raise AttributeError,name

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


class PySwigIterator(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PySwigIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PySwigIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
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
    def __eq__(*args): return _scaliendb_client.PySwigIterator___eq__(*args)
    def __ne__(*args): return _scaliendb_client.PySwigIterator___ne__(*args)
    def __iadd__(*args): return _scaliendb_client.PySwigIterator___iadd__(*args)
    def __isub__(*args): return _scaliendb_client.PySwigIterator___isub__(*args)
    def __add__(*args): return _scaliendb_client.PySwigIterator___add__(*args)
    def __sub__(*args): return _scaliendb_client.PySwigIterator___sub__(*args)
    def __iter__(self): return self
PySwigIterator_swigregister = _scaliendb_client.PySwigIterator_swigregister
PySwigIterator_swigregister(PySwigIterator)

class imaxdiv_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, imaxdiv_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, imaxdiv_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["quot"] = _scaliendb_client.imaxdiv_t_quot_set
    __swig_getmethods__["quot"] = _scaliendb_client.imaxdiv_t_quot_get
    if _newclass:quot = _swig_property(_scaliendb_client.imaxdiv_t_quot_get, _scaliendb_client.imaxdiv_t_quot_set)
    __swig_setmethods__["rem"] = _scaliendb_client.imaxdiv_t_rem_set
    __swig_getmethods__["rem"] = _scaliendb_client.imaxdiv_t_rem_get
    if _newclass:rem = _swig_property(_scaliendb_client.imaxdiv_t_rem_get, _scaliendb_client.imaxdiv_t_rem_set)
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
class SDBP_NodeParams(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, SDBP_NodeParams, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, SDBP_NodeParams, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _scaliendb_client.new_SDBP_NodeParams(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _scaliendb_client.delete_SDBP_NodeParams
    __del__ = lambda self : None;
    def Close(*args): return _scaliendb_client.SDBP_NodeParams_Close(*args)
    def AddNode(*args): return _scaliendb_client.SDBP_NodeParams_AddNode(*args)
    __swig_setmethods__["nodec"] = _scaliendb_client.SDBP_NodeParams_nodec_set
    __swig_getmethods__["nodec"] = _scaliendb_client.SDBP_NodeParams_nodec_get
    if _newclass:nodec = _swig_property(_scaliendb_client.SDBP_NodeParams_nodec_get, _scaliendb_client.SDBP_NodeParams_nodec_set)
    __swig_setmethods__["nodes"] = _scaliendb_client.SDBP_NodeParams_nodes_set
    __swig_getmethods__["nodes"] = _scaliendb_client.SDBP_NodeParams_nodes_get
    if _newclass:nodes = _swig_property(_scaliendb_client.SDBP_NodeParams_nodes_get, _scaliendb_client.SDBP_NodeParams_nodes_set)
    __swig_setmethods__["num"] = _scaliendb_client.SDBP_NodeParams_num_set
    __swig_getmethods__["num"] = _scaliendb_client.SDBP_NodeParams_num_get
    if _newclass:num = _swig_property(_scaliendb_client.SDBP_NodeParams_num_get, _scaliendb_client.SDBP_NodeParams_num_set)
SDBP_NodeParams_swigregister = _scaliendb_client.SDBP_NodeParams_swigregister
SDBP_NodeParams_swigregister(SDBP_NodeParams)

class SDBP_Buffer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, SDBP_Buffer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, SDBP_Buffer, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _scaliendb_client.new_SDBP_Buffer(*args)
        try: self.this.append(this)
        except: self.this = this
    def SetBuffer(*args): return _scaliendb_client.SDBP_Buffer_SetBuffer(*args)
    __swig_setmethods__["data"] = _scaliendb_client.SDBP_Buffer_data_set
    __swig_getmethods__["data"] = _scaliendb_client.SDBP_Buffer_data_get
    if _newclass:data = _swig_property(_scaliendb_client.SDBP_Buffer_data_get, _scaliendb_client.SDBP_Buffer_data_set)
    __swig_setmethods__["len"] = _scaliendb_client.SDBP_Buffer_len_set
    __swig_getmethods__["len"] = _scaliendb_client.SDBP_Buffer_len_get
    if _newclass:len = _swig_property(_scaliendb_client.SDBP_Buffer_len_get, _scaliendb_client.SDBP_Buffer_len_set)
    __swig_destroy__ = _scaliendb_client.delete_SDBP_Buffer
    __del__ = lambda self : None;
SDBP_Buffer_swigregister = _scaliendb_client.SDBP_Buffer_swigregister
SDBP_Buffer_swigregister(SDBP_Buffer)

SDBP_ResultClose = _scaliendb_client.SDBP_ResultClose
SDBP_ResultKey = _scaliendb_client.SDBP_ResultKey
SDBP_ResultValue = _scaliendb_client.SDBP_ResultValue
SDBP_ResultKeyBuffer = _scaliendb_client.SDBP_ResultKeyBuffer
SDBP_ResultValueBuffer = _scaliendb_client.SDBP_ResultValueBuffer
SDBP_ResultNumber = _scaliendb_client.SDBP_ResultNumber
SDBP_ResultDatabaseID = _scaliendb_client.SDBP_ResultDatabaseID
SDBP_ResultTableID = _scaliendb_client.SDBP_ResultTableID
SDBP_ResultBegin = _scaliendb_client.SDBP_ResultBegin
SDBP_ResultNext = _scaliendb_client.SDBP_ResultNext
SDBP_ResultIsEnd = _scaliendb_client.SDBP_ResultIsEnd
SDBP_ResultTransportStatus = _scaliendb_client.SDBP_ResultTransportStatus
SDBP_ResultCommandStatus = _scaliendb_client.SDBP_ResultCommandStatus
SDBP_Create = _scaliendb_client.SDBP_Create
SDBP_Init = _scaliendb_client.SDBP_Init
SDBP_Destroy = _scaliendb_client.SDBP_Destroy
SDBP_GetResult = _scaliendb_client.SDBP_GetResult
SDBP_SetGlobalTimeout = _scaliendb_client.SDBP_SetGlobalTimeout
SDBP_SetMasterTimeout = _scaliendb_client.SDBP_SetMasterTimeout
SDBP_GetGlobalTimeout = _scaliendb_client.SDBP_GetGlobalTimeout
SDBP_GetMasterTimeout = _scaliendb_client.SDBP_GetMasterTimeout
SDBP_CreateQuorum = _scaliendb_client.SDBP_CreateQuorum
SDBP_CreateDatabase = _scaliendb_client.SDBP_CreateDatabase
SDBP_CreateTable = _scaliendb_client.SDBP_CreateTable
SDBP_GetDatabaseID = _scaliendb_client.SDBP_GetDatabaseID
SDBP_GetTableID = _scaliendb_client.SDBP_GetTableID
SDBP_UseDatabase = _scaliendb_client.SDBP_UseDatabase
SDBP_UseTable = _scaliendb_client.SDBP_UseTable
SDBP_Get = _scaliendb_client.SDBP_Get
SDBP_GetCStr = _scaliendb_client.SDBP_GetCStr
SDBP_Set = _scaliendb_client.SDBP_Set
SDBP_SetIfNotExists = _scaliendb_client.SDBP_SetIfNotExists
SDBP_TestAndSet = _scaliendb_client.SDBP_TestAndSet
SDBP_Add = _scaliendb_client.SDBP_Add
SDBP_Delete = _scaliendb_client.SDBP_Delete
SDBP_Remove = _scaliendb_client.SDBP_Remove
SDBP_Begin = _scaliendb_client.SDBP_Begin
SDBP_Submit = _scaliendb_client.SDBP_Submit
SDBP_Cancel = _scaliendb_client.SDBP_Cancel
SDBP_IsBatched = _scaliendb_client.SDBP_IsBatched
SDBP_SetTrace = _scaliendb_client.SDBP_SetTrace


