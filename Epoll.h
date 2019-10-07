/*************************************************************************
	> File Name: Epoll.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Mon 07 Oct 2019 11:04:50 AM CST
	> Target: 
 ************************************************************************/

#ifndef _EPOLL_H
#define _EPOLL_H

#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <memory>

class Epoll {
public:
    Epoll();
   ~Epoll();
    void epoll_add(SP_Channel request, int timeout);
    void epoll_mod(SP_Channel request, int timeout);
    void epoll_del(SP_Channel request);
    std::vector<std::shared_ptr<Channel>> poll();
    std::vector<std::shared_ptr<Channel>> getEventsRequest(int events_num);
    void add_timer(std::shared_ptr<Channel> request_data, int timeout);

    int getEpollFd() {
        return epollFd_;
    }
    void handleExpired();
private:
    static const int MAXFDS = 100000;
    int epollFd_;
    std::vector<epoll_event> events_;
    std::shared_ptr<Channel> fd2chan_[MAXFDS];
    std::shared_ptr<HttpData> fd2http_[MAXFDS];
    TimerManager timerManager_;
};

#endif
