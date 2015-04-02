#pragma once

#include <boost/coroutine/coroutine.hpp>
#include <boost/coroutine/stack_context.hpp>
#include <vector>

typedef boost::coroutines::coroutine<void(void)> coro_t;

#define STACK_SIZE 128*1024
#define PRE_ALLOC_NUM 2048

namespace rrr{

class MyAllocator: public boost::coroutines::stack_allocator{
public:
	static std::vector<void*> free_buf;
	static void init();
	
	void allocate( boost::coroutines::stack_context & ctx, std::size_t size);

	void deallocate(boost::coroutines::stack_context &ctx);	
};

}