# WebServer-C++

# WebServer-C++

[![github](https://badgen.net/badge/OS/ubuntu/orange)](https://cn.ubuntu.com/download)
[![github](https://badgen.net/badge/g++/5.4/green)](https://packages.ubuntu.com/xenial/g++-5)
[![github](https://badgen.net/badge/build/passing/green)](#)
[![github](https://badgen.net/badge/license/MIT/green)](https://github.com/Rejudge-F/WebServer_/blob/master/LICENSE)


本项目基于[linyacool](https://github.com/linyacool/WebServer)

Table of Contents
=================

   * [WebServer-C  ](#webserver-c)
   * [WebServer-C  ](#webserver-c-1)
   * [项目简介](#项目简介)
   * [双缓冲异步日志的实现](#双缓冲异步日志的实现)
      * [何为双缓冲异步日志](#何为双缓冲异步日志)
      * [该项目中如何实现的](#该项目中如何实现的)
      * [各个文件及其实现类](#各个文件及其实现类)
         * [noncopyable.h](#noncopyableh)
         * [MutexLock.h](#mutexlockh)
         * [Condition.h](#conditionh)
         * [CountDownLatch.h &amp; CountDownLatch.cpp](#countdownlatchh--countdownlatchcpp)
         * [CurrentThread.h](#currentthreadh)
         * [Thread.h &amp; Thread.cpp](#threadh--threadcpp)
         * [FileUtil.h &amp; FileUtil.cpp](#fileutilh--fileutilcpp)
         * [LogFile.h &amp; LogFile.cpp](#logfileh--logfilecpp)
         * [AsyncLogging.h &amp; AsyncLogging.cpp](#asyncloggingh--asyncloggingcpp)
         * [LogStream.h &amp; LogStream.cpp](#logstreamh--logstreamcpp)
         * [Logging.h &amp; Logging.cpp](#loggingh--loggingcpp)
         

# 项目简介
整个项目实现了异步日志记录服务器运行状态，Http的管线化操作，长连接服务等，使用c++11中智能指针来避免不必要的内存泄露等功能，非常值得学习，我把该项目复现了一下，学习到了很多东西，主要是对reactor模式的理解以及双缓冲异步日志方面的理解，接下来我带大家梳理一下这个项目

---
- **注意：已下内容请结合代码食用**
---


# 双缓冲异步日志的实现

## 何为双缓冲异步日志

连接：[双缓冲异步日志](https://blog.csdn.net/daaikuaichuan/article/details/86500108)

## 该项目中如何实现的
1. 基础类：noncopyable、MutexLock、Conditicon、CurrentThread、Thread
2. 文件工具类：FileUtil、LogFile、LogStream
3. 逻辑实现类：AsyncLogging
4. 对外接口类：Logging

## 各个文件及其实现类

### noncopyable.h 
noncopyable.h文件实现了noncopyable类，noncopyable类又称作不可拷贝类，为什么有这种类的存在，我简单的说明一下，假设存在一个类A，成员为一个指针，然后我使用A类声明了两个对象，obj1和obj2，此时我执行了obj1=obj2，然后obj1的成员会被替换为obj2的成员，现在obj1之前指向的内存没有被释放造成内存泄漏，obj2指向的内存同时被两个对象拥有，在对象销毁的时候会进行两次析构函数造成双重释放，该类的实现参考的boost::noncopyable，把拷贝构造函数和重载等号设为private就可以了


### MutexLock.h
MutexLock.h以及MutexLock.cpp实现了MutexLock类以及MutexLockGuard类，MutexLock类封装了pthread_mutex_t的常用操作，包括lock、unlock以及get方法，MutexLockGuard则实现了一个自解锁的类，在构造函数中声明并加锁，析构时解锁，切记成员mutex_为引用

### Condition.h
Condition.h 封装了条件变量 pthread_cond_t 的基本操作，包括wait、timewait、signal以及broadcast，其中条件变量中的锁使用MutexLock，因此Condition类为MutexLock类的友元类

### CountDownLatch.h & CountDownLatch.cpp
首先我们了解一下什么是CountDownLatch：
- CountDownLatch这个类使一个线程等待其他线程各自执行完毕后再执行。
- 是通过一个计数器来实现的，计数器的初始值是线程的数量。每当一个线程执行完毕后，计数器的值就-1，当计数器的值为0时，表示所有线程都执行完毕，然后在闭锁上等待的线程就可以恢复工作了。

所以我们想到这个类的实现应该是包含一个锁，一个条件变量，以及一个计数器，成员函数就会有wait和countdown两个成员函数，需要注意的是因为CountDownLatch是运行再多线程环境下，所以需要使用锁来保持计数器不会出现时序竞态，每次读写计数器的时候需要加锁的和解锁

### CurrentThread.h
CurrentThread.h 使用 **__thread** 关键字声明了线程私有的数据，这里只保存线程tid，线程tid的string形式，线程tid的长度，线程名字

**注意**：这里的tid获取的方法为syscall(SYS_gettid)系统调用，这个tid在系统为唯一 ，与pthread_self() 的值进行区分


### Thread.h & Thread.cpp
Thread 文件主要实现了线程类，具体实现，我们可以把类差分为线程信息状态以及回调函数，我们主要考虑怎么实现start和join函数，start函数我们可以直观的想到一个pthread_create函数，然后根据函数的参数，我们想到会有一个callback函数，以及一个参数包，所以我们考虑将线程参数全部添加到结构体ThreadData中去，大概就是线程tid，latch，name以及任务函数，然后创建callback，去解析ThreadData，调用ThreadData中的RunInThread执行线程的任务

**注意**
- 记得使用latch来确保线程执行完毕，在去转线程状态为join
- 在Thread类析构的时候如果任务还处于运行状态的话，把线程detach出去执行，确保任务完成

### FileUtil.h & FileUtil.cpp
FileUtil 文件实现了AppenFile类，实现底层对文件的打开，append和write操作，这里自己对文件描述符设置一个新的缓冲区buffer， 每次需要保证把所有的内容全部写入文件，需要处理一下

### LogFile.h & LogFile.cpp
LogFile 实现LogFile类，该类持有一个FileUtil实现的AppendFile，为了保证线程安全，还需要一个互斥锁，然后提供appendn次去flush一次，这个通过一个计数器实现，当counter到达flushEveryN的时候就会去触发flush操作，将内容刷入硬盘

### AsyncLogging.h & AsyncLogging.cpp
AsyncLogging 实现AsyncLogging类，是该双缓冲异步日志的关键类，他持有一个LogFile，以及几个缓冲区（这里用到多个缓冲区，但是目的和双缓冲是一样的)，另外，该类持有一个Thread来执行日志的写入操作，同样主要实现append和flush操作。

**实现流程：**
- 将append后的currentBuffer压入buffers中
- 将buffersToWrite与buffers中的元素进行互换
- 将bufferToWrite写入文件


### LogStream.h & LogStream.cpp
LogStream 实现了FixedBuffer类（可以理解为一个比较灵活的buffer，底层还是数组），然后LogStream类主要对<<运算符进行重载，通过格式化的方式写入buffer，此时通过LogStream写入的日志全部缓存到FixedBuffer中，当需要写入的时候需要需要通过Logger对外接口调用全部写入Buffer中，然后去调用AsyncLogging写入，这里初始话AsyncLogging使用了PTHREAD_ONCE_INIT属性。

### Logging.h & Logging.cpp
Logging 实现了Logger类以及对外的Impl接口类，接口内主要是对LogStream的描述，Logging.cpp中声明一个AsyncLogging对象，首先会将日志写入stream中，然后在logger类析构的时候将日志写入调用线程异步写入硬盘
































