/*************************************************************************
    > File Name: ftpd.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年07月03日 星期三 01时03分09秒
 ************************************************************************/
#include <fstream>
#include "FTP_Server.h"
using namespace MidCHeck;

static bool parse_conf(char* buffer, const char* key, char* value){
	if(strncmp(buffer, key, strlen(key))){ // 如果不是以key开头
		return false;
	}
	while(!isdigit(*buffer)) ++buffer;
	strcpy(value, buffer);
	strcat(value, "\0");
	return true;
}
static bool parse_conf(char* buffer, char* user, char* passwd, char* home){	
	while(*buffer == ' ') ++buffer;
	char *ptr = strstr(buffer, " ");
	if(ptr == nullptr) return false;
	strncpy(user, buffer, ptr-buffer);
	strcat(user, "\0");
	
	while(*(ptr+1) == ' ') ++ptr;
	buffer = ++ptr;
	ptr = strstr(buffer, " ");
	strncpy(passwd, buffer, ptr-buffer);
	strcat(user, "\0");

	while(*(ptr+1) == ' ') ++ptr;
	strcpy(home, ++ptr);
	if(ptr = strstr(home, " ")){
		*ptr = '\0';
	}else{
		strcat(home, "\0");
	}
	return true;
}


int main(int argc, char **argv){
	if(argc < 2){
		std::cout << "缺少参数" << std::endl
			<< "用法: server 配置文件" << std::endl;
		return 1;
	}
	std::ifstream conf_file(argv[1], std::ios::in);
	if(!conf_file.is_open()){
		std::cerr << "打开文件失败!" << std::endl;
		return 1;
	}
	Database* db = new Database;
	char buffer[256], ip[20], port[10];
	char name[32], passwd[64], home[128];
	bool gflag = false, uflag = false;
	while(!conf_file.eof()){
		conf_file.getline(buffer, 256);
		if(!strncmp(buffer, "[global]", 8)) gflag = true;
		if(!strncmp(buffer, "[user]", 6)) {uflag = true; gflag = false;}
		if(gflag){
			parse_conf(buffer, "ip", ip);
			parse_conf(buffer, "port", port);
		}
		if(uflag){
			if(parse_conf(buffer, name, passwd, home)){
				db->add(name, passwd, home);
			}
			memset(name, 0, 32);
			memset(passwd, 0, 64);
			memset(home, 0, 128);
		}
		memset(buffer, 0, 256);
	}

	Shardata* sd = Shardata::GetEntity();
	sd->db = db;
	FTP_Server ftp(ip, atoi(port));
	
	daemon(0,0); // 设置守护进程

	ftp.start();

	delete db;
	return 0;
}
