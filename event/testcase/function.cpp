/*************************************************************************
    > File Name: function.cpp
    > Author: lmx
    > Mail: liumengxing2010@qq.com 
    > Created Time: Sat Apr 11 17:48:43 2015
 ************************************************************************/

#include<iostream>
#include"event/event.hpp"

using namespace std;


int func(){
	
}

void coro(){
//	REG_CORO;
}

int main(int argc, char** argv){
	char mod = argv[1][0];
	int circle = atoi(argv[2]);

#ifdef COROUTINE
	rrr::Coroutine::init();
#endif
    auto f = [=](){

	};
	long begin = clock();
	switch (mod){
		case '1':
			for (int i=0; i<circle; i++){
				func();
			}
			break;
		case '2':
			for (int i=0; i<circle; i++){
				f();
			}
			break;
		case '3':
			for (int i=0; i<circle; i++){
#ifdef COROUTINE
				auto f = boost::function<void(void)>(coro);
				rrr::Coroutine::mkcoroutine(&f);
#else
				coro();
#endif
			}
			break;
		default:
			cout << "wrong" << endl;
	}
	cout << "time: " << (clock()-begin)/1000 << endl;
}



