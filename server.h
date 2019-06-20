/*************************************************************************
    > File Name: server.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月08日 星期六 00时46分50秒
 ************************************************************************/
#ifndef MIDCHECK_SERVER_H
#define MIDCHECK_SERVER_H
#include "socket.h"

namespace MidCHeck{
class Server: public Socket{
public:
	Server(const char* ip, int port):Socket(ip, port){
		if(bind(this->sock, (struct sockaddr*)&this->sock_addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) mcthrow("bind失败!");
		if(listen(this->sock, 10) == SOCKET_ERROR) mcthrow("listen失败!");
	}
	~Server(){
		this->~Socket();
	}
};
} //命名空间MidCHeck结束
#endif
