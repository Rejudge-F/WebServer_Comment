# WebServer-C++

# WebServer-C++

[![github](https://badgen.net/badge/OS/ubuntu/orange)](https://cn.ubuntu.com/download)
[![github](https://badgen.net/badge/g++/5.4/green)](https://packages.ubuntu.com/xenial/g++-5)
[![github](https://badgen.net/badge/build/passing/green)](#)
[![github](https://badgen.net/badge/license/MIT/green)](https://github.com/Rejudge-F/WebServer_/blob/master/LICENSE)


本项目基于[linyacool](https://github.com/linyacool/WebServer)
         

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


# Reactor服务器的实现

## 服务器简介
整个服务器使用ET边缘触发模式，当有消息来临的时候全部读完，有消息需要写的话需要全部写完，对于每一个文件描述符需要一个Channel去处理IO事件，因此本项目中的所有文件描述符都会与一个Channel关联，每个文件描述符会设置Channel中的handle函数，通过这样的形式完成对一个文件描述符的IO，在整体上使用Reactor模式，整个程序拥有一个MainReactor，当有client来临的时候通过Round Robin的方法（在我看来像是轮流分配）分配一个subReactor去处理新的连接


## 各个文件实现及描述

### Util.h & Util.cpp
这里面包含的读写函数是为了适应整个服务器在ET模式下读取文件描述符，读到不能在读（EAGAIN），写到不能在写（EAGAIN）的情况，同时包含了对文件描述符设置的一些函数，主要属性包括非阻塞、对SIGPIPE（对端关闭，本端收到的信号）的处理，TCP中Nodelay算法的禁用，TCP中LINGER的设置

这里讲一下**linger属性**：我们知道在TCP断开连接的时候需要经历四次握手，在TIME_WAIT状态下等待时间时长为linger time，在`struct linger linger_;`这里面我们需要知道两个属性`l_onoff`和`l_linger`

1. 如果l_onoff=0的话那么主动关闭的一方不会等待进行四次握手，丢弃缓存数据直接发送RST包，并结束自己的连接
2. 如果l_onoff=1的话是需要进行四次握手的，那么既然需要四次握手就需要TIME_WAIT状态，那么TIME_WAIT等多久就会由l_linger来决定，如果l_linger=0的话那么就会按照默认的时间2MSL来进行，否则将会使用l_linger时间来作为等待的时间，等待的过程中这个连接的状态依然为TIME_WAIT

### Channel.h & Channel.cpp 
Channel 文件主要实现了Channel类，任务为读写一个文件描述符，以及对数据进行URI，Header的拆分并分析。

这里主要说明handleEvents：
- （EPOLLHUP && !EPOLLIN）：认为对端关闭
- （EPOLLERR）：文件描述符错误
- （EPOLLIN | EPOLLRDHUP | EPOLLPRI）：文件描述符有数据可以读，分别为普通数据，带外数据，对端关闭
- （EPOLLOUT）：有数据需要写入文件描述符

**注意**：
1. 带外数据：使用与普通数据不同的通道独立传送给用户，是相连的每一对流套接口间一个逻辑上独立的传输通道，通常TCP的URG紧急指针置位来完成。
2. 整个类的模式为，通过handleEvents来先处理读写时间，然后对连接进行处理，该项目中每个含有读写事件的文件描述符都会关联一个Channel，而Channel中的holder也保存了他的持有者

### Timer.h & Timer.cpp

Timer 文件实现了 TimerNode 超时节点类以及 TimerManager 超时节点管理类，每一个Node携带超时时间点，以及RequestData，Manager使用优先队列维护所有的Node，当Node的超时时间点越小就会再优先队列的头部。

Manager 因为是优先队列结构，所以不支持随机访问，如果有节点 delete 的时候并不会直接delete，最迟会在他的删除时间点因为超时删除，这样做的好处是不需要遍历队列去删除，以及如果相应的Request Data再次来临的时候不需要重新申请Node

### Epoll.h & Epoll.cpp

Epoll 文件实现了单一的 Epoll 类，封装了对Epoll的基本del，mod，add方法，同时通过 poll 来收集事件到 events，将events传送到Channel中的Revents，然后交给Channel处理，该Epoll的基本单位是Channel，并且拥有一个超时管理器，Epoll类主要实现了反应堆服务器模型的基本操作以及对应事件的处理方式。

### EventLoop.h & EventLoop.cpp

引用原作者：One loop per thread意味着每个线程只能有一个EventLoop对象，EventLoop即是时间循环，每次从poller里拿活跃事件，并给到Channel里分发处理。EventLoop中的loop函数会在最底层(Thread)中被真正调用，开始无限的循环，直到某一轮的检查到退出状态后从底层一层一层的退出。

### EventLoopThread.h & EventLoop.cpp

EventLoopThread 用来开辟一个线程承载EventLoop，通过执行EventLoop中的loop函数来实现EventLoop的时间循环

### EventLoopThreadPool.h & EventLoopThreadPool.cpp 

EventLoopThreadPool 将EventLoopThread抽象化到上层逻辑，使用线程池来管理每一个EventloopThread

### HttpData.h & HttpData.cpp 

HttpData 在整个项目中代码量最多，大概的流程是持有一个fd和Channel然后将各种Handler函数给Channel，每个函数总结起来大致就是模拟了Http的数据解析，包括请求行，请求头，请求body的解析，最终交给Channel处理，其中定义了很多的枚举类型，这些枚举类型是解析Http的流程指引，一步一步解析，也就是当前解析的状态，看似复杂，实际上只是在模拟Http的解析过程

### Sever.h & Server.cpp

Server 属于本项目的上层逻辑类了，启动一个监听接口，然后通过handleConn来将新链接的Channel加入Channel中，进行处理

### Main.cpp

Main 主要为读取命令行参数，然后传递给Server类进行运行

# 项目总结

这个项目主要以Channel和EventLoop来运行，其他主要是上层封装，值得学习的是双缓冲日志的实现方式，以及一个项目如何从底层一层一层到上层逻辑的构建，能够体验到一个项目从底层到上层的开发过程，深刻体会一下EventLoop和Channel类，其次就是Http的处理过程，能够理解到Http的工作过程，能够提升自己对整个项目和Http的理解























