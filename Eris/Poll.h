#ifndef ERIS_POLL_H
#define ERIS_POLL_H 

#include <sigc++/signal.h>

namespace Eris {

class Poll
{
public:
	Poll() {}
	virtual ~Poll() {}


private:
	Poll(const Poll&);
};

} // namespace Eris

#endif // ERIS_POLL_H
