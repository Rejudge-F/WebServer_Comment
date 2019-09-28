/*************************************************************************
	> File Name: CurrentThread.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Thu 26 Sep 2019 04:04:45 PM CST
	> Target: 定义当前线程的相关信息，使用 __thread 关键字，保证每个Thread互不影响
 ************************************************************************/

#ifndef _CURRENTTHREAD_H
#define _CURRENTTHREAD_H

#include <stdint.h>

namespace CurrentThread {
    extern __thread int         t_cacheTid;
    extern __thread char        t_tidString[32];
    extern __thread int         t_tidStringLength;
    extern __thread const char* t_threadName;
    void cacheTid();
    inline int tid() {
        if(__builtin_expect(t_cacheTid == 0, 0)) {
            cacheTid();
        }
        return t_cacheTid;
    }
    inline const char* tidString() {
        return t_tidString;
    }
    inline int tidStringLength() {
        return t_tidStringLength;
    }
    inline const char* name() {
        return t_threadName;
    }
}

#endif
