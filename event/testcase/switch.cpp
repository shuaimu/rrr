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

typedef boost::coroutines::coroutine<void(void)> coro_t;

#define REF(x) boost::ref(x)

bool finish = false;

void A(coro_t &ca){
	while(!finish){
		ca();
	}
}

int main(int argc, char** argv){
	if (argc < 1){
		exit(0);
	}
	int t = atoi(argv[1]);
	long begin = clock();

	coro_t c(A);
	for (int i=0; i<t-1; i++){
		c();
	}
	finish = true;
	c();
	
	long end = clock();
	cout << "time: " << end - begin << endl;
}