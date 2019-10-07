/*************************************************************************
	> File Name: EventLoopThreadPool.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Mon 07 Oct 2019 05:35:02 PM CST
	> Target: 
 ************************************************************************/

#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop, int numThreads) :
    baseLoop_(baseloop),
    started_(false),
    numThreads_(numThreads),
    next_(0)
{
    if(numThreads_ <= 0) {
        LOG << "numThreads_ <= 0";
        abort();
    }
}

void EventLoopThreadPool::start() {
    baseLoop_->assertInLoopThread();
    started_ = true;
    for(int i = 0; i < numThreads_; i++) {
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        threads_.push_back(t);
        loops_.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventLoop *loop = baseLoop_;
    if(!loops_.empty()) {
        loop = loops_[next_];
        next_ = (next_ + 1) % numThreads_;
    }
    return loop;
}
