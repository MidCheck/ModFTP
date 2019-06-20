# Using 1.70.0 in Ubuntu 18.04

#BOOST = /usr/local
#INCLUDES += -I$(BOOST)/include -Iinclude
#LIBS += -L$(BOOST)/lib -lboost_filesystem -lpthread
#CFLAGS= $(LIBS) 
CFLAGS = -l pthread -l boost_filesystem
objs = socket.o FTP_Shardata.o FTP_Server.o

all: main

main: main.cpp $(objs)
	g++ $(CFLAGS) -o $@ $< -Wl,$(objs) 

FTP_Server.o: FTP_Server.cpp FTP_Server.h FTP_Command.h server.h
	g++ -c $<

FTP_Shardata.o: FTP_Shardata.cpp FTP_Shardata.h FTP_User.h Database.h
	g++ -c $<

socket.o: socket.cpp socket.h exception.h
	g++ -c $<

test:
	g++ $(CFLAGS) -o $@ test.cpp

clean:
	rm $(objs)

clean_all:
	rm $(objs)
	rm main
