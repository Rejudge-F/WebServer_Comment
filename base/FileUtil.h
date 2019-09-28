/*************************************************************************
	> File Name: FileUtil.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Sat 28 Sep 2019 10:30:41 AM CST
	> Target: 实现一个文件工具类，包含append和write功能 
 ************************************************************************/

#ifndef _FILEUTIL_H
#define _FILEUTIL_H

#include "noncopyable.h"
#include <string>

class AppendFile: noncopyable {
public: 
    explicit AppendFile(std::string filename);
    ~AppendFile();
    void append(const char* logline, const size_t len);
    void flush();
private:
    size_t write(const char* logline, size_t len);
    FILE* fp_;
    char buffer_[1<<16];
};

#endif
