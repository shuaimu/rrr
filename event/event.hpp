#pragma once

#include "scheduler.hpp"

#ifdef COROUTINE
namespace rrr{
class Event{
public:
	enum status_t{
		WAIT, CANCEL, TRIGGER
	};
	coro_t::caller_type* ca;
	status_t _status;

	Event(coro_t::caller_type* t): ca(t), _status(WAIT){
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