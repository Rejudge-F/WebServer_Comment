/*************************************************************************
	> File Name: Condition.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Thu 26 Sep 2019 03:03:11 PM CST
	> Target: 实现对条件变量的封装 
 ************************************************************************/

#ifndef _CONDITION_H
#define _CONDITION_H

#include "noncopyable.h"
#include "MutexLock.h"
#include <pthread.h>            // for pthread_cond_
#include <errno.h>
#include <cstdint>
#include <time.h>               // for timespec 

class Condition: noncopyable {
public:
    explicit Condition(MutexLock &_mutex): mutex(_mutex) {
        pthread_cond_init(&cond, nullptr);
    }
    ~Condition() {
        pthread_cond_destroy(&cond);
    }
    
    void wait() {
        pthread_cond_wait(&cond, mutex.get());
    }

    void notify() {
        pthread_cond_signal(&cond);
    }

    void notifyAll() {
        pthread_cond_broadcast(&cond);
    }

    bool waitForSeconds(int seconds) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cond, mutex.get(), &abstime);
    }

private:
    MutexLock &mutex;
    pthread_cond_t cond;
};

#endif
