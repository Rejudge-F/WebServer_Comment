# WebServer-C++

[![github](https://badgen.net/badge/OS/ubuntu/orange)](https://cn.ubuntu.com/download)
[![github](https://badgen.net/badge/g++/5.4/green)](https://packages.ubuntu.com/xenial/g++-5)
[![github](https://badgen.net/badge/build/passing/green)](#)
[![github](https://badgen.net/badge/license/MIT/green)](#)


本项目基于[linyacool](https://github.com/linyacool/WebServer)

# 项目简介
整个项目实现了异步日志记录服务器运行状态，Http的管线化操作，长连接服务等，使用c++11中智能指针来避免不必要的内存泄露等功能，非常值得学习，我把该项目复现了一下，学习到了很多东西，主要是对reactor模式的理解以及双缓冲异步日志方面的理解，接下来我带大家梳理一下这个项目

---
- **注意：已下内容请结合代码食用**
---

# 双缓冲异步日志的实现
## 各个文件及其实现类之间的关系

### noncopyable文件
noncopyable.h文件实现了noncopyable类，noncopyable类又称作不可拷贝类，为什么有这种类的存在，我简单的说明一下，假设存在一个类A，成员为一个指针，然后我使用A类声明了两个对象，obj1和obj2，此时我执行了obj1=obj2，然后obj1的成员会被替换为obj2的成员，现在obj1之前指向的内存没有被释放造成内存泄漏，obj2指向的内存同时被两个对象拥有，在对象销毁的时候会进行两次析构函数造成双重释放，该类的实现参考的boost::noncopyable，把拷贝构造函数和重载等号设为private就可以了


### MutexLock文件
MutexLock.h以及MutexLock.cpp实现了MutexLock类以及MutexLockGuard类，MutexLock类封装了pthread_mutex_t的常用操作，包括lock、unlock以及get方法，MutexLockGuard则实现了一个自解锁的类，在构造函数中声明并加锁，析构时解锁，切记成员mutex_为引用



