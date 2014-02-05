#include <Eris/Types.h>

#include <Eris/PollDefault.h>
#include <Eris/Exceptions.h>
#include <Eris/DeleteLater.h>
#include <Eris/Log.h>
#include <Eris/TimedEventService.h>

bool Eris::Poll::new_timeout_ = false;

Eris::Poll& Eris::Poll::instance()
{
  if(!_inst)
    _inst = new PollDefault();

  return *_inst;
}

void Eris::Poll::setInstance(Poll* p)
{
  if(_inst)
    throw InvalidOperation("Can't set poll instance, already have one");

  _inst = p;
}

Eris::Poll* Eris::Poll::_inst = 0;

namespace Eris {

void PollDefault::doPoll(unsigned long timeout)
{
}

void PollDefault::poll(unsigned long timeout)
{
  // This will throw if someone is using another kind
  // of poll, and that's a good thing.
  PollDefault &inst = dynamic_cast<PollDefault&>(Poll::instance());

#ifndef NDEBUG
  // Prevent reentrancy
  static bool already_polling = false;
  assert(!already_polling);
  already_polling = true;


  try {
#endif

    unsigned long wait_time = 0;
    inst.new_timeout_ = false;

    // This will only happen for timeout != 0
    while(wait_time < timeout) {
      inst.doPoll(wait_time);
      timeout -= wait_time;
      wait_time = TimedEventService::instance()->tick();
      if(inst.new_timeout_) {
        // Added a timeout, the time until it must be called
        // may be shorter than wait_time
        wait_time = 0;
        inst.new_timeout_ = false;
      }
    }

    inst.doPoll(timeout);
    TimedEventService::instance()->tick(true);
    execDeleteLaters();

#ifndef NDEBUG
    // We're done, turn off the reentrancy prevention flag
    assert(already_polling);
    already_polling = false;
  }
  catch(...) {
    already_polling = false;
    throw;
  }
#endif
}

int PollDefault::maxStreams() const
{
    return FD_SETSIZE;
}

int PollDefault::maxConnectingStreams() const
{
    return maxStreams();
}
}
