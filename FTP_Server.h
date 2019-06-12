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
#include<mutex>

namespace MidCHeck{
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
class SharedData{
private:
	std::unordered_map<const char*, COMMAND> cmd_map;
	std::unordered_mpa<const char*, User> users;
	std::string dir_root;
	static std::mutex m;
public:
	ShareData(const string& home);
	static SharedData* GetEntity();
	bool AddUser(User &user);
	bool IsLegalUser(const char* user_name){
		return users.end() != users.find(user_name);
	}
	string GetPasswd(const char* user_name){
		std::unordered_map<string, string>::const_iterator user = users.find(user_name);
		return user != users.end() ? user->second : "";
	}
	COMMAND GetCmd(std::vector<char>& buf, int& cur){
		char cmd[5] = {'\0'}, *ptr = strstr(&buf[1], ' ');
		cur = ptr - &buf[0];
		try{
			memcpy(cmd, &buf[1], cur - 1);
		}catch(...){
			for(int i = 0; i < cur - 1; ++i)
				cmd[i] = buf[1+i];
		}
		std::unordered_map<const char*, COMMAND>::const_iterator it 
			= cmd_map.find(cmd);
		return it != cmd_map.end() ? it->second : ERRCOMMAND;
	}
};
std::mutex SharedData::m;
SharedData* SharedData::GetEntity(){
	m.lock();
	static SharedData obj;
	m.unlock();
	return &obj;
}
class Worker{
private:
	int _epollfd;
	int _sockfd;
	int _rw_cur;
	std::vector<char> buf(128); // 每个线程都有缓冲区
	COMMAND parse(std::vector<char> &, int&);
public:
	Worker(int efd, int sfd):_epollfd(efd), _sockfd(sfd), _rw_cur(0){}
	void operator()(){
		buf[0] = '\0';
		// 如果命令后的参数太长，则可能出bug
		while(1){
			// 非第一次接受数据
			if(buf[0] != 0) buf.clear();
			int ret = recv(_sockfd, &buf[1], buf.size() - 1, 0);
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
				buf[0]++;
				// 解析命令
				Command* cmd = CommandFactory(parse(buf, _rw_cur));
				if(cmd.process(
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
