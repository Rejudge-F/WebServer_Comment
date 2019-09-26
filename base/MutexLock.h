/*************************************************************************
	> File Name: MutexLock.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Thu 26 Sep 2019 02:46:29 PM CST
	> Target: 实现对锁的封装,包含常规的互斥锁以及自解锁 
 ************************************************************************/

#ifndef _MUTEXLOCK_H
#define _MUTEXLOCK_H

#include "noncopyable.h"
#include <pthread.h>
#include <cstdio>

class MutexLock: noncopyable {
public:
    MutexLock() {
        pthread_mutex_init(&mutex, nullptr);
    }

    ~MutexLock() {
        pthread_mutex_lock(&mutex);
        pthread_mutex_destroy(&mutex);
    }

    void lock() {
        pthread_mutex_lock(&mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_t* get() {
        return &mutex;
    }
private:
    pthread_mutex_t mutex;

private:
    friend class Condition;
};

class MutexGuard: noncopyable {
public: 
    explicit MutexGuard(MutexLock &_mutex): mutex(_mutex) {
        mutex.lock();
    }
    ~MutexGuard() {
        mutex.unlock();
    }
private:
    MutexLock &mutex;
};

#endif
