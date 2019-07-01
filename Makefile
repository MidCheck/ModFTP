# Using 1.70.0 in Ubuntu 18.04

GCC = g++
BOOST = /usr/local
INCLUDES += -I$(BOOST)/include -Iinclude
LIBS +=  -L$(BOOST)/lib -lboost_filesystem -lpthread
CFLAGS= $(INCLUDES) $(LIBS)
#CFLAGS = -l pthread -l boost_filesystem
objs = socket.o FTP_Shardata.o FTP_Server.o FTP.o
cli_objs = socket.o passwd.o FTP.o
all: server client

client: FTP_Client.cpp $(cli_objs) FTP_Client.h FTP_User.h
	$(GCC) -o $@ $< -Wl,$(cli_objs) -lcurses

passwd: passwd.cpp
	$(GCC) -c $<

server: main.cpp $(objs)
	$(GCC) -o $@  $< -Wl,$(objs) $(CFLAGS)

FTP_Server.o: FTP_Server.cpp FTP_Server.h FTP_Command.h server.h
	$(GCC) -c $<

FTP_Shardata.o: FTP_Shardata.cpp FTP_Shardata.h FTP_User.h Database.h
	$(GCC) -c $<

socket.o: socket.cpp socket.h exception.h
	$(GCC) -c $<

FTP.o: FTP.cpp FTP.h
	$(GCC) -c $<

test: test.cpp
	g++ -o $@ test.cpp

clean:
	rm $(objs) 

clean_cli:
	rm $(cli_objs)

clean_all:
	rm  *.o
	rm server client
