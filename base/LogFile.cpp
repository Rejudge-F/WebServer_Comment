/*************************************************************************
	> File Name: LogFile.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Sat 28 Sep 2019 11:02:55 AM CST
	> Target: 
 ************************************************************************/

#include "LogFile.h"
#include "FileUtil.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

using namespace std;

LogFile::LogFile(const string& basename, int flushEveryN):
    basename_(basename),
    flushEveryN_(flushEveryN),
    count_(0),
    mutex_(new MutexLock)
{
    file_.reset(new AppendFile(basename));
}

LogFile::~LogFile(){
}

void LogFile::append(const char* logline, const size_t len) {
    MutexLockGuard lockGuard(*mutex_);
    append_unlocked(logline, len);
}

void LogFile::flush() {
    MutexLockGuard lockGuard(*mutex_);
    file_->flush();
}

void LogFile::append_unlocked(const char* logline, const size_t len) {
    file_->append(logline, len);
    count_++;
    if(count_ >= flushEveryN_) {
        count_ = 0;
        file_->flush();
    }
}
