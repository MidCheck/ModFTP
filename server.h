/*************************************************************************
    > File Name: server.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月08日 星期六 00时46分50秒
 ************************************************************************/
#include "socket.h"

namespace MidCHeck{
class Server: public Socket{
public:
	Server(const string& ip, int port):Socket(ip, port){
		try{
			if(bind(this->sock, (struct sockaddr*)&this->sock_addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
				throw "bind失败!";
			if(listen(this->sock, 10) == SOCKET_ERROR)
				throw "listen失败!";
		}
	}
	~Server(){
		std::cout << "关闭服务..." << std::endl;
		this->~Socket();
	}
#endif
};
} //命名空间MidCHeck结束
