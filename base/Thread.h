/*************************************************************************
	> File Name: Thread.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Thu 26 Sep 2019 03:51:47 PM CST
	> Target: 定义Thread类，包含Thread的基本状态，以及实现开始和挂起函数 
 ************************************************************************/

#ifndef _THREAD_H
#define _THREAD_H

#include "CountDownLatch.h"
#include "noncopyable.h"
#include <functional>              // for function 
#include <memory>
#include <pthread.h>
#include <string>
#include <sys/syscall.h>           // for SYS_**
#include <unistd.h>

class Thread: noncopyable {
public:
    typedef std::function<void ()> ThreadFunc;
    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    ~Thread();
    void start();
    int join();
    bool started() const {return started_;}
    pid_t tid() const {return tid_;}
    const std::string& name() const {return name_;}
private:
    void setDefaultName();
    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;
};

#endif
