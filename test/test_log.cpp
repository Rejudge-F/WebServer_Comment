/*************************************************************************
	> File Name: test_log.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Mon 30 Sep 2019 08:06:31 PM CST
	> Target: 
 ************************************************************************/
#include "../base/Logging.h"
#include "../base/Thread.h"
#include <iostream>

using namespace std;

const int lines = 100000;


void threadFunc() {
    for(int i = 0; i < lines; i++) {
        LOG  << i;
    }
}

void testSingleThread() {
    shared_ptr<Thread> thread(new Thread(threadFunc, "single_test"));
    thread->start();
}

int main() {
    threadFunc();
    sleep(3);
    return 0;
}


