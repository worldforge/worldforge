#include "wrapperClient.h"
#include "wrapperObject.h"
#include <assert.h>

void    wrapperClient::gotMsg(const Object& msg)
{
  //CHEAT: hmm... what if got this before setting m_instance?
  //should not happen: before poll is called, instance is set
  PyObject *obj = AtlasWrapperObject_FromAtlasObject(msg);
  if(!obj) return; //let caller handle errors
  PyObject *res = PyObject_CallMethod(m_instance,"gotMsg","(O)",obj);
  Py_XDECREF(res);
}

void    wrapperClient::gotDisconnect()
{
  PyObject *res = PyObject_CallMethod(m_instance,"gotDisconnect","()");
  Py_XDECREF(res);
}

void wrapperClient::setPythonInstance(PyObject *instance)
{
  m_instance = instance;
}
