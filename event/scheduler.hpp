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

#include "coroPool.hpp"

//typedef boost::coroutines::coroutine< void(void) > coro_t;
//typedef boost::function< void(coro_t::caller_type&) > fp;
typedef boost::function< void(void) >* fp;
typedef boost::function< void(void) > fn;
//#define COROUTINE
//#define COROUTINE_COUNT

#ifdef COROUTINE
	#define coro_f(x, ...) void x(__VA_ARGS__) 
	#define REG_CORO {} //rrr::Coroutine::reg(&ca)
#else
	#define coro_f(x, ...) void x(__VA_ARGS__) 
	#define REG_CORO {}
#endif

#define WAIT(x) rrr::Coroutine::wait(x)
#define REF(x) boost::ref(x)
#define GET_CA()  rrr::Coroutine::get_ca()
#define GET_CP()  rrr::Coroutine::get_cp()
#ifdef COROUTINE

namespace rrr{
class Event;
class Coroutine;

class CoroMgr{
	//coro_t* _c;
	//coro_t::caller_type* _ca;
public:
//	std::map<coro_t*, coro_t::caller_type*> callee_map;
//	std::map<coro_t::caller_type*, coro_t*> caller_map;
	
	std::vector<Event* > ready_event;
	std::vector<Event* > wait_event;

	CoroPool _pool;

	CoroMgr();
	~CoroMgr(){
		_pool.release();
	}

	void mkcoroutine(fp f);
	
	CoroPair* get_cp();

	void recovery();
	
	void wait(Event*);
	bool search_all_trigger();

	void resume_triggered_event();

	virtual int get_next();
	virtual void insert_trigger(Event*);
};	


class Coroutine{;
public:
	static int pool_size;
	static pthread_key_t coroMgr_key;

	static std::map<pthread_t, CoroMgr*> cmgr_map;
	static CoroMgr* reg_cmgr();
	static int reg_cmgr(CoroMgr* cmgr);

	static CoroMgr* get_cmgr();	
	static CoroMgr* get_current_cmgr();
	
	static void mkcoroutine(fp f);
	static CoroPair* get_cp(pthread_t t=0);

	static void wait(Event*);

	static void recovery();
	static void init(int size = 1000);
#ifdef COROUTINE_COUNT
	static void report();
#endif
};
} // namespace base

#endif
