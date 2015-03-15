/*************************************************************************
 > File Name: test.cpp
 > Author: Mengxing Liu
 > Created Time: 2015-03-11
 ************************************************************************/

#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>

typedef boost::coroutines::coroutine<void(void)> coro_t;


class task;

coro_t* c;
class task{
	int closure;
public:
	task():closure(123){

	}

	static void fn(coro_t::caller_type& ca, task* task, coro_t* c){
		ca();
		std::cout << "local print 1" << std::endl;
		ca();
		std::cout << "local print 2" << std::endl;
		ca();
		std::cout << "local print 3" << std::endl;
	}

	coro_t* start(){
		coro_t *c;
		c = new coro_t(boost::bind(task::fn, _1, this, c));
		return c;
	}
};

void run1(){
	std::cout << "main print 1" << std::endl;
	(*c)();
	std::cout << "main print 2" << std::endl;
}

void run2(){
	std::cout << "main print 3" << std::endl;
	(*c)();
	std::cout << "main print 4" << std::endl;
}

int main(){
	task t;
	c = t.start();

	run1();
	run2();

	std::cout << "main print 5" << std::endl;
	(*c)();
}

