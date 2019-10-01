CC=g++
CCFLAGS=-std=c++11 -g -Wall -O3 -D_PTHREADS
CCLINK=-lpthread

MAIN_SOURCE:=Main.cpp ./test/test_log.cpp  
SOURCE:=$(wildcard *.cpp ./test/*.cpp ./base/*.cpp) 
override SOURCE:=$(filter-out $(MAIN_SOURCE), $(SOURCE)) 
OBJECTS:=$(patsubst %.cpp, %.o, $(SOURCE)) 

MAIN_TARGET:=./bin/WebServer 
SUB_TARGET:=./bin/test_log 

all:$(SUB_TARGET) 
$(SUB_TARGET):$(OBJECTS) ./test/test_log.o 
	$(CC) $(CCFLAGS) $^ $(CCLINK) -o $@

%.o:%.cpp 
	$(CC) $(CCFLAGS) -c  -o $@ $<

MAIN_TARGET:time $(OBJECTS) Main.o
	$(CC) $(CCFLAGS) $(CCLINK) -o $@ $^ 

Main.o: Main.cpp 
	$(CC) $(CCFLAGS) -c $(CCLINK) -o $@ $^

.PHONY:
clean:
	rm $(OBJECTS) $(all)
time:
	date
