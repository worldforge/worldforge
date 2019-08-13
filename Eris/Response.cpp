#include "Response.h"
#include <Atlas/Objects/Operation.h>
#include <memory>
#include "LogStream.h"

using namespace Atlas::Objects::Operation;

namespace Eris
{

std::string getErrorMessage(const RootOperation & err)
{
    std::string msg;
    const std::vector<Atlas::Objects::Root>& args = err->getArgs();
    if (args.empty()) {
        error() << "got Error error op from server without args";
        msg = "Unknown error.";
    } else {
        const Atlas::Objects::Root & arg = args.front();
        Atlas::Message::Element message;
        if (arg->copyAttr("message", message) != 0) {
            error() << "got Error error op from server without message";
            msg = "Unknown error.";
        } else {
            if (!message.isString()) {
                error() << "got Error error op from server with bad message";
                msg = "Unknown error.";
            } else {
                msg = message.String();
            }
        }
    }
    return msg;
}

ResponseBase::~ResponseBase() = default;

ResponseTracker::~ResponseTracker() = default;

void ResponseTracker::await(long serial, Callback callback)
{
    m_pending.emplace(serial, std::move(callback));
}


void ResponseTracker::await(long serialno, ResponseBase* resp)
{
    assert(m_pending.count(serialno) == 0);
    std::shared_ptr<ResponseBase> holder(resp);
    await(serialno, [holder](const Atlas::Objects::Operation::RootOperation& op)->Router::RouterResult{
        auto result = holder->responseReceived(op);
        return result ? Router::HANDLED : Router::IGNORED;
    });
}

Router::RouterResult ResponseTracker::handleOp(const RootOperation& op)
{
    if (op->isDefaultRefno()) return Router::IGNORED; // invalid refno, not a response op

    auto it = m_pending.find(op->getRefno());
    if (it == m_pending.end()) {
        warning() << "received op with valid refno (" << op->getRefno() << 
            ") but no response is registered";
        return Router::IGNORED;
    }

// order here is important, so the responseReceived can re-await the op
    auto resp = it->second;
    m_pending.erase(it);

    auto result = resp(op);

    return result;
}

Router::RouterResult NullResponse::responseReceived(const Atlas::Objects::Operation::RootOperation&)
{
    //debug() << "nullresponse, ignoring op with refno " << op->getRefno();
    return Router::RouterResult::IGNORED;
}

void* clearMemberResponse(void* d)
{
    debug() << "clearing out member response object";
    
    void** objectPointer = (void**) d;
    *objectPointer = nullptr;
    return nullptr;
}

} // of namespace
