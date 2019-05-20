# Using 1.68.0 in Ubuntu 18.04

BOOST = /usr/local
INCLUDES += -I$(BOOST)/include -Iinclude
LIBS += -L$(BOOST)/lib -lboost_filesystem -lboost_system -lpthread
CFLAGS=$(INCLUDES) $(LIBS)

all: example

FTPServer: example.cpp
	g++ -o $@ example.cpp $(CFLAGS)
clean:
	rm example
