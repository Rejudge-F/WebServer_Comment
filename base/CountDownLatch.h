/*************************************************************************
	> File Name: CountDownLatch.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Thu 26 Sep 2019 03:15:21 PM CST
	> Target: 声明CountDownLatch,目的是让一个线程等待其他线程结束之后再执行 
 ************************************************************************/

#ifndef _COUNTDOWNLATCH_H
#define _COUNTDOWNLATCH_H

#include "Condition.h"
#include "noncopyable.h"
#include "MutexLock.h"

class CountDownLatch: noncopyable {
public: 
    explicit CountDownLatch(int count);
    void wait();
    void countDown();
private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};

#endif
