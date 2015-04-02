#include "scheduler.hpp"
#include "../base/logging.hpp"
#include "event.hpp"
#include "fastStackAllocator.hpp"
#include <vector>

namespace rrr{
std::map<pthread_t, CoroMgr*> Coroutine::cmgr_map;

pthread_mutex_t coro_lock;
pthread_cond_t coro_cond;

MyAllocator stack_allocator;
boost::coroutines::attributes attr;
std::allocator<coro_t> allo;

bool in_coroutine = false;
pthread_t work_pid = 0;

void Coroutine::reg_cmgr(pthread_t t){
	if (cmgr_map.find(t) != cmgr_map.end()){
		Log_error("current thread has been registered");
	}else{
		pthread_mutex_lock(&coro_lock);
		cmgr_map[t] = new CoroMgr();
		pthread_mutex_unlock(&coro_lock);
	}
}
void Coroutine::reg_cmgr(){
	pthread_t t = pthread_self();
	reg_cmgr(t);
}

CoroMgr* Coroutine::get_cmgr(pthread_t pid){
	if (cmgr_map.find(pid) == cmgr_map.end()){
		reg_cmgr(pid);
	}
	return cmgr_map[pid];
}

void Coroutine::mkcoroutine(fp f){
	pthread_t t = pthread_self();
	if (cmgr_map.find(t) == cmgr_map.end()){
		Log_info("register Coroutine manager");
		reg_cmgr();
	}
	if (work_pid == 0){
		work_pid = t;
	}else{
		verify(t == work_pid);
	}
	cmgr_map[t]->mkcoroutine(f); 
}

void Coroutine::reg(coro_t::caller_type* cat){
	//verify(_c != NULL);
	pthread_t t = pthread_self();
//	Log_info("thread %x register coroutine %x", t, cat);
	verify(cmgr_map.find(t) != cmgr_map.end());
	cmgr_map[t]->reg(cat);
}

void Coroutine::reg(coro_t* c, coro_t::caller_type* cat){
	pthread_t t = pthread_self();
	verify(cmgr_map.find(t) != cmgr_map.end());
	cmgr_map[t]->reg(c, cat);
}

coro_t* Coroutine::get_c(){
	pthread_t t = pthread_self();
	verify(cmgr_map.find(t) != cmgr_map.end());
	return cmgr_map[t]->get_c();
}

coro_t::caller_type* Coroutine::get_ca(){
	pthread_t t = pthread_self();
	verify(cmgr_map.find(t) != cmgr_map.end());
	return cmgr_map[t]->get_ca();
}

void Coroutine::yeild(){
}

void Coroutine::yeildto(coro_t *ct){	
}

void Coroutine::wait(Event* ev){
	pthread_t t = pthread_self();
	verify(cmgr_map.find(t) != cmgr_map.end());
	cmgr_map[t]->wait(ev);
}

void Coroutine::recovery(){
	pthread_t t = pthread_self();
	if (cmgr_map.find(t) == cmgr_map.end()){
		Log_info("register Coroutine manager");
		reg_cmgr();
	}
	cmgr_map[t]->recovery();
}

void Coroutine::init(){
	pthread_mutex_init(&coro_lock, NULL);
	pthread_cond_init(&coro_cond, NULL);

	MyAllocator::init();
}

void CoroMgr::mkcoroutine(fp f){
//	_c = new coro_t(f, attr, stack_allocator, allo);
	coro_t c(f, attr, stack_allocator, allo);
//	Log_info("insert c: %x  ca: %x", _c, _ca);
//	c_set.push_back(_c);
//	reg(_c, _ca);

//	show_map();
//	wait_for_all_finished();
/*	int i=0;
	while ((*_c)){
		Log_info("yeild %d times", ++i);
		(*_c)();
	}  */
//	recovery();
//	show_map();
}

// tell CoroMgr the current coroutine caller
void CoroMgr::reg(coro_t::caller_type* ca){
	_ca = ca;	
}

// build the map between caller and callee
void CoroMgr::reg(coro_t* c, coro_t::caller_type* ca){ 
/*	std::map<coro_t*, coro_t::caller_type*>::iterator it;
	for (it=_map.begin(); it!=_map.end(); it++){
		Log_info("in map: %x => %x", it->first, it->second);
	}
*/
	if (callee_map.find(c) == callee_map.end()){
		callee_map[c] = ca;
	}
	verify(callee_map[c] == ca);

	if (caller_map.find(ca) == caller_map.end()){
		caller_map[ca] = c;
	}
	verify(caller_map[ca] == c);
}

coro_t* CoroMgr::get_c(){
	return _c;
}

coro_t::caller_type* CoroMgr::get_ca(){
	return _ca;
}

void CoroMgr::recovery(){ 
	for (int i=0; i<c_set.size(); ){
		if (!(*c_set[i])){
			Log_info("delete %x", c_set[i]);

			coro_t* c = c_set[i];
			coro_t::caller_type* ca = callee_map[c];

			callee_map.erase(c);
			caller_map.erase(ca);

			/*
			std::map<coro_t*, coro_t::caller_type*>::iterator it;
			for (it=_map.begin(); it!=_map.end(); it++){
				Log_info("in map: %x => %x", it->first, it->second);
			}*/			

			delete c;
			c_set.erase(c_set.begin() + i);
			continue;
		}else{
			Log_info("%x not finished", c_set[i]);
		}
		i++;
	} 
}

void CoroMgr::wait(Event* ev){
	if (ev->status() == Event::TRIGGER || 
		ev->status() == Event::CANCEL){
		//return;
	}
	
	wait_event.push_back(ev);
	Log_info("thread: %x, coroutine: %x, wait_event size: %d status: %d", pthread_self(), ev->ca, wait_event.size(), ev->status());
	Log_info("current coroutine: %x", ev->ca);
	in_coroutine = false;
	(*_ca)();
}

bool CoroMgr::search_all_trigger(){
	bool find = false;
	auto ite = wait_event.begin();
	for (; ite != wait_event.end(); ){
		Event* ev = (Event*)(*ite);
		if (ev->status() == Event::TRIGGER){
			wait_event.erase(ite);
			trigger_event.push_back(ev);
			find = true;
			continue;
		}
		ite++;
	}
	//Log_info("thread: %x, wait_event size: %d  find: %d", pthread_self(), wait_event.size(), find);
	return find; 
}

void CoroMgr::wait_for_all_finished(){
	//Log_info("search all trigger, wait_event size: %d, trigger_event size: %d", wait_event.size(), trigger_event.size()); 
    while(search_all_trigger() || trigger_event.size() > 0 
          || wait_event.size() > 0)
    {   
        Log_info("search all trigger, wait_event size: %d, trigger_event size: %d", wait_event.size(), trigger_event.size()); 
        while(trigger_event.size() > 0){
            Event* ev = trigger_event[0];
            coro_t* c = caller_map[ev->ca];
            reg(ev->ca);

            Log_info("resume coroutine: %x", ev->ca);
            (*c)();
            Log_info("resume coroutine: %x back", ev->ca);
            trigger_event.erase(trigger_event.begin());
     //       delete ev;
        }
    }
    //Log_info("all coroutine finished");
}

void CoroMgr::show_map(){
	Log_info("===========================");
	Log_info("======= caller map ========");
	auto key = caller_map.begin();
	for (; key!=caller_map.end(); key++){
		Log_info("%x --> %x", key->first, key->second);
	}
	Log_info("======= callee map ========");
	for (key=callee_map.begin(); key!=callee_map.end(); key++){
		Log_info("%x -> %x", key->first, key->second);
	}
	Log_info("===========================");
}

}