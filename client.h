/*************************************************************************
    > File Name: client.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月08日 星期六 01时32分11秒
 ************************************************************************/
#include "socket.h"

namespace MidCHeck{
class Client: public Socket{
public:
	Client(const string& ip, int port):Socket(ip, port){
		if(!this->connect()){
			this->~Socket();
			exit(1);
		}
	}
	bool connect(){
		try{
			if(connect(this->sock, (struct sockaddr*)&this->sock_addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
				throw runtime_error("连接失败");
			else
				return true;
		}catch(runtime_error err){
			std::cerr << err.what() <<  std::flush;
		}
		return false;
	}
};
} //命名空间MidCHeck结束
