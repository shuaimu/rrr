/*************************************************************************
 > File Name: test.cpp
 > Author: Mengxing Liu
 > Created Time: 2015-03-23
 ************************************************************************/

#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>

typedef boost::coroutines::coroutine<void(void)> coro_t;

#define coro_f(x, ...) void x(coro_t::caller_type& ca, __VA_ARGS__)

class task{
	int closure;
public:
	task():closure(123){

	}

	coro_f(func, coro_t* c){
		ca();
		std::cout << closure << std::endl;
	}

	coro_t* start(){
		coro_t *c;
		c = new coro_t( boost::bind(&task::func, this, _1, c));
		return c;
	}
};

coro_t* getc(){
	task t;
	return t.start();
}
int main(){
	coro_t* c = getc();
	(*c)();
}
