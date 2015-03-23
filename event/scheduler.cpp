#include "scheduler.hpp"

coro_t::caller_type* rrr::Coroutine::ca = NULL;
coro_t* rrr::Coroutine::c = NULL;

void rrr::Coroutine::mk_coro(fp f, coro_t* ct){
	c = new coro_t(f);	
	if (ct){
		ct = c;
	}
}

coro_t* rrr::Coroutine::get_cur_coro(){
	return c;
}

void rrr::Coroutine::set_ca(coro_t::caller_type* cat){
	ca = cat;
}

coro_t::caller_type* rrr::Coroutine::get_cur_ca(){
	return ca;
}