/*************************************************************************
 > File Name: switch.cpp
 > Author: Mengxing Liu
 > Created Time: 2015-03-30
 ************************************************************************/

#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <time.h>

using namespace std;

#ifdef FP
typedef boost::coroutines::coroutine<int*(int *)> coro_t;
#else
typedef boost::coroutines::coroutine<void(void)> coro_t;
#endif

#define REF(x) boost::ref(x)

bool finish = false;

void A(coro_t::caller_type &ca){
	while(!finish){
#ifdef FP
		int *p = ca.get();
		ca(p);
#else
		ca();
#endif
	}
}

int main(int argc, char** argv){
	if (argc < 1){
		exit(0);
	}
	int t = atoi(argv[1]);
	long begin = clock();
	
#ifdef FP
	int *p = new int;
	coro_t c(A, p);
	int *s = c.get();
#else
	coro_t c(A);
#endif
	for (int i=0; i<t-1; i++){
#ifdef FP
		int* s = c(p).get();
#else
		c();
#endif
	}	
	finish = true;
#ifdef FP
	c(p);
#else
	c();
#endif
	
	long end = clock();
	cout << "time: " << end - begin << endl;
}
