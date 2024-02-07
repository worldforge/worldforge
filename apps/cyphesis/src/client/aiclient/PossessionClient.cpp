/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#endif

#include "PossessionClient.h"
#include "PossessionAccount.h"

#include "Remotery.h"
#include "common/operations/Possess.h"
#include "common/id.h"
#include "common/custom.h"

#include "common/debug.h"
#include "common/CommSocket.h"

#include <Atlas/Objects/Entity.h>

#include <chrono>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::RootOperation;

size_t PossessionClient::operations_in = 0;
size_t PossessionClient::operations_out = 0;

PossessionClient::PossessionClient(CommSocket& commSocket,
								   MindKit& mindFactory,
								   std::function<void()> reconnectFn) :
		BaseClient(commSocket),
		m_startTime{std::chrono::steady_clock::now()},
		m_mindFactory(mindFactory),
		m_reconnectFn(std::move(reconnectFn)),
		m_account(nullptr),
		m_operationsDispatcher([&](const Operation& op, Ref<BaseMind> from) { this->operationFromEntity(op, std::move(from)); },
							   [&]() -> std::chrono::steady_clock::duration { return getTime(); }),
		m_dispatcherTimer(commSocket.m_io_context),
		m_serverLocalTimeDiff(0) {


}


PossessionClient::~PossessionClient() {
	if (m_reconnectFn) {
		m_reconnectFn();
	}
}

void PossessionClient::notifyAccountCreated(RouterId accountId) {
	spdlog::info("Creating possession account on server.");
	m_account = std::make_unique<PossessionAccount>(accountId, m_mindFactory, *this);
	OpVector res;
	m_account->enablePossession(res);
	for (auto& op: res) {
		send(op);
	}
}


void PossessionClient::operationFromEntity(const Operation& op, Ref<BaseMind> mind) {
	if (!mind->isDestroyed()) {
		rmt_ScopedCPUSample(operationFromEntity, 0)

		OpVector res;
		//Adjust the time of the operation to fit with the server's time
		op->setStamp(op->getStamp() - m_serverLocalTimeDiff.count());
		processOperation(op, res);
		operations_out += res.size();
		send(res);
	}
}

void PossessionClient::operation(const Operation& op, OpVector& res) {
	rmt_ScopedCPUSample(operation, 0)
	operations_in++;
	if (!op->isDefaultStamp()) {
		//Store the difference between server time and local time, so we can properly adjust the time of any locally scheduled ops when they are dispatched.
		m_serverLocalTimeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(getTime()) - std::chrono::milliseconds(op->getStamp());
	}
	processOperation(op, res);
	operations_out += res.size();
}

void PossessionClient::resolveDispatchTimeForOp(Atlas::Objects::Operation::RootOperationData& op) {
	if (!op.isDefaultFutureMilliseconds()) {
		auto timeNow = std::chrono::steady_clock::now() - m_startTime;
		auto timeAsMilliseSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow);
		auto t = timeAsMilliseSeconds + std::chrono::milliseconds(op.getFutureMilliseconds());
		op.setStamp(t.count());
		op.removeAttrFlag(Atlas::Objects::Operation::FUTURE_MILLISECONDS_FLAG);
	} else if (op.isDefaultStamp()) {
		op.setStamp(std::chrono::duration_cast<std::chrono::milliseconds>(getTime()).count());
	}
}

void PossessionClient::processResponses(const OpVector& incomingRes, OpVector& outgoingRes) {
	bool updatedDispatcher = false;

	for (auto& resOp: incomingRes) {
		if (debug_flag) {
			std::cout << "PossessionClient::operation return {" << std::endl;
			debug_dump(resOp, std::cout);
			std::cout << "}" << std::endl;
		}
		//Any op with both "from" and "to" set should be re-sent.
		if ((!resOp->isDefaultTo() && !resOp->isDefaultFrom())) {
			auto mind = m_account->findMindForId(resOp->getTo());
			if (mind) {
				resolveDispatchTimeForOp(*resOp);
				m_operationsDispatcher.addOperationToQueue(resOp, std::move(mind));
				updatedDispatcher = true;
			} else {
				spdlog::warn("Resulting op of type '{}' is set to the mind with id '{}', which can't be found.", resOp->getParent(), resOp->getTo());
			}
		} else {
			//            if (resOp->getClassNo() != Atlas::Objects::Operation::TICK_NO) {
			//                spdlog::info("Out {} from {}", resOp->getParent(), resOp->getFrom());
			//            }
			outgoingRes.emplace_back(resOp);
		}
	}
	if (updatedDispatcher) {
		scheduleDispatch();
	}
}


void PossessionClient::processOperation(const Operation& op, OpVector& res) {
	if (debug_flag) {
		std::cout << "PossessionClient::operation received {" << std::endl;
		debug_dump(op, std::cout);
		std::cout << "}" << std::endl;
	}

	OpVector accountRes;
	m_account->operation(op, accountRes);
	processResponses(accountRes, res);
}

std::chrono::steady_clock::duration PossessionClient::getTime() const {
	return std::chrono::steady_clock::now() - m_startTime;
}


const std::unordered_map<std::string, Ref<BaseMind>>& PossessionClient::getMinds() const {
	return m_account->getMinds();
}


void PossessionClient::scheduleDispatch() {
	m_dispatcherTimer.cancel();
	auto waitTime = m_operationsDispatcher.timeUntilNextOp(getTime());
#if BOOST_VERSION >= 106600
	m_dispatcherTimer.expires_after(waitTime);
#else
	m_dispatcherTimer.expires_from_now(waitTime);
#endif

	m_dispatcherTimer.async_wait([&](boost::system::error_code ec) {
		if (!ec) {
			//process at least one op
			auto currentTime = getTime();
			m_operationsDispatcher.processUntil(currentTime, std::chrono::microseconds(100));
			scheduleDispatch();
		}
	});

}
