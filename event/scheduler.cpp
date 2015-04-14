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

#ifdef COROUTINE_COUNT
int coro_count = 0;
int switch_count = 0;
#endif

CoroMgr* Coroutine::reg_cmgr(pthread_t t){
	Log_info("thread %x register CoroMgr");
	if (cmgr_map.find(t) != cmgr_map.end()){
		Log_error("current thread has been registered");
		return cmgr_map[t];
	}else{
		pthread_mutex_lock(&coro_lock);
		cmgr_map[t] = new CoroMgr();
		pthread_mutex_unlock(&coro_lock);
		return cmgr_map[t];
	}
}
CoroMgr* Coroutine::reg_cmgr(){
	pthread_t t = pthread_self();
	return reg_cmgr(t);
}

int Coroutine::reg_cmgr(pthread_t t, CoroMgr* cmgr){
	int ret = 0;
	if (cmgr_map.find(t) == cmgr_map.end()){
		cmgr_map[t] = cmgr;
	}else if (cmgr_map[t] != cmgr){
		delete cmgr_map[t];
		cmgr_map[t] = cmgr;
		ret = 1;
	}
	return ret;
}

CoroMgr* Coroutine::get_cmgr(pthread_t pid){
	if (cmgr_map.find(pid) == cmgr_map.end()){
		reg_cmgr(pid);
	}
	return cmgr_map[pid];
}

void Coroutine::mkcoroutine(fp f){
#ifdef COROUTINE_COUNT
	coro_count += 1;
#endif	
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

#ifdef COROUTINE_COUNT

void Coroutine::report(){
	Log_info("coroutine count: %d", coro_count);
	Log_info("coroutine switch: %d", switch_count);
}
#endif

void CoroMgr::mkcoroutine(fp f){
	_pool.reg_function(f);
//	_c = new coro_t(f, attr, stack_allocator, allo);

//	c_set.push_back(_c);
/*	if (!(*_c)){
		delete _c;
	}else{
	//	Log_info("reg callee: %x caller: %x", _c, _ca);
		reg(_c, _ca);	
	} */
}

// tell CoroMgr the current coroutine caller
void CoroMgr::reg(coro_t::caller_type* ca){
	//_ca = ca;	
}

// build the map between caller and callee
void CoroMgr::reg(coro_t* c, coro_t::caller_type* ca){ 
/*	if (callee_map.find(c) == callee_map.end()){
		callee_map[c] = ca;
	}
	verify(callee_map[c] == ca);

	if (caller_map.find(ca) == caller_map.end()){
		caller_map[ca] = c;
	}
	verify(caller_map[ca] == c); */
}

coro_t* CoroMgr::get_c(){
	return _pool._c;
}

coro_t::caller_type* CoroMgr::get_ca(){
	return _pool._ca;
}

void CoroMgr::recovery(){ 
}

void CoroMgr::wait(Event* ev){
	if (ev->status() == Event::TRIGGER || 
		ev->status() == Event::CANCEL){
		return;
	}

#ifdef COROUTINE_COUNT
	switch_count += 1;
#endif

	wait_event.push_back(ev);
	//Log_info("thread: %x, coroutine: %x, wait_event size: %d status: %d ev: %x", pthread_self(), ev->ca, wait_event.size(), ev->status(), ev);
	//in_coroutine = false;

	(*_pool._ca)();
}

bool CoroMgr::search_all_trigger(){
	bool find = false;
	auto ite = wait_event.begin();
	for (; ite != wait_event.end(); ){
		Event* ev = (Event*)(*ite);
		if (ev->status() == Event::TRIGGER){
			wait_event.erase(ite);
			//trigger_event.push_back(ev);
			insert_trigger(ev);
			find = true;
			continue;
		}
		ite++;
	}
	//Log_info("thread: %x, wait_event size: %d  find: %d", pthread_self(), wait_event.size(), find);
	return find; 
}

int CoroMgr::get_next(){
	return 0;
}

void CoroMgr::insert_trigger(Event* ev){
	trigger_event.push_back(ev);
}

void CoroMgr::resume_triggered_event(){
//	Log_info("search all trigger, wait_event size: %d, trigger_event size: %d", wait_event.size(), trigger_event.size()); 
    while(search_all_trigger())
    {   
//        Log_info("search all trigger, wait_event size: %d, trigger_event size: %d", wait_event.size(), trigger_event.size()); 
        while(trigger_event.size() > 0){
        	int t = get_next();
            Event* ev = trigger_event[t];
            
            coro_t::caller_type* ca = ev->ca;
/*            coro_t* c = caller_map[ca];

            reg(ca);

        //    Log_info("resume coroutine: %x", ca);
            (*c)();
            if (!(*c)){
            	delete c;
            	callee_map.erase(c);
            	caller_map.erase(ca);
            }
        //    Log_info("resume coroutine: %x back", ca);
        */
            _pool.yeildto(ca);
            trigger_event.erase(trigger_event.begin() + t);
        }
    }
    //Log_info("all coroutine finished");
}

}