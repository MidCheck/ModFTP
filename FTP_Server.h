/*************************************************************************
    > File Name: FTP_Server.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月08日 星期六 02时25分09秒
 ************************************************************************/
#include "server.h"
#include<vertor>
#ifdef __linux__
#include<sys/epoll.h>
#include<pthread.h>
#define MAX_EVENT_NUMBER 1024
#endif

namespace MidCHeck{
/*访问控制命令*/
#define _USER	"USER"
#define _PASS	"PASS"
#define _CWD	"CWD"
#define _REIN	"REIN"
#define _QUIT	"QUIT"
/*传输参数命令*/
#define _PORT	"PORT"
#define _PASV	"PASV"
#define _TYPE	"TYPE"
#define _STRU	"STRU"
#define _MODE	"MODE"
/*FTP服务命令*/
#define _SIZE	"SIZE"
#define _RETR	"RETR"
#define _STOR	"STOR"
#define _ALLO	"ALLO"
#define _REST	"REST"
#define _RNFT	"RNFR"
#define _RNTO	"RNTO"
#define _ABOR	"ABOR"
#define _DELE	"DELE"
#define _RMD	"RMD"
#define _MKD	"MKD"
#define _PWD	"PWD"
#define _LIST	"LIST"
#define _NLST	"NLST"
#define _SYST	"SYST"
#define _HELP	"HELP"
#define _NOOP	"NOOP"


typedef enum { 
struct Buffer{
	std::vector<char> data;
	int read;
	int write;
	Buffer(int size = 128, int rwsize = 2): 
		data(std::vector<char>(size)), read(rwsize), write(rwsize){}
	bool full(int cap = 0){ return (write + cap) >= data.size() - 1; }
	void inc(int size = 2){
		data.resize(data.size()*size);
	}
	
};
struct Worker{
	int _epollfd;
	int _sockfd;
	vector<char> buf(128);
	Worker(int efd, int sfd):_epollfd(efd), _sockfd(sfd){}
	void operator()(){
		while(1){
			int ret = recv(_sockfd, &buf[0], buf.size(), 0);
			if(ret == 0){
				close(sockfd);
				break;
			}else if(ret < 0){
				if(errno == EAGAIN){
					reset_oneshot(_epollfd, _sockfd);
					// read later
					break;
				}
			}else{
				
			}
		}
	}
	
};
class FTP_Server: public Server{
private:
	epill_event events[MAX_EVENT_NUMBER];
	std::vector<Socket> clients;
public:
	FTP_Server(const string& ip, int port):Server(ip, port){}
	void start();

	void PI(const char* op);
	void DTP();
	#ifdef __linux__
	friend void addfd(int, int, bool);
	friend void reset_oneshot(int, int);
	#endif
};
#ifdef __linux__
inline void addfd(int epollfd, int fd, bool oneshot){
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	if(oneshot){
		event.events |= EPOLLONESHOT;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnonblocking(fd);
}
inline void reset_oneshot(int epollfd, int fd){
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}
#endif
} // 命名空间MidCHeck结束
