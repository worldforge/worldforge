#ifndef ERIS_POLL_WINSOCK_H
#define ERIS_POLL_WINSOCK_H 

#include <Eris/Poll.h>

namespace Eris {

class PollWinsock : public Poll, virtual public sigc::trackable
{
public:
	PollWinsock() {}
	virtual ~PollWinsock() {}

	virtual void addStream(const basic_socket*, Check);
	virtual void changeStream(const basic_socket*, Check);
	virtual void removeStream(const basic_socket*);

	static void poll(unsigned long timeout = 0);

	typedef std::map<const basic_socket*,Check> MapType;
private:
	MapType _streams;
	typedef MapType::iterator _iter;

	void doPoll(unsigned long timeout);
};

class PollDataWinsock : public PollData
{
public:
        PollDataWinsock(const PollWinsock::MapType&, bool&, unsigned long);

        virtual bool isReady(const basic_socket*);
private:
        typedef PollWinsock::MapType::const_iterator _iter;
        fd_set reading, writing, exceptions;
        SOCKET_TYPE maxfd;
};

} // namespace Eris

#endif // ERIS_POLL_WINSOCK_H
