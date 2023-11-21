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
        logger->error("got Error error op from server without args");
        msg = "Unknown error.";
    } else {
        const Atlas::Objects::Root & arg = args.front();
        Atlas::Message::Element message;
        if (arg->copyAttr("message", message) != 0) {
            logger->error("got Error error op from server without message");
            msg = "Unknown error.";
        } else {
            if (!message.isString()) {
                logger->error("got Error error op from server with bad message");
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

void ResponseTracker::await(std::int64_t serial, Callback callback)
{
    m_pending.emplace(serial, std::move(callback));
}


void ResponseTracker::await(std::int64_t serialno, std::unique_ptr<ResponseBase> resp)
{
    assert(m_pending.count(serialno) == 0);
    std::shared_ptr<ResponseBase> holder = std::move(resp);
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
        logger->warn("received op with valid refno ({}) but no response is registered", op->getRefno());
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

void clearMemberResponse(sigc::notifiable* d)
{
    logger->debug("clearing out member response object");
	static_cast<ResponseBase*>(d)->detach();
}

} // of namespace
