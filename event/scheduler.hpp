/*************************************************************************
 > File Name: scheduler.hpp
 > Author: Mengxing Liu
 > Time: 2015-03-23
*************************************************************************/
#pragma once

#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <map>
#include <set>
#include <pthread.h>

typedef boost::coroutines::coroutine< void(void) > coro_t;
typedef boost::function< void(coro_t::caller_type&) > fp;

#define COROUTINE
#define COROUTINE_COUNT

#ifdef COROUTINE
	#define coro_f(x, ...) void x(coro_t::caller_type& ca, __VA_ARGS__)
	#define REG_CORO rrr::Coroutine::reg(&ca)
#else
	#define coro_f(x, ...) void x(__VA_ARGS__) 
	#define REG_CORO {}
#endif

#define WAIT(x) rrr::Coroutine::wait(x)
#define REF(x) boost::ref(x)

namespace rrr{
class Event;

class CoroMgr{
	std::vector<coro_t*> c_set;
	coro_t* _c;
	coro_t::caller_type* _ca;
public:
	std::map<coro_t*, coro_t::caller_type*> callee_map;
	std::map<coro_t::caller_type*, coro_t*> caller_map;
	
	std::vector<Event* > trigger_event;
	std::vector<Event* > wait_event;

	void mkcoroutine(fp f);
	void reg(coro_t::caller_type*);
	void reg(coro_t*, coro_t::caller_type*);
	
	coro_t* get_c();
	coro_t::caller_type* get_ca();

	void recovery();
	
	void wait(Event*);
	bool search_all_trigger();

	void resume_triggered_event();

	void show_map();

	virtual int get_next();
	virtual void insert_trigger(Event*);
};	


class Coroutine{
public:
	static std::map<pthread_t, CoroMgr*> cmgr_map;
	static CoroMgr* reg_cmgr();
	static CoroMgr* reg_cmgr(pthread_t pid);
	static int reg_cmgr(pthread_t pid, CoroMgr* cmgr);

	static CoroMgr* get_cmgr(pthread_t pid);	
	
	static void mkcoroutine(fp f);
	static void reg(coro_t::caller_type*);
	static void reg(coro_t* c, coro_t::caller_type* ca);
	
	static coro_t* get_c();
	static coro_t::caller_type* get_ca();

	// do we need interface of yeild? or just wait
	static void yeild();
	static void wait(Event*);

	static void yeildto(coro_t* c);	

	static void recovery();
	static void init();
#ifdef COROUTINE_COUNT
	static void report();
#endif
};
} // namespace base

