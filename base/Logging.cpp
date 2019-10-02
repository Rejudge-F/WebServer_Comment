/*************************************************************************
	> File Name: Logging.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Sun 29 Sep 2019 08:02:50 PM CST
	> Target: 通过建立一个AsyncLogging来实现对日志的append，将stream中的数据通过AsyncLogging来append到文件中 
 ************************************************************************/

#include "Logging.h"
#include "CurrentThread.h"
#include "Thread.h"
#include "AsyncLogging.h"
#include <assert.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging *AsyncLogger_;

std::string Logger::logFileName_ = "./WebServer.log";

void once_init() {
    AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
    AsyncLogger_->start();
}

void output(const char *msg, int len) {
    pthread_once(&once_control_, once_init);
    AsyncLogger_->append(msg, len);
}

Logger::Impl::Impl(const char *fileName, int line)
: stream_(),
line_(line),
basename_(fileName)
{
    formatTime();
}

void Logger::Impl::formatTime() {
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday(&tv, nullptr);
    time = tv.tv_sec;
    struct tm *p_time = localtime(&time);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S ", p_time);
    stream_ << str_t;
}

Logger::Logger(const char *fileName, int line) 
: impl_(fileName, line) {
    impl_.stream_ << impl_.basename_ << ":" << impl_.line_ << ": ";
}

Logger::~Logger(){
    impl_.stream_ << "\n";
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
}
