#pragma once

#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "base/logging.hpp"
#include <unordered_map>

typedef boost::function<void(void)> fn;
typedef boost::coroutines::coroutine< void(fn*) > coro_t;
typedef coro_t::caller_type caller_t;

namespace rrr{
struct CoroPair{
	coro_t *c;
	caller_t* ca;
};
class CoroPool{
	void reg_ca(caller_t* ca);
	void reg();

	void release(CoroPair* cp);
	void main_loop(caller_t& ca);

	std::unordered_map<coro_t *, caller_t *> callee_map;
	std::unordered_map<caller_t *, coro_t *> caller_map;
	CoroPair** _pool;
	int _pool_num;

	std::vector<CoroPair*> _all_coro;
public:
	coro_t* _c;
	caller_t* _ca;

	CoroPair* current_pair;

	void init(int size=1000);
	void reg_function(fn* f);
	void yeild();

	void yeildto(CoroPair* ca);

	void release();
};

}