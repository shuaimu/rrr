#include <iostream>
#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>

typedef boost::coroutines::coroutine<void(void)> coro_t;

void test(coro_t::caller_type& ca, int i){
}

int main(int argc, char** argv){
	if (argc < 1){
		exit(0);
	}
	int n = atoi(argv[1]);
	coro_t** c = new coro_t*[n];

	std::cout << n << " coroutines will be created" << std::endl;
	std::cout << "size of single coroutine object: " << sizeof(coro_t) << std::endl;
	
	std::vector<coro_t*> v;
	for (int s=0; s<n/3000; s++){

		for (int i=0; i<3000; i++){
			c[i] = new coro_t(boost::bind(&test, _1, 1));
			v.push_back(c[i]);
			// delete c[i];
		}

		for (int i=0; i<3000; i++){
			coro_t* t = c[i];
			delete t;
		}
	}

}
