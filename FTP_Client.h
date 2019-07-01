/*************************************************************************
    > File Name: FTP_Client.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月29日 星期六 13时41分54秒
 ************************************************************************/
#ifndef MIDCHECK_FTP_CLIENT_H
#define MIDCHECK_FTP_CLIENT_H
#include <unordered_map>
#include "socket.h"
#include "FTP_User.h"
#include "passwd.h"
namespace MidCHeck{
class FTP_Client: public User, public Socket{
private:
	std::vector<std::string> history;
	std::unordered_map<COMMAND, std::string> cmd_help;
	std::unordered_map<std::string, COMMAND> cmd_map;
public:
	FTP_Client(const char* ip, int port = 21);
	COMMAND parse();
	void package();
	void start();
	void CmdHelp();
	void CmdUser();
	void CmdList();
	void CmdRetr();
	void CmdStor();
};
} // end namespace MidCHeck
#endif
