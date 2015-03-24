#include "scheduler.hpp"
#include "../base/logging.hpp"
#include "event.hpp"

class Event;

coro_t* rrr::Coroutine::_ct;
coro_t::caller_type* rrr::Coroutine::_cat;
coro_t rrr::Coroutine::_c;
coro_t::caller_type rrr::Coroutine::_ca;


std::map< coro_t*, coro_t::caller_type* > rrr::Coroutine::_map;

std::vector<rrr::Event*> rrr::EventMgr::wait_event;
std::vector<rrr::Event*> rrr::EventMgr::trigger_event;

void rrr::Coroutine::mkcoroutine(fp f, coro_t* ct){
	// _c = new coro_t(f);	
	_c = coro_t(f);
}

void rrr::Coroutine::init(coro_t::caller_type* cat){
	//verify(_c != NULL);
	init(&_c, cat);
}

void rrr::Coroutine::init(coro_t* c, coro_t::caller_type* ca){
	if (_map.find(c) == _map.end()){
		_map[c] = ca;
	}else{
		verify(_map[c] == ca);
	}
}

coro_t* rrr::Coroutine::get_c(){
	return &_c;
}

void rrr::Coroutine::yeild(){
	coro_t* tmp = &_c;
	(*_map[tmp])();
}

void rrr::Coroutine::yeildto(coro_t *ct){
	_ct = ct;
	(*ct)();
}

void rrr::EventMgr::wait(Event* ev){
	if (ev->status() == Event::TRIGGER || 
		ev->status() == Event::CANCEL){
		return;
	}
	wait_event.push_back(ev);
	Coroutine::yeild(); 
}

bool rrr::EventMgr::search_all_trigger(){
	bool find = false;
	auto ite = wait_event.begin();
	for (; ite != wait_event.end(); ite++){
		Event* ev = (Event*)(*ite);
		if (ev->status() == Event::TRIGGER){
			wait_event.erase(ite);
			trigger_event.push_back(ev);
			find = true;
		}
	}
	return find; 
}