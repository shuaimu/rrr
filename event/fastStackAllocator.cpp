#include "fastStackAllocator.hpp"
#include "base/logging.hpp"

namespace rrr{

std::vector<void*> MyAllocator::free_buf;

void MyAllocator::init(){
	for (int i=0; i < PRE_ALLOC_NUM; i++){
		void *limit = malloc(STACK_SIZE);
		free_buf.push_back(limit);
	}
}

void MyAllocator::allocate( boost::coroutines::stack_context & ctx, std::size_t size){
	if (size > STACK_SIZE){
		Log_error("size is too large to allocate");
		return;
	}
	void *limit;
	int s = free_buf.size();
	if (s == 0){
		Log_warn("preallocated stacks are used up");
		limit = malloc(STACK_SIZE);
	}else{
		limit = free_buf[s-1];
		free_buf.erase(free_buf.end()-1);
	}
	// memset(limit, '\0', size);
	ctx.size = size;
	ctx.sp = static_cast<char *>(limit) + ctx.size;
}

void MyAllocator::deallocate(boost::coroutines::stack_context &ctx){
	void * limit = static_cast< char * >( ctx.sp) - ctx.size;
	free_buf.push_back(limit);
}

}

