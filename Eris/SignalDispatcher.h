#ifndef ERIS_SIGNAL_DISPATCH_H
#define ERIS_SIGNAL_DISPATCH_H

#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

#include "Dispatcher.h"

namespace Eris {

template <class T>
class SignalDispatcher :
	public LeafDispatcher, 
	public SigC::Object
{
public:
	SignalDispatcher(const std::string &nm, const SigC::Slot1<void, const T& > &slot) :
		LeafDispatcher(nm)
	{ Signal.connect(slot); }

	virtual ~SignalDispatcher() {;}

	/// virtual interface for invoking the dispatcher; causes the signal to emit
	virtual bool dispatch(DispatchContextDeque &dq)
	{
		T object = T::Instantiate();
		Atlas::Message::Object::MapType::const_iterator I = dq.front().AsMap().begin();
		
		for (; I != dq.front().AsMap().end(); ++I)
        		object.SetAttr(I->first, I->second);
		Signal.emit(object);
		
		return LeafDispatcher::dispatch(dq);
	}

	/// invoked when the specified class is recieved
	SigC::Signal1<void, const T&> Signal;
protected:
	
};

class SignalDispatcher0 :
	public LeafDispatcher, 
	public SigC::Object
{
public:
	SignalDispatcher0(const std::string &nm, const SigC::Slot0<void> &slot) :
		LeafDispatcher(nm)
	{ Signal.connect(slot); }

	virtual ~SignalDispatcher0() {;}

	/// virtual interface for invoking the dispatcher; causes the signal to emit
	virtual bool dispatch(DispatchContextDeque &dq)
	{
		Signal.emit();
		return LeafDispatcher::dispatch(dq);
	}

	/// invoked when the specified class is recieved
	SigC::Signal0<void> Signal;
protected:
	
};

template <class T, class S>
class SignalDispatcher2 :
	public LeafDispatcher, 
	public SigC::Object
{
public:
	SignalDispatcher2(const std::string &nm, 
		const SigC::Slot2<void, const T&, const S& > &slot) :
		LeafDispatcher(nm)
	{ Signal.connect(slot); }

	virtual ~SignalDispatcher2() {;}

	/// virtual interface for invoking the dispatcher; causes the signal to emitted
	virtual bool dispatch(DispatchContextDeque &dq)
	{
		DispatchContextDeque::iterator Q = dq.begin();
		
		S object = S::Instantiate();
		Atlas::Message::Object::MapType::const_iterator I = Q->AsMap().begin();
		
		for (; I != Q->AsMap().end(); ++I)
        		object.SetAttr(I->first, I->second);
		++Q;
		T parent = T::Instantiate();
		I = Q->AsMap().begin();
		for (; I != Q->AsMap().end(); ++I)
        		parent.SetAttr(I->first, I->second);
		
		Signal.emit(parent, object);
		return LeafDispatcher::dispatch(dq);
	}

	/** Invoked when the specified class is recieved. The first argument is
	the parent, the second is the child. E.g for a Sight operation, declare
	the slot as MySlot(Operation::Sight &op, Entity::GameEntity &ent); */
	SigC::Signal2<void, const T&, const S&> Signal;
protected:
	
};

/** The message dispatcher provides a non-decoded output of the selected message. This is most
useful in conjunction with the EncapDispatcher, where the de-encapsulated object is not an Atlas::Object,
but simple some defined map or list structure.*/
class MessageDispatcher :
	public Dispatcher,
	public SigC::Object
{
public:	
	/** standard constructor
	@param nm The dispatcher label
	@param slot A SigC  slot that takes an Atlas::Message::Object */
	MessageDispatcher(const std::string &nm, const SigC::Slot1<void, const Atlas::Message::Object&> &slot) :
		Dispatcher(nm)
	{ Signal.connect(slot); }
	
	virtual ~MessageDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq)
	{ 
		Signal.emit(dq.front());
		return false;
	}
protected:	
	SigC::Signal1<void, const Atlas::Message::Object&> Signal;
};


} // of namespace

#endif
