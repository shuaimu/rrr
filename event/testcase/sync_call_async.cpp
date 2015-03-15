/*************************************************************************
 > File Name: sync_call_async.cpp
 > Author: Mengxing Liu
 > Created Time: 2015-03-15
 ************************************************************************/

/* this file is just a pseduo-like code to explain how a 
   sync function call an aysnc function.
   when we finished our framework, this testcase should be rewrite
   */

typedef boost::coroutines::coroutine< void()> coro_t;

void async(coro_t::caller_type &ca, event* ev){
	// do something 
	
	// yield and return to the caller
	ca();

	// do something else
	
	// trigger the event to remind the caller the job has been finished
	ev->ready();
}
void sync(coro_t::caller_type &ca){
	coro_t *c;

	event* ev = new event();
	c = new coro_t(boost::bind(async, _1, ev));

	if(wait(ev)){
		ca();
	}
}

int main(){
	coro_t task(sync);
}
