/*************************************************************************
 > File Name: test.cpp
 > Author: Mengxing Liu
 > Created Time: 2015-03-11
 ************************************************************************/

#include <iostream>
#include <pthread.h>

#include "dballevent.hpp"
#include "rrr.hpp"

using namespace rrr;

void async(){
	DballEvent* ev = new DballEvent(Coroutine::get_ca());
	ev->add();

	WAIT(ev);
}

void hello(int i, std::function<void(int, int)> f){
	Log_info("hello %d start", i);
	f(i, 1);
	f(i, 2);
	Log_info("hello %d end", i);
}

void hello_coro(coro_t::caller_type& ca, int i, std::function<void (int, int)> f){
	REG_CORO;
	Log_info("hello %d start", i);
	async();
	f(i, 1);
	async();
	f(i, 2);
	async();
	Log_info("hello %d end", i);
}

void container(){
	pthread_t pid = pthread_self();
	Coroutine::reg_cmgr(pid);
	CoroMgr* cmgr = Coroutine::get_cmgr(pid);

	int *s = new int;
	auto reply = [=](int i, int j){
		Log_info("hello %d yeild %d", i, j);
		*s = 1;
	};

	for (int i=0; i<5; i++){
		Log_info("========= new  Coroutine ========= ");
		Coroutine::mkcoroutine(boost::bind(&hello_coro, _1, i, reply));
		//hello(i, reply);
	}

	cmgr->wait_for_all_finished();
 	cmgr->recovery();
}
int main(){
	Coroutine::init();	

	container();
}
