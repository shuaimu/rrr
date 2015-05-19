#ifdef COROUTINE

#include "coroPool.hpp"

namespace rrr{
	void CoroPool::reg_ca(caller_t* ca){
		_ca = ca;
	}
	void CoroPool::reg(){
		coro_t* c = _c;
		caller_t* ca = _ca;

		if (callee_map.find(c) == callee_map.end()){
			callee_map[c] = ca;
		}
		verify(callee_map[c] == ca);

		if (caller_map.find(ca) == caller_map.end()){
			caller_map[ca] = c;
		}
		verify(caller_map[ca] == c);
	}

	void CoroPool::release(CoroPair* cp){
		_pool[_pool_num++] = cp;
		//Log_info("release ca: %x _pool_num: %d", ca, _pool_num);
	}

	void CoroPool::main_loop(caller_t& ca){
		reg_ca(&ca);
		ca.get();

		ca();
		
		while(true){
			auto f = ca.get();
			if (f == NULL){
				break;
			}else{
				(*f)();
				release(current_pair);
				ca();
			}
		}
	}

	void CoroPool::init(int size){
		_pool_num = size;
		_pool = new CoroPair*[size];
		for (int i=0; i<size; i++){
			coro_t* c = new coro_t(boost::bind(&CoroPool::main_loop, this, _1), 0);

			_pool[i] = new CoroPair;
			_pool[i]->c = c; _pool[i]->ca = _ca;
			current_pair = _pool[i];

			_all_coro.push_back(current_pair);
		}
	}
	void CoroPool::reg_function(fn* f){
		if (_pool_num < 1){
		//	Log_error("coroutine pool is empty");
			return;
		}
		coro_t* c = _pool[ --_pool_num]->c;
		current_pair = _pool[_pool_num];

		//Log_info("CoroPool num : %d  alloc ca: %x", _pool_num, _ca);
		(*c)(f);
	}
	void CoroPool::yeild(){
		//Log_info("coro: %x, caller: %x, yeild", c, _ca);
		(*(current_pair->ca))();
	}
	void CoroPool::yeildto(CoroPair* cp){
		current_pair = cp;
		//Log_info("coro: %x, caller: %x,resume coroutine", c, ca);
		(*(cp->c))(0);
	}

	void CoroPool::release(){
		for (auto &cp: _all_coro){
			(*(cp->c))(0);
			delete cp;
		}
	}
}

#endif
