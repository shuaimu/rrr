#pragma once

#include "scheduler.hpp"

#ifdef COROUTINE
namespace rrr{
class Event{
public:
	enum status_t{
		IDLE, WAIT, CANCEL, READY, TRIGGER
	};
	CoroPair* cp;
	status_t _status;

	Event(CoroPair* t): cp(t), _status(IDLE){
	}
	void trigger(){
		assert(_status == IDLE || _status == READY);
		_status = TRIGGER;	
	}
	void cancel(){
		assert(_status == READY || _status == WAIT);
		_status = CANCEL;
	}

	void ready(){
		assert(_status == WAIT);
		_status = READY;
	}

	void wait(){
		assert(_status == IDLE || _status == READY);
		_status = WAIT;
	}
	status_t status(){
		return _status;
	}
};

}

#endif