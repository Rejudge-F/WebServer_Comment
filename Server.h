/*************************************************************************
	> File Name: Server.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Mon 07 Oct 2019 05:41:07 PM CST
	> Target: 
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H
#include "EventLoop.h"
#include "Channel.h"
#include "EventLoopThreadPool.h"
#include <memory>

class Server {
public:
    Server(EventLoop *loop, int threadNum, int port);
    ~Server() {}
    EventLoop *getLoop() const {return loop_;}
    void start();
    void handNewConn();
    void handThisConn() {
        loop_->updatePoller(acceptChannel_);
    }
private:
    EventLoop *loop_;
    int threadNum_;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;
    int port_;
    int listenFd_;
    static const int MAXFDS = 100000;
};
#endif
