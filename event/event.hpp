#pragma once

#include "scheduler.hpp"

namespace rrr{
class Event{
public:
	enum status_t{
		WAIT, CANCEL, TRIGGER
	};
	status_t _status;

	coro_t* coro;

	Event(coro_t* t): coro(t), _status(WAIT){
	}
	void trigger(){
		_status = TRIGGER;	
	}

	void cancel(){
		_status = CANCEL;
	}

	bool status(){
		return _status;
	}
};

}
