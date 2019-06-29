/*************************************************************************
    > File Name: FTP_Client.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月29日 星期六 13时41分54秒
 ************************************************************************/
#ifndef MIDCHECK_FTP_CLIENT_H
#define MIDCHECK_FTP_CLIENT_H
#include "socket.h"
#include "FTP_User.h"

namespace MidCHeck{
class FTP_Client: public User, public Socket{
private:
	std::vector<std::string> history;
public:
	FTP_Client(const char* ip, int port = 21);
	bool parse();
	void package();
	void start();
};
} // end namespace MidCHeck
#endif
