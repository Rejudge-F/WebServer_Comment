/*************************************************************************
	> File Name: noncopyable.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Thu 26 Sep 2019 02:40:29 PM CST
    > Target: 实现一个不可copy的类 
 ************************************************************************/

#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H

class noncopyable {
protected:
    noncopyable();
    ~noncopyable();
private:
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
};

#endif
