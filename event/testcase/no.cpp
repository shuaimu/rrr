/*************************************************************************
 > File Name: no.cpp
 > Author: Mengxing Liu
 > Created Time: 2015-03-25
 ************************************************************************/

#include <iostream>
#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>

typedef boost::coroutines::coroutine<void(void)> coro_t;

void test(coro_t::caller_type& ca, int i){

}

int main(int argc, char** argv){
	coro_t* c = new coro_t(boost::bind(&test, _1, 1));
	coro_t c2(boost::bind(&test, _1, 1));

	std::cout << !(*c) << std::endl;
	std::cout << (*c) << std::endl;
	std::cout << c2 << std::endl;
}
