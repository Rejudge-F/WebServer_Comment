/*************************************************************************
	> File Name: Channel.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Fri 04 Oct 2019 12:19:40 PM CST
	> Target: 负责一个文件描述符的IO事件,有IO事件的类都会与一个Channel进行一个关联 
 ************************************************************************/

#ifndef _CHANNEL_H
#define _CHANNEL_H
#include <string>
#include <unordered_map>
#include <memory>
#include <sys/epoll.h>
#include <functional>

class EventLoop;
class HttpData;

class Channel {
private:
    typedef std::function<void()> CallBack;
    EventLoop *loop_;
    int fd_;
    __uint32_t events_;
    __uint32_t revents_;
    __uint32_t lastEvents_;

    std::weak_ptr<HttpData> holder_;  // Channel的持有者

public:
    Channel(EventLoop *loop); 
    Channel(EventLoop *loop, int fd);
    ~Channel();
    int getFd();
    void setFd(int fd);
    
    void setHolder(std::shared_ptr<HttpData> holder) {
        holder_ = holder;
    }

    std::shared_ptr<HttpData> getHolder() {
        std::shared_ptr<HttpData> res(holder_.lock());
        return res;
    }

    void setReadHandler(CallBack &&readHandler) {
        readHandler_ = readHandler;
    }
    void setWriteHandler(CallBack &&writeHandler) {
        writeHandler_ = writeHandler;
    }
    void setErrorHandler(CallBack &&errorHandler) {
        errorHandler_ = errorHandler;
    }
    void setConnHandler(CallBack &&connHandler) {
        connHandler_ = connHandler;
    }

    void handleRead();
    void handleWrite();
    void handleError(int fd, int err_num, std::string short_msg);
    void handleConn();

    // 处理事件,如果有读不到或者HUP认为对端关闭 
    void handleEvents() {
        events_ = 0;
        if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
            events_ = 0;
            return ;
        }
        if(revents_ & EPOLLERR) {
            if(errorHandler_) errorHandler_();
            events_ = 0;
            return ;
        }
        if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
            handleRead();
        }
        if(revents_ & EPOLLOUT) {
            handleWrite();
        }
        handleConn();
    }
    
    void setRevents(__uint32_t ev) {
        revents_ = ev;
    }

    void setEvents(__uint32_t ev) {
        events_ = ev;
    }

    __uint32_t &getEvents() {
        return events_;
    }

    bool EqualAndUpdateLastEvents() {
        bool res = (lastEvents_ == events_); 
        lastEvents_ = events_;
        return res;
    }

    __uint32_t getLastEvents() {
        return lastEvents_;
    }
    


private:
    int parse_URI();        // 处理请求行
    int parse_Headers();    // 处理请求头
    int analysisRequest();  // 分析请求

    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;
    CallBack connHandler_;
};

typedef std::shared_ptr<Channel> SP_Channel;
#endif
