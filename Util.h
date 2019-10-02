/*************************************************************************
	> File Name: Util.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Wed 02 Oct 2019 03:37:38 PM CST
	> Target: 
 ************************************************************************/

#ifndef _UTIL_H
#define _UTIL_H

#include <cstdio>
#include <string>

ssize_t readn(int fd, void *buff, size_t n);
ssize_t readn(int fd, std::string &inBuffer, bool &zero);
ssize_t readn(int fd, std::string &inBuffer);
ssize_t writen(int fd, void *buff, size_t n);
ssize_t writen(int fd, std::string &sbuff);
void handle_for_sigpipe();
int setSocketNonBlocking(int fd);
void setSocketNodelay(int fd);
void setSocketNoLinger(int fd);
void shutDownWR(int fd);
int socket_bind_listen(int port);

#endif
