#include "scheduler.hpp"
#include "../base/logging.hpp"
#include "event.hpp"

std::map<pthread_t, rrr::CoroMgr*> rrr::Coroutine::cmgr_map;

pthread_mutex_t coro_lock;
pthread_cond_t coro_cond;

void rrr::Coroutine::reg_cmgr(){
	pthread_t t = pthread_self();
	if (cmgr_map.find(t) != cmgr_map.end()){
		Log_error("current thread has been registered");
	}else{
		pthread_mutex_lock(&coro_lock);
		cmgr_map[t] = new CoroMgr();
		pthread_mutex_unlock(&coro_lock);
	}
}

void rrr::Coroutine::mkcoroutine(fp f){
	pthread_t t = pthread_self();
	if (cmgr_map.find(t) == cmgr_map.end()){
		Log_info("register Coroutine manager");
		reg_cmgr();
	}
	cmgr_map[t]->mkcoroutine(f);
}

void rrr::Coroutine::reg(coro_t::caller_type* cat){
	//verify(_c != NULL);
	pthread_t t = pthread_self();
	verify(cmgr_map.find(t) != cmgr_map.end());
	cmgr_map[t]->reg(cat);
}

void rrr::Coroutine::reg(coro_t* c, coro_t::caller_type* cat){
	pthread_t t = pthread_self();
	verify(cmgr_map.find(t) != cmgr_map.end());
	cmgr_map[t]->reg(c, cat);
}

coro_t* rrr::Coroutine::get_c(){
	pthread_t t = pthread_self();
	verify(cmgr_map.find(t) != cmgr_map.end());
	return cmgr_map[t]->get_c();
}

void rrr::Coroutine::yeild(){
}

void rrr::Coroutine::yeildto(coro_t *ct){	
}

void rrr::Coroutine::wait(Event* ev){
	pthread_t t = pthread_self();
	verify(cmgr_map.find(t) != cmgr_map.end());
	cmgr_map[t]->wait(ev);
}

void rrr::Coroutine::recovery(){
	pthread_t t = pthread_self();
	if (cmgr_map.find(t) == cmgr_map.end()){
		Log_info("register Coroutine manager");
		reg_cmgr();
	}
	cmgr_map[t]->recovery();
}

void rrr::Coroutine::init(){
	pthread_mutex_init(&coro_lock, NULL);
	pthread_cond_init(&coro_cond, NULL);
}

void rrr::CoroMgr::mkcoroutine(fp f){
	_c = new coro_t(f);
	c_set.push_back(_c);
//	recovery();
}

void rrr::CoroMgr::reg(coro_t::caller_type* ca){
	reg(_c, ca);
}

void rrr::CoroMgr::reg(coro_t* c, coro_t::caller_type* ca){
	if (_map.find(c) == _map.end()){
		_map[c] = ca;
	}else{
		verify(_map[c] == ca);
	}
}

coro_t* rrr::CoroMgr::get_c(){
	return _c;
}

void rrr::CoroMgr::recovery(){ 
	for (int i=0; i<c_set.size(); ){
		if (!(*c_set[i])){
			delete c_set[i];
			c_set.erase(c_set.begin() + i);
			continue;
		}else{
			Log_info("%x not finished", c_set[i]);
		}
		i++;
	} 
}

void rrr::CoroMgr::wait(Event* ev){
	if (ev->status() == Event::TRIGGER || 
		ev->status() == Event::CANCEL){
		return;
	}
	wait_event.push_back(ev);
	(*_ca)();
}

bool rrr::CoroMgr::search_all_trigger(){
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