#pragma once
#include "event.hpp"

#ifdef COROUTINE

#include "base/debugging.hpp"
#include "base/logging.hpp"

namespace rrr{

class DballEvent:public Event{
public:
    int size;
    int count;
    DballEvent(coro_t::caller_type* c, int s=1): Event(c), size(s), count(0){
    //    Log_info("create dball: %x", this);
    }
    void add(){
        count++; 
       // Log_info("%x lock event add trigger, count: %d, size: %d, cur_ca: %x", this, count, size, Coroutine::get_ca());
        verify(count <= size);
        if (count == size){
            trigger();
        }
    }
};

}

#endif