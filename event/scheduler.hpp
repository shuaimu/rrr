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

#define coro_f(x, ...) void x(coro_t::caller_type& ca, __VA_ARGS__)
#define WAIT(x) rrr:EventMgr::wait(x)

namespace rrr{
class Event;

class CoroMgr{
	std::vector<coro_t*> c_set;
	std::vector<Event* > wait_event;
	std::vector<Event* > trigger_event;
	std::map<coro_t*, coro_t::caller_type*> _map;

	coro_t* _c;
	coro_t::caller_type* _ca;
public:
	void mkcoroutine(fp f);
	void reg(coro_t::caller_type*);
	void reg(coro_t*, coro_t::caller_type*);
	coro_t* get_c();

	void recovery();
	
	void wait(Event*);
	bool search_all_trigger();
};	


class Coroutine{
public:
	static std::map<pthread_t, CoroMgr*> cmgr_map;
	static void reg_cmgr();
	static void mkcoroutine(fp f);

	static void reg(coro_t::caller_type*);
	static void reg(coro_t* c, coro_t::caller_type* ca);
	static coro_t* get_c();

	// do we need interface of yeild? or just wait
	static void yeild();
	static void wait(Event*);

	static void yeildto(coro_t* c);	

	static void recovery();
	static void init();
};

} // namespace base

