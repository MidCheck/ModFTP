/*************************************************************************
    > File Name: FTP_User.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月12日 星期三 20时22分44秒
 ************************************************************************/
#ifndef MIDCHECK_FTP_USER_H
#define MIDCHECK_FTP_USER_H
#include<string>
#include<vector>
#include<sys/socket.h>
#include<netinet/in.h> 
#include<condition_variable>
#include<boost/filesystem.hpp>

namespace fs = boost::filesystem;
namespace MidCHeck{
/*用户状态*/
typedef enum {
	/*语法错误, 合法用户，非法用户*/
	SYNTAXERR,	ILLEGAL,	LEGAL, 
	/*未登录，已登录，已退出*/
	NOTLOGGED,	LOGGED,		QUITED, 
	/*正在传输中, PORT模式，PASV模式*/
	TRANSING,	MODEPORT,	MODEPASV,
} Usrstat;

/*用户类，存放用户环境变量以及当前信息*/
class User{
public:
	int sockfd;  // 用户的控制连接套接字
	// 用户的数据连接套接字
	// 在主动模式下，直连用户的端口,被动模式下，接受用户的连接
	int dsockfd; 

	/* 主动与被动模式都需要两个套接字
	 * 对于主动模式来说,dsockfd用来绑定服务器端数据端口,然后用来连接客户端
	 * 对于被动模式来说,dsockfd用来侦听服务器端数据接口,csockfd用来连接客户端　
	 */
	std::string name;
	std::string passwd;
	std::string home; // 记录用户主目录
	std::string path; // 记录用户当前路径
	
	Usrstat auth;  // 记录用户权限
	Usrstat status; // 记录用户状态

	Usrstat mode; // 记录用户工作模式
	uint16_t port;
	union{
		sockaddr_in guest;
		sockaddr_in serv;
	};

	char buffer[128];
	int rw_cur;

	std::mutex mut;
	std::condition_variable wait_data; // 等待数据传输完成
	User(): rw_cur(0){}
	void flush(){
		send(sockfd, buffer, rw_cur, 0);
	}
};
} // end namespace MidCHeck
#endif
