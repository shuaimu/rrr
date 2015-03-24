/*************************************************************************
 > File Name: test.cpp
 > Author: Mengxing Liu
 > Created Time: 2015-03-11
 ************************************************************************/

#include <iostream>

#include "event.hpp"

coro_f(hello, int a){
	std::cout << a << std::endl;
	std::cout << "coroutine" << std::endl;
}
int main(){
	rrr::Coroutine::mkcoroutine(boost::bind(&hello, _1, 1));
}
