/*
 *
 * Here is how it works, there is a queue, a flush thread, and a callback thread.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <chrono>

#include "base/logging.hpp"
#include "base/debugging.hpp"

#include "stat.hpp"
#include "recorder.hpp"

namespace rrr {

Recorder::Recorder(const char *path) {
    Log::debug("disk log into %s", path);

    fd_ = open(path, O_RDWR | O_CREAT, 0644);
    if (errno == EINVAL) {
	Log::error("Open record file failed, are"
		   " yo2u trying to write into a tmpfs?");
	fd_ = open(path, O_RDWR | O_CREAT, 0644);
    }
    if (fd_ <= 0) {
	Log::error("Open record file failed, errno:"
		   " %d, %s", errno, strerror(errno));
	verify(fd_ > 0);
    }

    flush_reqs_ = new std::list<io_req_t*>();
    callback_reqs_ = new std::list<io_req_t*>();
#ifdef COROUTINE
    ev_flush_reqs_ = new std::list<ev_req_t*>();
    ev_callback_reqs_ = new std::list<ev_req_t*>();
#endif

    th_flush_ = new std::thread(&Recorder::flush_loop, this);

    timer_.start();

//    th_flush_ = new std::thread([this] () {
//	    this->flush_loop();
//	});
    //    th_pool_ = new base::ThreadPool(1);
}

void Recorder::flush_loop() {
    while (true) {
        mtx_cd_flush_.lock();

        auto now = std::chrono::system_clock::now();
        cd_flush_.wait(mtx_cd_flush_);
        flush_buf();
        mtx_cd_flush_.unlock();
    }
}

//void Recorder::submit(const std::string &buf) {
//    std::function<void(void)> empty_func;
//    submit(buf, empty_func);
//}
#ifdef COROUTINE
void Recorder::submit_ev(const std::string &buf, Event* cb) {
    ev_req_t *req = new ev_req_t(buf, cb);
    ScopedLock(this->mtx_);
    ev_flush_reqs_->push_back(req);
}

void Recorder::submit_ev(Marshal &m, Event* cb) {
    ev_req_t *req = new ev_req_t();
    std::string &s = req->first;
    req->second = cb;

    s.resize(m.content_size());
    m.write((void*)s.data(), m.content_size());

    ScopedLock(this->mtx_);
    ev_flush_reqs_->push_back(req);
}

#endif
void Recorder::submit(const std::string &buf,
		      const std::function<void(void)> &cb) {

    io_req_t *req = new io_req_t(buf, cb);
    ScopedLock(this->mtx_);
    flush_reqs_->push_back(req);
}

void Recorder::submit(Marshal &m,
                      const std::function<void(void)> &cb) {
    io_req_t *req = new io_req_t();
    std::string &s = req->first;
    req->second = cb;

    s.resize(m.content_size());
    m.write((void*)s.data(), m.content_size());

    ScopedLock(this->mtx_);
    flush_reqs_->push_back(req);
}

void Recorder::flush_buf() {
//    Log_info("flush_buf");
    mtx_.lock();

    int cnt_flush = 0;
    int sz_flush = 0;

    int sz = flush_reqs_->size();
    auto reqs = flush_reqs_;

    if (sz > 0) {
	flush_reqs_ = new std::list<io_req_t*>;
    }
#ifdef COROUTINE
    auto ev_reqs = ev_flush_reqs_;
    int ev_sz = ev_flush_reqs_->size();
    if (ev_sz > 0){
        ev_flush_reqs_ = new std::list<ev_req_t*>;
    }
#endif

    mtx_.unlock();

    if (sz == 0
#ifdef COROUTINE
        && ev_sz == 0
#endif
        ) {
	return;
    }

    for (auto &p: *reqs) {
	std::string &s = p->first;
	int ret = write(fd_, s.data(), s.size());
	verify(ret == s.size());
        cnt_flush ++;
        sz_flush += ret;
    }
#ifdef COROUTINE
    for (auto &p: *ev_reqs) {
        std::string &s = p->first;
        int ret = write(fd_, s.data(), s.size());
        verify(ret == s.size());
        cnt_flush ++;
        sz_flush += ret;
    }
#endif

#ifndef __APPLE__
    fdatasync(fd_);
#endif

    stat_cnt_.sample(cnt_flush);
    stat_sz_.sample(sz_flush);

    // push to call back reqs.

    mtx_.lock();
    callback_reqs_->insert(callback_reqs_->end(),
                           reqs->begin(), reqs->end());
#ifdef COROUTINE
    ev_callback_reqs_->insert(ev_callback_reqs_->end(), 
                           ev_reqs->begin(), ev_reqs->end());
#endif

    mtx_.unlock();
    return;

}

void Recorder::invoke_cb() {
    mtx_.lock();
    int sz = callback_reqs_->size();
    auto reqs = callback_reqs_;
    if (sz > 0) {
        callback_reqs_ = new std::list<io_req_t*>;
    }
#ifdef COROUTINE
    int ev_sz = ev_callback_reqs_->size();
    auto ev_reqs = ev_callback_reqs_;
    if (ev_sz > 0){
        ev_callback_reqs_ = new std::list<ev_req_t*>;
    }
#endif
    mtx_.unlock();

    if (sz !=0){
        for (auto &p: *reqs) {
            auto &cb = p->second;
            if (cb) {
                cb();
            }
            delete p;
        }
        delete reqs;
    }

#ifdef COROUTINE
    if (ev_sz != 0){
        for (auto &p: *ev_reqs){
            auto &cb = p->second;
            if (cb){
                cb->ready();
            }
            delete p;
        }
        delete ev_reqs;
    }
#endif
}

Recorder::~Recorder() {
}

} // namespace rrr
