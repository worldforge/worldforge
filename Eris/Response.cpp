#include <Eris/Response.h>
#include <Atlas/Objects/Operation.h>
#include <Eris/LogStream.h>

using namespace Atlas::Objects::Operation;

namespace Eris
{



void ResponseTracker::await(int serialno, ResponseBase* resp)
{
    assert(m_pending.count(serialno) == 0);
    m_pending[serialno] = resp;
}

bool ResponseTracker::handleOp(const RootOperation& op)
{
    if (op->getRefno() == 0) return false; // invalid refno, not a response op
    
    RefnoResponseMap::iterator it = m_pending.find(op->getRefno());
    if (it == m_pending.end()) {
        warning() << "received op with valid refno. but no response is registered";
        return false;
    }

    it->second->responseReceived(op);
    delete it->second;
    m_pending.erase(it);

    return true;
}

} // of namespace
