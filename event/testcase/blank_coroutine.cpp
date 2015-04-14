/*************************************************************************
 > File Name: blank_coroutine.cpp
 > Author: Mengxing Liu
 > Created Time: 2015-03-30
 ************************************************************************/

#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <time.h>
#include <vector>

using namespace std;

typedef boost::coroutines::coroutine<void(void)> coro_t;

#define REF(x) boost::ref(x)

#define SIZE 1024*128
#define PRE_ALLOC_SIZE 500

int s;

vector<void* > free_buf;

void init(){
	for (int i=0; i < PRE_ALLOC_SIZE; i++){
		void *limit = malloc(SIZE);
		memset(limit, '\0', SIZE);
		free_buf.push_back(limit);
	}
}

void A(coro_t &ca){
	for (int i=0; i<s; i++)
		ca();
}


class MyAllocator: public boost::coroutines::stack_allocator{
public:
	void allocate( boost::coroutines::stack_context & ctx, std::size_t size){
		if (size > SIZE){
			cout << "size is too big: " << size << endl;
			return;
		}
		void *limit;
		int s = free_buf.size();
		if (s == 0){
			cout << "free_buf is used up" << endl;
			limit = malloc(SIZE);
		}else{
			limit = free_buf[s-1];
			free_buf.erase(free_buf.end()-1);
		}
		// memset(limit, '\0', size);
		ctx.size = size;
		ctx.sp = static_cast<char *>(limit) + ctx.size;
	}

	void deallocate(boost::coroutines::stack_context &ctx){
		void * limit = static_cast< char * >( ctx.sp) - ctx.size;
		free_buf.push_back(limit);
	}	
};

int main(int argc, char** argv){
	if (argc < 1){
		exit(0);
	}
	init();

	int t = atoi(argv[1]);
	s = atoi(argv[2]);

	MyAllocator stack_allocator;
	boost::coroutines::attributes attr;
	allocator<coro_t> allo;

	long begin = clock();
	for (int i=0; i<t; i++){
		coro_t *c[500];
		for (int j=0; j<500; j++){
			c[j] = new coro_t(A, attr, stack_allocator, allo);
		}for (int j=0; j<500; j++){
			for (int g=0; g<s; g++)
				(*c[j])();
			delete c[j];
		}
	}
	long end = clock();
	cout << "time: " << end - begin << endl;
}
