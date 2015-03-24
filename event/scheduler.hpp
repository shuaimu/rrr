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

typedef boost::coroutines::coroutine< void(void) > coro_t;
typedef boost::function< void(coro_t::caller_type&) > fp;
#define WAIT(x) rrr:EventMgr::wait(x)

namespace rrr{
class Event;

class Coroutine{
	static std::map< coro_t*, coro_t::caller_type* > _map;
public:
	static coro_t* _c;
	static coro_t::caller_type *_ca;

	static void mkcoroutine(fp f, coro_t* ct = NULL);
	static void init(coro_t::caller_type*);
	static void init(coro_t* c, coro_t::caller_type* ca);
	static coro_t* get_c();

	static void yeild();
	static void yeildto(coro_t* c);	
};

//coro_t* Coroutine::_c;
//coro_t::caller_type* Coroutine::_ca;
//std::map< coro_t*, coro_t::caller_type* > Coroutine::_map;

class EventMgr{
	static std::vector<Event* > wait_event;
	static std::vector<Event* > trigger_event;
public:
	static void wait(Event* ev);
	static bool search_all_trigger();
};

//std::vector<Event* > Scheduler::wait_event;
//std::vector<Event* > Scheduler::trigger_event;

} // namespace base

