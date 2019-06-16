/*************************************************************************
    > File Name: FTP_Shardata.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月16日 星期日 23时29分07秒
 ************************************************************************/
#ifndef FTP_SHARDATA_H
#define FTP_SHARDATA_H

#include "Database.h"
#include "FTP_User.h"

#include<mutex>
namespace MidCHeck{
/*访问控制命令*/
#define _USER	"USER"
#define _PASS	"PASS"
#define _CWD	"CWD"
#define _REIN	"REIN"
#define _QUIT	"QUIT"
/*传输参数命令*/
#define _PORT	"PORT"
#define _PASV	"PASV"
#define _TYPE	"TYPE"
#define _STRU	"STRU"
#define _MODE	"MODE"
/*FTP服务命令*/
#define _SIZE	"SIZE"
#define _RETR	"RETR"
#define _STOR	"STOR"
#define _ALLO	"ALLO"
#define _REST	"REST"
#define _RNFT	"RNFR"
#define _RNTO	"RNTO"
#define _ABOR	"ABOR"
#define _DELE	"DELE"
#define _RMD	"RMD"
#define _MKD	"MKD"
#define _PWD	"PWD"
#define _LIST	"LIST"
#define _NLST	"NLST"
#define _SYST	"SYST"
#define _HELP	"HELP"
#define _NOOP	"NOOP"

typedef enum { 
	USER, PASS, CWD,  REIN, QUIT, POST, PASV, TYPE, 
	STRU, MODE, SIZE, RETR, STOR, ALLO, REST, RNFT,
	RNTO, ABOR, DELE, RMD,  MKD,  PWD,  LIST, NLST, 
	SYST, HELP, NOOP, ERRCOMMAND
} COMMAND;

/*数据层*/
class Shardata{
public:
	std::unordered_map<std::string, COMMAND> cmd_map;	
//	std::unordered_map<const char*, User*> users;
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
	/*
	bool IsLegalUser(const char* user_name){
		return users.end() != users.find(user_name);
	}
	const std::string& GetPasswd(const char* user_name){
		std::unordered_map<string, string>::const_iterator user = users.find(user_name);
		return user != users.end() ? user->second->passwd : "";
	}
	*/
	const std::string& GetRootPath(){ return dir_root; }
	/*
	COMMAND GetCmd(std::vector<char>& buf, int& cur){
		char cmd[5] = {'\0'}, *ptr = strstr(&buf[1], ' ');
		cur = ptr - &buf[0];
		try{
			memcpy(cmd, &buf[1], cur - 1);
		}catch(...){
			for(int i = 0; i < cur - 1; ++i)
				cmd[i] = buf[1+i];
		}
		std::unordered_map<const char*, COMMAND>::const_iterator it 
			= cmd_map.find(cmd);
		return it != cmd_map.end() ? it->second : ERRCOMMAND;
	}
	*/
};
/*
bool Shardata::AddUser(int fd, User *user){
	if(users.find(fd) != users.end()){
		users[fd] = user;
		return true;
	}
	return false;
}
*/
} // end namespace MidCHeck

#endif
