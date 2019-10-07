/*************************************************************************
	> File Name: Main.cpp
	> Author: zhangfeng
	> Mail: brave_zephyr@163.com
	> Created Time: Mon 07 Oct 2019 07:24:40 PM CST
	> Target: 
 ************************************************************************/

#include "EventLoop.h"
#include "Server.h"
#include "base/Logging.h"
#include <getopt.h>
#include <string> 
#include <stdio.h>

int main(int argc, char *argv[]) {
    int threadNum = 4;
    int port = 80;
    std::string logPath = "./WebServer.log";
    int opt;
    const char *str = "t:l:p:";
    while((opt = getopt(argc, argv, str)) != -1) {
        switch(opt) {
            case 't':{
                threadNum = atoi(optarg);
                break;
            }
            case 'l':{
                logPath = optarg;
                break;
            }
            case 'p': {
                port = atoi(optarg);
                break;
            }
            default:break;
        }
    }
    Logger::setLogFileName(logPath);

    std::cout << "threadNum: " << threadNum << ", port: " << port << ", logPath: " << logPath << endl;

    EventLoop mainLoop;
    Server myHTTPServer(&mainLoop, threadNum, port);
    myHTTPServer.start();
    mainLoop.loop();
    return 0;
}
