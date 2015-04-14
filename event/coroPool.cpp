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

	void CoroPool::release(caller_t* ca){
		coro_t* c = caller_map[ca];
		_pool[_pool_num++] = c;
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
				release(&ca);
				ca();
			}
		}
	}

	void CoroPool::init(int size){
		_pool_num = size;
		_pool = new coro_t*[size];
		for (int i=0; i<size; i++){
			coro_t* c = new coro_t(boost::bind(&CoroPool::main_loop, this, _1), 0);

			_all_coro.push_back(c);
			_c = c;
			reg();
			
			_pool[i] = c;
		}
	}
	void CoroPool::reg_function(fn* f){
		if (_pool_num < 1){
			Log_error("coroutine pool is empty");
			return;
		}
		coro_t* c = _pool[ --_pool_num ];		
		caller_t* ca = callee_map[c];
		reg_ca(ca);

		//Log_info("CoroPool num : %d  alloc ca: %x", _pool_num, _ca);
		(*c)(f);
	}
	void CoroPool::yeild(){
		coro_t* c = caller_map[_ca];
		//Log_info("coro: %x, caller: %x, yeild", c, _ca);
		(*_ca)();
	}
	void CoroPool::yeildto(caller_t* ca){
		coro_t* c = caller_map[ca];
		reg_ca(ca);
		
		//Log_info("coro: %x, caller: %x,resume coroutine", c, ca);
		(*c)(0);
	}

	void CoroPool::release(){
		for (auto &c: _all_coro){
			(*c)(0);
			delete c;
		}
	}
}