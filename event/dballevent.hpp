#include "event.hpp"
#include "base/debugging.hpp"
#include "base/logging.hpp"

namespace rrr{

class DballEvent:public Event{
public:
    int size;
    int count;
    DballEvent(coro_t::caller_type* c, int s=1): Event(c), size(s), count(0){
    }
    void add(){
        count++; 
        verify(count <= size);
        if (count == size){
            trigger();
        }
        Log_info("lock event add trigger, count: %d, size: %d, status: %d", count, size, _status);
    }
};

}