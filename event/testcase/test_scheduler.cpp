#include "rrr.hpp"
#include "event/event.hpp"
#include <stdlib.h>
#include <vector>

using namespace rrr;

std::vector<Event*> v;

class PriorityEvent: public Event{
public:
	int _p;
	PriorityEvent(coro_t::caller_type* ca, int p):Event(ca), _p(p){
	}
};

class Scheduler: public CoroMgr{
public:
	void insert_trigger(Event* _e){
		PriorityEvent* ev = (PriorityEvent*)_e;
		int t = trigger_event.size();
		for (int i=0; i<trigger_event.size(); i++){
			PriorityEvent *tev = (PriorityEvent*)trigger_event[i];
			if ( tev->_p > ev->_p){
				t = i;
				break;
			}
		}
		trigger_event.insert(trigger_event.begin() + t, ev);
	}
	int get_next(){
		return 0;
	}
};

void yeild(int i){
	PriorityEvent* ev = new PriorityEvent(Coroutine::get_ca(), i);
	v.push_back(ev);
	WAIT(ev);
	delete ev;
}

coro_f(test, int i, std::function<void(int, int)> f){
	REG_CORO;
	f(i, 0);
	yeild(i);
	f(i, 1);
}

void container(){
	pthread_t pid = pthread_self();
	CoroMgr* cmgr = new Scheduler();
	Coroutine::reg_cmgr(pid, cmgr);

	int *s = new int;
	auto reply = [=](int i, int j){
		Log_info("hello %d yeild %d", i, j);
		*s = 1;
	};

	int rank[10] = {3, 5, 2, 1, 4, 7, 9, 8, 6, 10};
	for (int i=0; i<10; i++){
		Log_info("========= new  Coroutine ========= ");

		Coroutine::mkcoroutine(boost::bind(&test, _1, rank[i], reply));
		//hello(i, reply);
	}

	for (auto ev: v){
		ev->trigger();
	}
	cmgr->resume_triggered_event();
}

int main(){
	Coroutine::init();
	container();
}