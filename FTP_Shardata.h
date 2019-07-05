/*************************************************************************
    > File Name: FTP_Shardata.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月16日 星期日 23时29分07秒
 ************************************************************************/
#ifndef FTP_SHARDATA_H
#define FTP_SHARDATA_H
#include "FTP.h"
#include "Database.h"
#include "FTP_User.h"

#include<mutex>
namespace MidCHeck{

/*数据层*/
class Shardata{
public:
	std::unordered_map<std::string, COMMAND> cmd_map;	
	std::unordered_map<int, User*> users;
	std::string dir_root;
	MidCHeck::Database* db;
	static std::mutex m;

	Shardata();
	static Shardata* GetEntity();
	bool AddUser(int fd, User *user){
		if(users.find(fd) != users.end()){
			users[fd] = user;
			return true;
		}
		return false;
	}
	bool DelUser(int fd){
		std::unordered_map<int, User*>::iterator it = users.find(fd);
		if(it != users.end()){
			#ifdef DEBUG
			User* ptr = it->second;
			Debug("Before delete User, auth: %d, name: %s", ptr->auth, ptr->name.c_str());
			#endif
			delete it->second;
			users.erase(it);
			#ifdef DEBUG
			Debug("After delete User, auth: %d, name: %s", ptr->auth, ptr->name.c_str());
			#endif
			return true;
		}
		return false;
	}
	const std::string& GetRootPath(){ return dir_root; }
};
} // end namespace MidCHeck

#endif
