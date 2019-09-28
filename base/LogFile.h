/*************************************************************************
	> File Name: LogFile.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Sat 28 Sep 2019 10:52:01 AM CST
	> Target: 定义日志类的基本功能 
 ************************************************************************/

#ifndef _LOGFILE_H
#define _LOGFILE_H
#include "FileUtil.h"
#include "noncopyable.h"
#include "MutexLock.h"
#include <memory>
#include <string>

// 自动归档
class LogFile : noncopyable {
public:
    LogFile(const std::string &basename, int flushEveryN = 1024);
    ~LogFile();

    void append(const char* logline, const size_t len);
    void flush();
    bool rollFile();
private:
    void append_unlocked(const char* logline, const size_t len);
    
    const std::string basename_;
    const int flushEveryN_;

    int count_;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};

#endif
