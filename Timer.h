/*************************************************************************
	> File Name: Timer.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Wed 02 Oct 2019 07:25:59 PM CST
	> Target: 
 ************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

#include "base/noncopyable.h"
#include "base/MutexLock.h"
#include "HttpData.h"
#include <unistd.h>
#include <memory>
#include <queue>
#include <deque>

class HttpData;

class TimerNode {
public:
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
    TimerNode(TimerNode &timerNode);
    ~TimerNode();

    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted() {deleted_ = true;}
    bool isDeleted() const {return deleted_;}
    size_t getExpTime() const {return expiredTime_;}
private:
    bool deleted_;
    size_t expiredTime_;
    std::shared_ptr<HttpData> SPHttpData;
};

struct TimerCmp {
    bool operator()(std::shared_ptr<TimerNode> &first, std::shared_ptr<TimerNode> &second) const {
        return first->getExpTime() > second->getExpTime();
    }
};

class TimerManager {
public:
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();
private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp> timerNodeQueue;
};

#endif
