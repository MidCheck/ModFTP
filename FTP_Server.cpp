/*************************************************************************
    > File Name: FTP_Server.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月09日 星期日 20时46分32秒
 ************************************************************************/
#include "FTP_Server.h"
using namespace MidCHeck;

COMMAND Worker::parse(char* buf, int& cur){
	if(!replace(buf, " ")){ 
		// 检查是否以'\n'结尾的合法命令
		if(!replace(buf)) return ERRCOMMAND;
	}
	// 此处cur的定位只假设此buf的首地址开始计数,如果自定义格式应由外部保证cur
	cur = strlen(buf) + 1;

	Shardata* sd = Shardata::GetEntity();
	std::unordered_map<std::string, COMMAND>::const_iterator it =
		sd->cmd_map.find(buf);
	return it != sd->cmd_map.end() ? it->second : ERRCOMMAND;
}
void FTP_Server::start(){
	while(1){
		int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
		if(ret < 0) mcthrow("epoll failure");
		Debug("epoll_wait: %d", ret);
		for(int i = 0; i < ret; ++i){
			int sockfd = events[i].data.fd;
			Debug(" sockfd: %d", sockfd);
			if(sockfd == this->sock){
				struct sockaddr_in client_address;
				socklen_t client_addrlength = sizeof(client_address);
				int connfd = accept(this->sock, (struct sockaddr*)&client_address,
						&client_addrlength);
				int len = sizeof("220 Welcome to my FTP site!\r\n") - 1;
				const char *ftp_220 = "220 Welcome to my FTP site!\r\n";
				send(connfd, ftp_220, len , 0);
				/*对每个非监听文件描述符都注册EPOLLONESHOT事件*/
				addfd(epollfd, connfd, true);
				User *new_user = new User;
				new_user->sockfd = connfd;
			}else if(events[i].events & EPOLLIN){
				Debug("work 定义前");
				Worker work(epollfd, sockfd); // work析够一次
				Debug("work 定义后");
				std::thread my_thread(work);
				Debug("work thread 创建后");
				my_thread.detach();
				Debug("work thread 分离后");
			}else {
				std::cerr << "[?] something else happeded" << std::endl;
			}
		}
	}
}
