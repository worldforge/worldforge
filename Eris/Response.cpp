#include <Eris/Response.h>
#include <Atlas/Objects/Operation.h>
#include <Eris/LogStream.h>

using namespace Atlas::Objects::Operation;

namespace Eris
{

ResponseBase::~ResponseBase()
{
}

ResponseTracker::~ResponseTracker()
{
    //clean up any lingering responses
    for (RefnoResponseMap::iterator it = m_pending.begin(); it != m_pending.end(); ++it) {
        delete it->second;
    }
}

void ResponseTracker::await(int serialno, ResponseBase* resp)
{
    assert(m_pending.count(serialno) == 0);
    m_pending[serialno] = resp;
}

bool ResponseTracker::handleOp(const RootOperation& op)
{
    if (op->isDefaultRefno()) return false; // invalid refno, not a response op
    
    RefnoResponseMap::iterator it = m_pending.find(op->getRefno());
    if (it == m_pending.end()) {
        warning() << "received op with valid refno (" << op->getRefno() << 
            ") but no response is registered";
        return false;
    }

// order here is important, so the responseReceived can re-await the op
    ResponseBase* resp = it->second;
    m_pending.erase(it);

    bool result = resp->responseReceived(op);
    delete resp;

    return result;
}

bool NullResponse::responseReceived(const Atlas::Objects::Operation::RootOperation&)
{
    //debug() << "nullresponse, ignoring op with refno " << op->getRefno();
    return false;
}

void* clearMemberResponse(void* d)
{
    debug() << "clearing out member response object";
    
    void** objectPointer = (void**) d;
    *objectPointer = NULL;
    return NULL;
}

} // of namespace
