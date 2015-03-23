/*************************************************************************
 > File Name: schedular.hpp
 > Author: Mengxing Liu
*************************************************************************/
#pragma once

#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

typedef boost::coroutines::coroutine< void(void) > coro_t;
typedef boost::function< void(coro_t::caller_type&) > fp;

namespace rrr{

class Coroutine{
public:
	static coro_t* c;
	static coro_t::caller_type *ca;

	static void mk_coro(fp f, coro_t* ct = NULL);

	static coro_t* get_cur_coro();

	static void set_ca(coro_t::caller_type* cat);

	static coro_t::caller_type* get_cur_ca();
};

} // namespace base

