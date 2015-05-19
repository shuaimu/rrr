#pragma once

#include "scheduler.hpp"

#ifdef COROUTINE
namespace rrr{
class Event{
public:
	enum status_t{
		WAIT, CANCEL, TRIGGER
	};
	CoroPair* cp;
	status_t _status;

	Event(CoroPair* t): cp(t), _status(WAIT){
	}
	void trigger(){
		_status = TRIGGER;	
	}

	void cancel(){
		_status = CANCEL;
	}

	status_t status(){
		return _status;
	}
};

}

#endif