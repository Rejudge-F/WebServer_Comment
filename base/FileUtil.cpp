/*************************************************************************
	> File Name: FileUtil.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Sat 28 Sep 2019 10:35:59 AM CST
	> Target: 实现FileUtil类中的功能 
 ************************************************************************/

#include "FileUtil.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

AppendFile::AppendFile(string filename)
: fp_(fopen(filename.c_str(), "ae"))
{
    setbuffer(fp_, buffer_, sizeof buffer_);
}

AppendFile::~AppendFile() {
    fclose(fp_);
}

void AppendFile::append(const char* logline, const size_t len) {
    size_t n = this->write(logline, len);
    size_t remain = len - n;
    while(remain > 0) {
        size_t x = this->write(logline + n, remain);
        if(x == 0) {
            int err = ferror(fp_);
            if(err) {
                fprintf(stderr, "AppendFile::append() Filed\n");
            }
            break;
        }
        n += x;
        remain = len - n;
    }
}

void AppendFile::flush() {
    fflush(fp_);
}

size_t AppendFile::write(const char* logline, const size_t len) {
    return fwrite_unlocked(logline, 1, len, fp_);
}
