# Using 1.70.0 in Ubuntu 18.04

GCC = g++
BOOST = /usr/local
INCLUDES += -I$(BOOST)/include -Iinclude
LIBS +=  -L$(BOOST)/lib -lboost_filesystem -lpthread -lcurses
CFLAGS= $(INCLUDES) $(LIBS)
#CFLAGS = -lpthread -lboost_filesystem -lcurses
objs = socket.o FTP.o
ser_objs = $(objs) FTP_Shardata.o FTP_Server.o
cli_objs = $(objs) passwd.o FTP_Client.o

all: ftp ftpd

ftp: ftp.cpp $(cli_objs)
	$(GCC) -o $@ $< -Wl,$(cli_objs) $(CFLAGS)
	
ftpd: ftpd.cpp $(ser_objs)
	$(GCC) -o $@  $< -Wl,$(ser_objs) $(CFLAGS)

FTP_Client: FTP_Client.cpp FTP_Client.h FTP_User.h
	$(GCC) -c $<

FTP_Server.o: FTP_Server.cpp FTP_Server.h FTP_Command.h server.h
	$(GCC) -c $<

FTP_Shardata.o: FTP_Shardata.cpp FTP_Shardata.h FTP_User.h Database.h
	$(GCC) -c $<

socket.o: socket.cpp socket.h exception.h
	$(GCC) -c $<

FTP.o: FTP.cpp FTP.h
	$(GCC) -c $<

passwd: passwd.cpp
	$(GCC) -c $<

test: main.cpp
	g++ -o $@ main.cpp

clean:
	rm *.o

clean_cli:
	rm $(cli_objs)

clean_serv:
	rm $(cli_ser)

clean_all:
	rm  *.o
	rm ftpd ftp
