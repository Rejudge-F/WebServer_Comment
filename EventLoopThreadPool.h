/*************************************************************************
	> File Name: EventLoopThreadPool.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Mon 07 Oct 2019 05:31:20 PM CST
	> Target: 
 ************************************************************************/

#ifndef _EVENTLOOPTHREADPOOL_H
#define _EVENTLOOPTHREADPOOL_H
#include "base/noncopyable.h" 
#include "EventLoopThread.h"
#include "base/Logging.h"
#include <memory>
#include <vector>

class EventLoopThreadPool: noncopyable {
public:
    EventLoopThreadPool(EventLoop *baseloop, int numThreads);
    ~EventLoopThreadPool() {
        LOG << "~EventLoopThreadPool()";
    }
    void start();

    EventLoop *getNextLoop();
private:
    EventLoop *baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};
#endif
