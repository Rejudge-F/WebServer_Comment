/*************************************************************************
	> File Name: Thread.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Thu 26 Sep 2019 04:30:08 PM CST
	> Target: 
 ************************************************************************/

#include "Thread.h"
#include "CurrentThread.h" 
#include <memory>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <assert.h>
#include <iostream>
using namespace std;

namespace CurrentThread {
    __thread int         t_cacheTid = 0;
    __thread char        t_tidString[32];
    __thread int         t_tidStringLength = 6; // tid length = 5 
    __thread const char* t_thread_Name = "default";
}


// get thread tid use syscall
pid_t gettid() {
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cacheTid() {
    if(t_cacheTid == 0) {
        t_cacheTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d", t_cacheTid);
    }
}

struct ThreadData {
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc      func_;
    string          name_;
    pid_t*          tid_;
    CountDownLatch* latch_;

    ThreadData(const ThreadFunc& func, const string& name, pid_t *tid, CountDownLatch* latch):
    func_(func),
    name_(name),
    tid_(tid),
    latch_(latch) {}

    void runInThread() {
        *tid_ = CurrentThread::tid();
        tid_ = nullptr;
        latch_->countDown();
        latch_ = nullptr;

        CurrentThread::t_thread_Name = name_.empty() ? "Thread" : name_.c_str();
        prctl(PR_SET_NAME, CurrentThread::t_thread_Name);
        
        func_();
        CurrentThread::t_threadName = "finished";
    }
};

void *startThread(void *obj) {
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return nullptr;
}

Thread::Thread(const ThreadFunc& func, const std::string& name)
    : started_(false),
      joined_(false),
      pthreadId_(0),
      tid_(0),
      func_(func),
      name_(name),
      latch_(1)
{
    setDefaultName();
}

Thread::~Thread() {
    if(started_ && !joined_) 
        pthread_detach(pthreadId_);
}

void Thread::setDefaultName() {
    if(name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread");
        name_ = buf;
    }
}

void Thread::start() {
    assert(!started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
    if(pthread_create(&pthreadId_, nullptr, &startThread, data)) {
        started_ = false;
        delete data;
    } else {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, nullptr);
}
