/*************************************************************************
 > File Name: sync_call_async.cpp
 > Author: Mengxing Liu
 > Created Time: 2015-03-15
 ************************************************************************/

#include <boost/coroutine/all.hpp>
#include <iostream>

typedef boost::coroutines::coroutine< void(void)> coro_t;

coro_t* ca_t;

void _async(){
	// do something 
	
	// yield and return to the caller
	(*ca_t)();

	// do something else
	
	// trigger the event to remind the caller the job has been finished
}

void temp(){
	std::cout << "before async" << std::endl;
	_async();
	std::cout << "after async" << std::endl;
}

void _sync(coro_t::caller_type &ca){
	coro_t *c;
	ca_t = &ca;

	std::cout << "before temp" << std::endl;
	temp();
	std::cout << "after temp" << std::endl;
}

int main(){
	coro_t c(_sync);
	c();
}
