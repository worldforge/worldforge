%module ccAtlasNet
%{
#include <Atlas/Object/Object.h>
#include <Atlas/Net/Socket.h>
#include <Atlas/Net/TCPSocket.h>
#include <Atlas/Net/Client.h>
#include <Atlas/Net/Codec.h>
#include <Atlas/Net/XMLProtocol.h>
#include <Atlas/Net/PackedProtocol.h>
#include "wrapperObject.h"
#include "wrapperClient.h"
using namespace Atlas;
%}

%typemap(python,in) const std::string&(string string_temp) {
  if(!PyString_Check($source)) {
      PyErr_SetString(PyExc_TypeError, "Need a string object!");
      return nullptr;
  }
  string_temp=string(PyString_AsString($source));
  $target=&string_temp;
}

%typemap(python,out) const std::string {
  $target = PyString_FromStringAndSize($source->c_str(),$source->length());
}

%typemap(python,in) const Object&(Atlas::Object obj_temp) {
  if(!AtlasWrapperObject_Check($source)) {
      PyErr_SetString(PyExc_TypeError, "Need a wrapper for Atlas::Object!");
      return nullptr;
  }
  obj_temp=*((AtlasWrapperObject*)$source)->obj;
  $target=&obj_temp;
}

%typemap(python,out) Object& {
  $target = AtlasWrapperObject_FromAtlasObject(*$source);
}

%include Socket.i
%include TCPSocket.i

%include wrapperClient.h

%include Codec.i

%include Protocol.i
%include XMLProtocol.i
%include PackedProtocol.i
