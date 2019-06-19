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
	USER, PASS, CWD,  REIN, QUIT, PORT, PASV, TYPE, 
	STRU, MODE, SIZE, RETR, STOR, ALLO, REST, RNFT,
	RNTO, ABOR, DELE, RMD,  MKD,  PWD,  LIST, NLST, 
	SYST, HELP, NOOP, CDUP, ERRCOMMAND
} COMMAND;

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
	const std::string& GetRootPath(){ return dir_root; }
};
} // end namespace MidCHeck

#endif
