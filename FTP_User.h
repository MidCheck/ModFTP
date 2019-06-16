/*************************************************************************
    > File Name: FTP_User.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月12日 星期三 20时22分44秒
 ************************************************************************/
#include<string>
#include<vector>

namespace MidCHeck{
/*用户状态: 合法用户，非法用户，未登录，已登录，已退出*/
typedef enum {ILLEGAL, LEGAL, NOTLOGGED, LOGGED, QUITED} Usrstat;

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
	User(): rw_cur(1){}
	/*
	User(const char* nam, const char* pass, Usrstat au, Usrstat sta, const char* home = nullptr):
		name(nam), passwd(pass), auth(au), status(sta){
		Shardata* sd = Shardata::GetEntity();
		this->path = this->home = nullptr == home ? sd->GetRootPath() : home;
	} = delete;
	*/
};
} // end namespace MidCHeck
