/*************************************************************************
    > File Name: FTP_Server.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月09日 星期日 20时46分32秒
 ************************************************************************/
#include "FTP_Server.h"
using namespace MidCHeck;

COMMAND Worker::parse(char* buf, int& cur){
	char* ptr = strstr(buf, " ");
	std::cout << "  [w] recv:[" << buf << "]" << std::endl; 
	if(ptr == nullptr) return ERRCOMMAND;
	char *cmd = new char[ptr-buf];
	memcpy(cmd, buf, ptr-buf);
	//cmd[ptr-buf] = '\0';
	cur = ptr-buf+2;
	//std::cout << "  [w] cmd:[" << cmd << "]" << std::endl;
	Shardata* sd = Shardata::GetEntity();
	std::unordered_map<std::string, COMMAND>::const_iterator it =
		sd->cmd_map.find(cmd);
	COMMAND ret = it != sd->cmd_map.end() ? it->second : ERRCOMMAND;
	delete[] cmd;
	return ret;
}
void FTP_Server::start(){
	while(1){
		int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
		std::cout << "epoll_wait" << std::endl;
		if(ret < 0) throw "[-] epoll failure";
		for(int i = 0; i < ret; ++i){
			int sockfd = events[i].data.fd;
			if(sockfd == this->sock){
				std::cout << "[s] sockfd" << std::endl;

				struct sockaddr_in client_address;
				socklen_t client_addrlength = sizeof(client_address);
				int connfd = accept(this->sock, (struct sockaddr*)&client_address,
						&client_addrlength);
				/*对每个非监听文件描述符都注册EPOLLONESHOT事件*/
				std::cout << "[s] sock connfd: " << connfd << std::endl;
				addfd(epollfd, connfd, true);
			}else if(events[i].events & EPOLLIN){
				std::cout << "[s] epollin" << std::endl;
				Worker work(epollfd, sockfd);
				std::thread my_thread(work);
				my_thread.detach();
			}else {
				std::cout << "[?] something else happeded" << std::endl;
			}
		}
	}
}
