#pragma once

#include "scheduler.hpp"

namespace rrr{
class Event{
public:
	enum status_t{
		WAIT, CANCEL, TRIGGER
	};
	status_t _status;

	coro_t::caller_type* ca;

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
