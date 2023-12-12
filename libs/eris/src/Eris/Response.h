#ifndef ERIS_RESPONSE_H
#define ERIS_RESPONSE_H

#include "Router.h"

#include <Atlas/Objects/ObjectsFwd.h>
#include <unordered_map>
#include <functional>
#include <memory>
#include <cstdint>
#include <string>
#include <sigc++/trackable.h>

namespace Eris {

std::string getErrorMessage(const Atlas::Objects::Operation::RootOperation& err);


class ResponseBase : public sigc::notifiable {
public:
	virtual ~ResponseBase();

	virtual void detach() {}

	/**
	Process a response.
	*/
	virtual Router::RouterResult responseReceived(const Atlas::Objects::Operation::RootOperation& op) = 0;
};

class NullResponse : public ResponseBase {
public:
	Router::RouterResult responseReceived(const Atlas::Objects::Operation::RootOperation&) override;
};

void clearMemberResponse(sigc::notifiable*);

template<class T>
class MemberResponse : public ResponseBase {
public:
	typedef void (T::*T_method)(const Atlas::Objects::Operation::RootOperation& op);

	MemberResponse(T* obj, void (T::*method)(const Atlas::Objects::Operation::RootOperation& op)) :
			m_object(obj),
			m_func(method) {
		obj->add_destroy_notify_callback(this, &clearMemberResponse);
	}

	~MemberResponse() override {
		if (m_object) m_object->remove_destroy_notify_callback(this);
	}

	Router::RouterResult responseReceived(const Atlas::Objects::Operation::RootOperation& op) override {
		if (m_object) (m_object->*m_func)(op);
		return Router::HANDLED;
	}

	void detach() override {
		m_object = nullptr;
	}

private:
	T* m_object;
	T_method m_func;
};

class ResponseTracker {
public:

	typedef std::function<Router::RouterResult(const Atlas::Objects::Operation::RootOperation& op)> Callback;

	~ResponseTracker();

	void await(std::int64_t serialno, std::unique_ptr<ResponseBase>);

	void await(std::int64_t serial, Callback callback);

	template<class T>
	void await(std::int64_t serial, T* ins, void (T::*method)(const Atlas::Objects::Operation::RootOperation& op)) {
		await(serial, std::make_unique<MemberResponse<T>>(ins, method));
	}

	void ignore(std::int64_t serial) {
		await(serial, std::make_unique<NullResponse>());
	}

	Router::RouterResult handleOp(const Atlas::Objects::Operation::RootOperation& op);

private:
	std::unordered_map<std::int64_t, Callback> m_pending;
};

} // of namespace

#endif // of ERIS_RESPONSE_H
