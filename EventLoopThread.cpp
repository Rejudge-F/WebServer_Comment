/*************************************************************************
	> File Name: EventLoopThread.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Mon 07 Oct 2019 03:52:26 PM CST
	> Target: 
 ************************************************************************/

#include "EventLoopThread.h"
#include <functional>

EventLoopThread::EventLoopThread() :
    loop_(nullptr),
    existing_(false),
    thread_(bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
    mutex_(),
    cond_(mutex_)
{}

EventLoopThread::~EventLoopThread() {
    existing_ = true;
    if(loop_ != nullptr) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();
    {
        MutexLockGuard lock(mutex_);
        while(loop_ == nullptr) {
            cond_.wait();
        }
    }
    return loop_;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }
    loop.loop();
    loop_ = nullptr;
}
