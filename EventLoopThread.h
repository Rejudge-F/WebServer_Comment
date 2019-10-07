/*************************************************************************
	> File Name: EventLoopThread.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Mon 07 Oct 2019 03:49:18 PM CST
	> Target: 
 ************************************************************************/

#ifndef _EVENTLOOPTHREAD_H
#define _EVENTLOOPTHREAD_H 
#include "base/Condition.h"
#include "base/MutexLock.h"
#include "base/Thread.h"
#include "EventLoop.h"
#include "base/noncopyable.h"

class EventLoopThread: noncopyable {
public: 
    EventLoopThread();
    ~EventLoopThread();
    EventLoop *startLoop();

private:
    void threadFunc();
    EventLoop *loop_;
    bool existing_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
};

#endif
