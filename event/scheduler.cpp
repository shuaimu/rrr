#include "scheduler.hpp"
#include "../base/logging.hpp"
#include "event.hpp"
#include "fastStackAllocator.hpp"
#include <vector>

#ifdef COROUTINE
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

int Coroutine::pool_size;
pthread_key_t Coroutine::coroMgr_key;

CoroMgr* Coroutine::reg_cmgr(){
	pthread_t t = pthread_self();
	Log_info("thread %x register CoroMgr", t);
	//if (cmgr_map.find(t) != cmgr_map.end()){
	CoroMgr* cmgr = (CoroMgr*)pthread_getspecific(coroMgr_key);
	Log_info("cmgr %x", cmgr);
	if(cmgr != NULL){
		Log_error("current thread has been registered");
		//return cmgr_map[t];
		return cmgr;
	}else{
		cmgr = new CoroMgr();
		reg_cmgr(cmgr);
		return cmgr;
	}
}

int Coroutine::reg_cmgr(CoroMgr* cmgr){
	int ret = 0;
	//pthread_t t = pthread_self();
	pthread_mutex_lock(&coro_lock);
	/*if (cmgr_map.find(t) == cmgr_map.end()){
		cmgr_map[t] = cmgr;
	}else if (cmgr_map[t] != cmgr){
		delete cmgr_map[t];
		cmgr_map[t] = cmgr;
		ret = 1;
	}*/
	CoroMgr* c = (CoroMgr*)pthread_getspecific(coroMgr_key);
	if (c != NULL){
		delete c;
		ret = 1;
	}
	pthread_setspecific(coroMgr_key, cmgr);

	pthread_mutex_unlock(&coro_lock);
	return ret;
}

CoroMgr* Coroutine::get_cmgr(){
	pthread_t pid = pthread_self();
	if (cmgr_map.find(pid) == cmgr_map.end()){
		reg_cmgr();
	}
	return cmgr_map[pid];
}

CoroMgr* Coroutine::get_current_cmgr(){
	CoroMgr* cmgr = (CoroMgr*)pthread_getspecific(coroMgr_key);
	if (cmgr == NULL){
		cmgr = reg_cmgr();
	}
	return cmgr;
}

void Coroutine::mkcoroutine(fp f){
#ifdef COROUTINE_COUNT
	coro_count += 1;
#endif	
	(get_current_cmgr())->mkcoroutine(f);
}

CoroPair* Coroutine::get_cp(pthread_t t){
	if (t == 0){
		return (get_current_cmgr())->get_cp();
	}
	else{
		Log_error("try to get CoroMgr in another thread");
		return NULL;
	}
}

void Coroutine::wait(Event* ev){
	(get_current_cmgr())->wait(ev);
}

void Coroutine::recovery(){
	(get_current_cmgr())->recovery();
}

void Coroutine::init(int size){
	pthread_mutex_init(&coro_lock, NULL);
	pthread_cond_init(&coro_cond, NULL);
	
	pool_size = size;
	pthread_key_create(&coroMgr_key, NULL);
	Log_info("coroMgr_key %s", pthread_getspecific(coroMgr_key) );
//	MyAllocator::init();
}

#ifdef COROUTINE_COUNT

void Coroutine::report(){
	Log_info("coroutine count: %d", coro_count);
	Log_info("coroutine switch: %d", switch_count);
}
#endif

void CoroMgr::mkcoroutine(fp f){
	_pool.reg_function(f);
}

CoroMgr::CoroMgr(){
	Log_info("pool size: %d", Coroutine::pool_size);
	_pool.init(Coroutine::pool_size);
}

CoroPair* CoroMgr::get_cp(){
	return _pool.current_pair;
}
void CoroMgr::recovery(){ 
}

void CoroMgr::wait(Event* ev){
	if (ev->status() == Event::TRIGGER || 
		ev->status() == Event::CANCEL){
		return;
	}
	if (ev->status() == Event::READY){
		ev->trigger();
		return;
	}
	assert(ev->status() == Event::IDLE);

#ifdef COROUTINE_COUNT
	switch_count += 1;
#endif

	ev->wait();
	wait_event.push_back(ev);
	_pool.yeild();
}

bool CoroMgr::search_all_trigger(){
	bool find = false;
	auto ite = wait_event.begin();
	for (; ite != wait_event.end(); ){
		Event* ev = (Event*)(*ite);
		if (ev->status() == Event::READY){
			wait_event.erase(ite);
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
	ready_event.push_back(ev);
}

void CoroMgr::resume_triggered_event(){
    while(search_all_trigger())
    {   
        while(ready_event.size() > 0){
        	int t = get_next();
            Event* ev = ready_event[t];
            ev->trigger();            
            _pool.yeildto(ev->cp);
            ready_event.erase(ready_event.begin() + t);
        }
    }
}

}

#endif
