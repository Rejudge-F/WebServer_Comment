/*************************************************************************
	> File Name: CountDownLatch.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Thu 26 Sep 2019 03:20:58 PM CST
	> Target: 实现CountDownLatch类 
 ************************************************************************/

#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count):
    mutex_(),
    condition_(mutex_),
    count_(count) {
    }

void CountDownLatch::wait() {
    MutexGuard lock(mutex_);
    while(count_ > 0) 
        condition_.wait();
}

void CountDownLatch::countDown() {
    MutexGuard lock(mutex_);
    count_--;
    if(count_ == 0) {
        condition_.notifyAll();
    }
}
