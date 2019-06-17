/*************************************************************************
    > File Name: FTP_User.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月12日 星期三 20时22分44秒
 ************************************************************************/
#include<string>
#include<vector>
#include<sys/socket.h>
#include<condition_variable>
#include<boost/filesystem.hpp>
namespace fs = boost::filesystem;
namespace MidCHeck{
/*语法错误, 合法用户，非法用户，未登录，已登录，已退出, 正在传输中*/
typedef enum {SYNTAXERR, ILLEGAL, LEGAL, NOTLOGGED, LOGGED, QUITED, TRANSING} Usrstat;

/*用户类，存放用户环境变量以及当前信息*/
class User{
public:
	int sockfd;
	std::string name;
	std::string passwd;
	std::string home; // 记录用户主目录
	std::string path; // 记录用户当前路径
	Usrstat auth;  // 记录用户权限
	Usrstat status; // 记录用户状态

	char buffer[128];
	int rw_cur;

	std::mutex mut;
	std::condition_variable wait_data; // 等待数据传输完成
	User(): rw_cur(1){}
	void flush(){
		send(sockfd, buffer, rw_cur, 0);
	}
};
} // end namespace MidCHeck
