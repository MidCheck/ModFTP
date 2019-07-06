/*************************************************************************
    > File Name: FTP.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年07月01日 星期一 20时25分18秒
 ************************************************************************/
#ifndef MIDCHECK_FTP_H
#define MIDCHECK_FTP_H
#include <unordered_map>
#include <iostream>
#include <string.h>
#include "debug.h"
namespace MidCHeck{
typedef enum { 
	USER, PASS, CWD,  REIN, QUIT, PORT, PASV, TYPE, 
	STRU, MODE, SIZE, RETR, STOR, ALLO, REST, RNFT,
	RNTO, ABOR, DELE, RMD,  MKD,  PWD,  LIST, NLST, 
	SYST, HELP, NOOP, CDUP, ERRCOMMAND
} COMMAND;

/*用户状态*/
typedef enum {
	/*语法错误, 非法用户，合法用户*/
	SYNTAXERR,	ILLEGAL,	LEGAL, 
	/*未登录，已登录，已退出*/
	NOTLOGGED,	LOGGED,		QUITED, 
	/*正在传输中, PORT模式，PASV模式*/
	TRANSING,	MODEPORT,	MODEPASV,
} Usrstat;

inline void InitMap(std::unordered_map<std::string, COMMAND>& cmd_map){
	cmd_map["USER"] = USER;
	cmd_map["PASS"] = PASS;
	cmd_map["CWD"] = CWD;
	cmd_map["CDUP"] = CDUP;
	cmd_map["QUIT"] = QUIT;
	cmd_map["REIN"] = REIN;
	cmd_map["PORT"] = PORT;
	cmd_map["PASV"] = PASV;
	cmd_map["TYPE"] = TYPE;
	cmd_map["STRU"] = STRU;
	cmd_map["MODE"] = MODE;
	cmd_map["SIZE"] = SIZE;
	cmd_map["RETR"] = RETR;
	cmd_map["STOR"] = STOR;
	cmd_map["ALLO"] = ALLO;
	cmd_map["REST"] = REST;
	cmd_map["RNFT"] = RNFT;
	cmd_map["RNTO"] = RNTO;
	cmd_map["ABOR"] = ABOR;
	cmd_map["DELE"] = DELE;
	cmd_map["RMD"] = RMD;
	cmd_map["MKD"] = MKD;
	cmd_map["PWD"] = PWD;
	cmd_map["LIST"] = LIST;
	cmd_map["NLST"] = NLST;
	cmd_map["SYST"] = SYST;
	cmd_map["HELP"] = HELP;
	cmd_map["NOOP"] = NOOP;	
}


/*
 * 为了使用默认参，使用inline
 * 功能: 把在buf里wd指定的子字符串的首位替换成'\0'，去掉结束标志
 */
inline char* replace(char *buf, const char* wd = "\n"){
	if(buf == nullptr) return nullptr;
	char *_ptr_ = strstr(buf, wd);
	if(_ptr_ == nullptr){
		return nullptr;
	}else if(*(_ptr_-1) == '\r'){
		*(_ptr_-1) = '\0';
	}else{
		*_ptr_ = '\0';
	}
	return ++_ptr_;
}

void parse_ip(char *src, uint16_t& port);

} // end namespace MidCHeck

#endif
