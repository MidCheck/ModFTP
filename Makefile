# Using 1.70.0 in Ubuntu 18.04

BOOST = /usr/local
INCLUDES += -I$(BOOST)/include -Iinclude
LIBS += -L$(BOOST)/lib -lboost_filesystem -lboost_system -lpthread
CFLAGS=$(INCLUDES) $(LIBS) -std=c++17

all: FTPServer

FTPServer: FTPServer.cpp
	g++ -o $@ FTPServer.cpp $(CFLAGS)
clean:
	rm FTPServer
