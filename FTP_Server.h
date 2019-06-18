/*************************************************************************
    > File Name: FTP_Server.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月08日 星期六 02时25分09秒
 ************************************************************************/
#ifndef FTP_SERVER_H
#define FTP_SERVER_H

#include "server.h"
//#include "FTP_User.h"
#include "FTP_Command.h"
//#include "Database.h"

#include<thread>
//#include<unordered_map>
#include<string.h>

#include<vector>

#ifdef __linux__
#include<sys/epoll.h>
#include<pthread.h>
#define MAX_EVENT_NUMBER 1024
#endif

namespace MidCHeck{
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
/*网络传输层*/
class Worker{
private:
	int _epollfd;
	int _sockfd;
	MidCHeck::User *&_user;  // 指向当前sockfd的缓冲区
	//std::vector<char> buf(128); // 每个线程都有缓冲区
	COMMAND parse(char*, int&);
public:
	Worker(int efd, int sfd):_epollfd(efd), _sockfd(sfd),
		_user(Shardata::GetEntity()->users[sfd])
	{
		// 保证每一个命令都有数据空间
		// 注意到在quit退出或套接字关闭时释放空间
		if(_user == nullptr) _user = new User;
		_user->sockfd = _sockfd;
		std::cout << "[s] workin, sockfd: " << _sockfd << std::endl;
	}
	~Worker(){
		std::cout << "[s] work析构 " << std::endl;
	}
	void operator()(){
		char* buf = _user->buffer;
		bool flag = false;
		// 如果命令后的参数太长，则可能出bug
		while(1){
			// 非第一次接受数据
			// 可能不需要清空内存，因为解析时以结束符为标志
			if(flag) memset(buf, '\0', _user->rw_cur);
			int ret = recv(_sockfd, buf, 128, 0);

			if(ret == 0){
				//delete _user;
				//Shardata::GetEntity()->users.erase(sockfd);
				close(_sockfd);
				break;
			}else if(ret < 0){
				if(errno == EAGAIN){
					reset_oneshot(_epollfd, _sockfd);
					// read later
					break;
				}
			}else{
				flag = true;
				// 解析命令
				COMMAND recv_cmd = parse(buf, _user->rw_cur);
				
				Command* cmd = CommandFactory(_user).CreateCmd(recv_cmd);
				cmd->process();
				//　阻塞与非阻塞可能有bug
				send(_sockfd, buf, _user->rw_cur, 0);
			}
		}
	}
	
};
class FTP_Server: public Server{
private:
	epoll_event events[MAX_EVENT_NUMBER];
	std::vector<Socket> clients;
	int epollfd;
public:
	FTP_Server(const char* ip, int port):Server(ip, port){
		if((epollfd = epoll_create(5)) == -1) throw "Create epoll Failed!";
		addfd(epollfd, this->sock, false);
	}
	~FTP_Server(){ this->~Server(); }
	void start();

	//void PI(const char* op);
	//void DTP();
	#ifdef __linux__
	friend void addfd(int, int, bool);
	friend void reset_oneshot(int, int);
	#endif
};
} // 命名空间MidCHeck结束

#endif
