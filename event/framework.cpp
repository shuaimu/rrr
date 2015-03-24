/* this file is a pseudo-code, just to show how our framework will be constructed
 * and explain the relationship of three basic class: scheduler/task/event
 * */
typedef boost::coroutines::coroutine<void(void)> coro_t

class scheduler{
	vector<event*> waited_event;
	vector<event*> ready_event;

	vector<task*> _task;
	int epollfd;

	// run() is the main function of a scheduler. there will be two threads, 
 	// one checks epoll event for disk/net IO, one checks man-defined events
	void run(){
		pthread_create(&id, NULL, (void*) epoll_loop, NULL);

		while(true){
			if (ready_event.size() > 0){
				// choose one event ev from ready_event vector
				ev.resume();
			}
		}
	}	

	// epoll loop for check IO events
	void epoll_loop(){
		while(1){
			ndfs = epoll_wait(epollfd, events, MAX_EVENTS, -1);
			for (n=0; n<ndfs; n++){
				event* ev = events[ev].data.ptr;
				ev->ready();
			}
		}
	}
	void epolladd(int fd){
		epoll_event eevent;
		epoll_ctl(epollfd, EPOOL_CTL_ADD, fd, eevent);
	}

	// this function can be called by task to wait for a event.
	bool wait_event(event* ev){
		if ( /* ev has to wait */ ){
			waited_event.push_back(ev);
			if (/* this is a epoll event*/ ){
				epolladd(ev->fd);
			}
			return true;
		}else{ // ev does not have to wait; a bypass
			return false;
		}
	}
};

#define wait(ev) if(s->wait_event(ev)){ ca(); }

class task{
public:
	scheduler* s;
	void fn(coro_t::caller_t ca, void* p){
		coro_t coro = (coro_t*)p;

		event* ev = new event(this, p, fd);
		wait(ev)

		//do other thing
	}

	// every task first excute start() to create
	// a coroutine 
	void start(){
		coro_t* c;
		c = new coro_t(fn, c);
	}
};

class event{
	coro_t* coro;
	scheduler* sche;
	enum{
		wait, ready, trigger, cancel
	}status;
public:
	event(task* t, coro_t* c){
		this.coro = c;
		this.sche = t->s;
	}

	// recover coroutine binded with the event
	// we assume that an event is just associated with one coroutine
	void trigger(){
		(*coro)();
	}

	// if this is a manually defined event, trigger() has to be invoked mannully
	// if this is a basic IO event, trigger() will be invoked automatically
	void ready(){
		sche->waited_event.erase(this);
		sche->ready_event.push_back(this);
	}

	// cancel() is invoked when caller want to stop waiting for event;
	// and the corotinue of this event will be continue? or just killed?
	void cancel(){
		if (state == ready)
			sche->ready_event.erase(this);
		if (state == wait)
			sche->wait_event.erase(this);
	}
};

// net_event is a basic event we provided
// there may be many ways to distinguish net_event/ disk_event/ mannully defined event
class net_event: public event{
public:
	event(task*t, coro_t*c, int fd):supper(t, c){
		this.fd = fd;
		this.epollfd = t->s->epollfd;
	}
};
