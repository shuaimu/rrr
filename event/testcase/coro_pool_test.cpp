#include <iostream>
#include <time.h>
#include "event/coroPool.hpp"

using namespace std;

rrr::CoroPool pool;

void test(int i){
}

void main_loop(caller_t& ca){
	ca.get();
	ca();
	
	while(true){
		auto f = ca.get();
		if (f == NULL){
			break;
		}else{
			(*f)();
			ca();
		}
	}
}

int main(int argc, char**  argv){
	long t0 = clock();
	pool.init(1000);
	int t = atoi(argv[1]);

	coro_t c(main_loop, 0);

	long t1 = clock();
	for (int i=0; i<t; i++){
		boost::function<void (void)> f = boost::bind(&test, 1);
//		c(&f);
		pool.reg_function(&f);
	}
	long t2 = clock();

	c(0);
	cout << "init time: " << t1 - t0 << endl;
	cout << "run time: " << t2 - t1 << endl;
	pool.release();
}
