/*************************************************************************
	> File Name: Channel.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Fri 04 Oct 2019 01:58:10 PM CST
	> Target: 
 ************************************************************************/

#include "Channel.h"
#include "Util.h"
#include <unistd.h>
#include <queue>
#include <cstdio>
#include <iostream>

using namespace std;

Channel::Channel(EventLoop *loop)
:   loop_(loop),
    events_(0),
    lastEvents_(0)
    {
}

Channel::Channel(EventLoop *loop, int fd) 
:   loop_(loop),
    fd_(fd),
    events_(0),
    lastEvents_(0)
    {        
}

Channel::~Channel() {}

int Channel::getFd() {
    return fd_;
}

void Channel::setFd(int fd) {
    fd_ = fd;
}

void Channel::handleRead() {
    if(readHandler_) {
        readHandler_();
    }
}

void Channel::handleWrite() {
    if(writeHandler_) {
        writeHandler_();
    }
}

void Channel::handleConn() {
    if(connHandler_) {
        connHandler_();
    }
}
