/*************************************************************************
	> File Name: Util.h
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Wed 02 Oct 2019 03:37:38 PM CST
 ************************************************************************/

#ifndef _UTIL_H
#define _UTIL_H

#include <cstdio>
#include <string>

ssize_t readn(int fd, void *buff, size_t n);                 // 读取结构体数据
ssize_t readn(int fd, std::string &inBuffer, bool &zero);    // 读取字符串数据zero为最终是否读取完成
ssize_t readn(int fd, std::string &inBuffer);                // 读取字符串数据 
ssize_t writen(int fd, void *buff, size_t n);                // 写入结构体数据 
ssize_t writen(int fd, std::string &sbuff);                  // 写入字符串数据
void handle_for_sigpipe();                                   // 处理对端关闭收到了SIG_PIPE信号
int setSocketNonBlocking(int fd);                            // 设置文件描述符为非阻塞态 
void setSocketNodelay(int fd);                               // 关闭TCP的nagle算法
void setSocketNoLinger(int fd);                              // 对端关闭后会等待一定时长
void shutDownWR(int fd);                                     // shut掉文件描述符的读写 
int socket_bind_listen(int port);                            // 返回一个文件描述符,监听port端口

#endif
