cc=g++
server=ChatServer
client=ChatClient
INCLUDE=-I./lib/include
LDFLAGS=-std=c++11 -lpthread
LIB_PATH=./lib/lib/libjsoncpp.a

.PHONY:all
all:$(server) $(client)

$(client):ChatClient.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH) -lncurses $(LDFLAGS)

$(server):ChatServer.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGS)

.PHONY:clean
clean:
	rm -f $(server) $(client)
