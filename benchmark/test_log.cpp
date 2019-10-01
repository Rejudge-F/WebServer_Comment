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

Logger logger(__FILE__, __LINE__);

void threadFunc() {
    for(int i = 0; i < lines; i++) {
        logger.stream() << "safasfdsfsafaf";
    }
}

void testSingleThread() {
}

int main() {
    threadFunc();
    cout << __FILE__ << " " << __LINE__ << endl;
    return 0;
}

