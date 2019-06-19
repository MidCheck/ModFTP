/*************************************************************************
    > File Name: FTP_Command.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月12日 星期三 17时11分11秒
 ************************************************************************/
#ifndef FTP_COMMAND_H
#define FTP_COMMAND_H

#include <regex>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <boost/format.hpp>
#include "FTP_Shardata.h"

namespace MidCHeck{
/*
 * 回复一个常量字符串,x应为常量
 */
#define reply(x) strcpy(this->user->buffer, x);\
this->user->rw_cur = sizeof(x)

/*
 * 为了使用默认参，使用inline
 * 功能: 把在buf里wd指定的子字符串的首位替换成'\0'，去掉结束标志
 */
inline bool replace(char *buf, const char* wd = "\n"){
	char *_ptr_ = strstr(buf, wd);
	if(_ptr_ == nullptr){
		return false;
	}else if(*(_ptr_-1) == '\r'){
		*(_ptr_-1) = '\0';
	}else{
		*_ptr_ = '\0';
	}
	return true;
}
/*
 * 检查x是否以'\n'或者'\r\n'结束
 * 必须在有user成员的类成员函数中使用
 */
#define check(x) if(!replace(x)){\
	reply("501 Syntax error in parameters or arguments");\
	this->user->status = SYNTAXERR;\
	return;\
}
/*控制层*/
class Command{
protected:
	User* user;
public:
	Command(User* usr):user(usr){}
	virtual void process() = 0;
};

class CmdIllegal: public Command{
public:
	CmdIllegal(User *usr):Command(usr){}
	void process(){
		reply("502 Command not implemented.\r\n");
	}
};

class CmdUSER: public Command {
public:
	CmdUSER(User *usr):Command(usr){}
	void process(){
		char *buffer = &user->buffer[user->rw_cur];
		check(buffer);

		Shardata *sd = Shardata::GetEntity();
		const MidCHeck::Tabusr* db_usr = sd->db->query(buffer);

		if(db_usr != nullptr){
			this->user->name = db_usr->name;
			this->user->passwd = db_usr->passwd;
			this->user->path = this->user->home = db_usr->home_path;
			
			this->user->status = NOTLOGGED;
			this->user->auth = LEGAL;
			sd->AddUser(this->user->sockfd, this->user);
			
			reply("331 Need password!\r\n");
			return;
		}

		this->user->auth = ILLEGAL;
		reply("332 has no this acount!\r\n");
	}
};
class CmdPASS: public Command{
public:
	CmdPASS(User *usr):Command(usr){}
	void process(){
		char *buffer = &this->user->buffer[user->rw_cur];
		if(this->user->auth == LEGAL){
			check(buffer);
			if(this->user->passwd == buffer){
				this->user->status = LOGGED;
				reply("230 User logged in, proceed\r\n");
				//std::cout << " [P] " << fs::current_path().c_str() << std::endl;
				fs::current_path(fs::path(user->path));
				//std::cout << " [P2]" << fs::current_path().c_str() << std::endl;
				return;
			}
			reply("530 login failed,wrong passwd!\r\n");
			return;
		}
		reply("332 need account!\r\n");
	}
};

class CmdSYST: public Command{
public:
	CmdSYST(User* usr):Command(usr){}
	void process(){
#ifdef WIN32
		reply("215 DOS/360 Window NT\r\n");
#elif __linux__
		reply("215 UNIX Type: L8\r\n");
#endif
	}
};

class CmdPWD: public Command{
public:
	CmdPWD(User* usr):Command(usr){}
	void process(){
		user->rw_cur = sprintf(user->buffer, "%d-\"%s\" directory already exists;\n%d taking no action.\r\n\0", 521, this->user->path.c_str(), 521);
		return;
	}
};

class CmdQUIT: public Command{
public:
	CmdQUIT(User* usr):Command(usr) {}
	void process(){
		std::unique_lock<std::mutex> lk(user->mut);
		if(user->status == TRANSING){
			// 如果用户在传输数据,必须等待传输完成
			reply("200 waiting for data transmission to complete\r\n");
			user->flush();
			user->wait_data.wait(lk);
		}
		reply("221 closing connection\r\n");
		return;
	}
};

class CmdCWD: public Command{
protected:
	void parse(char* buf, std::vector<std::string>& list){
		char *ptr = buf;
		while(*ptr != '\0' && (ptr = strstr(buf, "/"))){
			*ptr++ = '\0';
			list.push_back(buf);
			buf = ptr;
		}
		if(*buf != '\0')
			list.push_back(buf);
	}
public:
	CmdCWD(User* usr):Command(usr) {}
	void process(){
		char* buf = &user->buffer[user->rw_cur];
		fs::path p(user->path);
		if(buf[0] == '/') {
			p = "/";
			++buf;
		}
		check(buf);
		std::vector<std::string> path_list;
		parse(buf, path_list);

		for(std::string& path : path_list){
			if(path == ".."){
				p = p.parent_path();
			}else if(path == "."){
				continue;
			}else{
				p /= path;
			}
		}

		if(!fs::exists(p)){
			user->rw_cur = sprintf(user->buffer, "431 no such directory %s\r\n\0", p.c_str());
			return;
		}
		if(!fs::is_directory(p)){
			user->rw_cur = sprintf(user->buffer, "%d %s is not a directory\r\n\0", 550, p.c_str());
			return;
		}
		//current_path(p);
		user->path = p.string();
		user->rw_cur = sprintf(user->buffer, "200 working directory changed to %s\r\n\0", p.c_str());
	}
};

class CmdMKD: public Command{
public:
	CmdMKD(User* usr): Command(usr){}
	void process(){
		char* buf = &user->buffer[user->rw_cur];
		fs::path p(user->path);
		if(buf[0] == '/'){ 
			p = "/";
			++buf;
		}
		check(buf);
		p /= buf;

		if(fs::exists(p)){
			user->rw_cur = sprintf(user->buffer, "%d-\"%s\" directory already exists;\n%d taking no action.\r\n\0", 521, p.c_str(), 521);
			return;
		}
		try{
			bool flag = strstr(buf, "/") == nullptr
				?fs::create_directory(p)
				:fs::create_directories(p);
			if(flag){
				user->rw_cur = sprintf(user->buffer, "%d %s directory created\r\n\0", 257, p.c_str());	
				return;
			}
		}catch(...){
			reply("553 filenames are not allowed\r\n");
			return;
		}
		reply("550 create failed, unknow error!");
	}
};

class CmdRMD: public Command{
public:
	CmdRMD(User *usr): Command(usr){}
	void process(){
		char* buf = &user->buffer[user->rw_cur];
		fs::path p(user->path);
		if(buf[0] == '/') {
			p = "/";
			++buf;
		}
		check(buf);
		p /= buf;

		if(!fs::exists(p)){
			reply("431 no such directory\r\n");
			return;
		}
		try{
			uintmax_t removed = fs::remove_all(p);
			user->rw_cur = sprintf(user->buffer, "%d %d files removed\r\n\0", 250, removed);
		}catch(...){
			reply("550 removed failed, unknown error\r\n");
		}
	}
};
class CmdCDUP: public Command{
public:
	CmdCDUP(User* usr): Command(usr){}
	void process(){
		fs::path p(user->path);
		user->rw_cur = sprintf(user->buffer, "200 %s\r\n\0", p.parent_path().c_str());
	}
};
class CmdNOOP: public Command{
public:
	CmdNOOP(User *usr): Command(usr){}
	void process(){
		reply("200 OK");
	}
};
class CmdPORT: public Command{
private:
	struct sockaddr_in serv;
	struct sockaddr_in guest;
	char serv_ip[20];
	char guest_ip[20];
	void parse_ip(char *src, int& port){
		char *ptr = src;
		for(int i = 0; i < 3; ++i){
			if((ptr = strstr(ptr, ",")) != nullptr){
				*ptr++ = '.';
			}
		}
		if((ptr = strstr(ptr, ",")) != nullptr){
			*ptr++ = '\0';
		}
		char *next = strstr(ptr, ",");
		if(next != nullptr) {
			*next++ = '\0';
			std::cout << " [PORT] ->parse_ip: ptr[" << ptr << "] next[" << next <<"]" << std::endl;
			port = 256*atoi(ptr) + atoi(next);
		}else{
			port = atoi(ptr);
		}
	}
public:
	CmdPORT(User *usr): Command(usr){
		socklen_t serv_len = sizeof(serv), guest_len = sizeof(guest);
		getsockname(user->sockfd, (struct sockaddr*)&serv, &serv_len);
		getpeername(user->sockfd, (struct sockaddr*)&guest, &guest_len);
		//inet_ntop(AF_INET, &serv.sin_addr, serv_ip, sizeof(serv_ip));
		inet_ntop(AF_INET, &guest.sin_addr, guest_ip, sizeof(guest_ip));
		//data = Socket(serv_ip, ntohs(serv.sin_port)-1);
		try{
			if((user->dsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
				throw std::runtime_error("socket创建出错!");
		}catch(std::runtime_error err){
			std::cerr << err.what() << " line: " << __LINE__ << std::endl;
			exit(1);
		}
		//serv.sin_family = AF_INET;
		serv.sin_port = htons(ntohs(serv.sin_port)-1);
		
		std::cout << "Serv Port:" << ntohs(serv.sin_port) 
				<< " Clin Port:" << ntohs(guest.sin_port)
				<< " Port:" << ntohs(serv.sin_port) - 1
				<< std::endl;
		
		//serv.sin_addr.s_addr = inet_addr(serv_ip);
		try{
			if(bind(user->dsockfd, (struct sockaddr*)&serv, sizeof(struct sockaddr_in)) == -1)
				throw std::runtime_error("PORT 绑定数据端口出错!");
		}catch(std::runtime_error err){
			std::cerr << err.what() << " line: " << __LINE__ << std::endl;
			/*
			std::cerr << "Serv Port:" << ntohs(serv.sin_port) 
				<< " Clin Port:" << ntohs(guest.sin_port)
				<< " Port:" << ntohs(serv.sin_port) - 1
				<< std::endl;
			*/
			//exit(1);
		}
	}
	void process(){
		char* buf = &user->buffer[user->rw_cur];
		check(buf);
		int port;
		parse_ip(buf, port); // buf里存放ip
		guest.sin_port = htons(port);
		
		std::cout << " [PORT] port:" << port << std::endl;
		try{
			if(strcmp(buf, guest_ip)){ // 如果ip不相等
				guest.sin_addr.s_addr = inet_addr(buf);
			}
			if(connect(user->dsockfd, (struct sockaddr*)&guest, sizeof(guest)) == -1)
				throw std::runtime_error("PORT连接数据端口失败");
		}catch(std::runtime_error err){
			std::cerr << err.what() << " line: " << __LINE__ << std::endl;
			std::cerr << "Port:" << port << std::endl;
			//exit(1);
		}
		reply("200 connect ok!");
		user->mode = MODEPORT;
	}
};
class CmdPASV: public Command{
public:
	CmdPASV(User* usr): Command(usr){}
	void process(){
		struct sockaddr_in serv;
		socklen_t serv_len = sizeof(serv);
		getsockname(user->sockfd, (struct sockaddr*)&serv, &serv_len);
		uint16_t port = ntohs(serv.sin_port) - 1;
		serv.sin_port = htons(port);
		/*检测数据socket是否有连接*/
		struct tcp_info info;
		int len = sizeof(info);
		getsockopt(user->dsockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*)&len);
		if(info.tcpi_state != TCP_CLOSE){
			close(user->dsockfd);
		}
		try{
			if((user->dsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
				throw std::runtime_error("[-] PASV socket创建出错!");
			std::cout << " [PASV] Port:" << ntohs(serv.sin_port)-1 << std::endl;
			if(bind(user->dsockfd, (struct sockaddr*)&serv, sizeof(struct sockaddr_in)) == -1)
				throw std::runtime_error("[-] PASV绑定数据端口出错!");
		}catch(std::runtime_error e){
			std::cerr << e.what() << std::endl 
				<< "\t可能重复绑定，程序继续..." << std::endl;
		}
		strcpy(user->buffer, "227 entering Passive Mode (");
		user->rw_cur = sizeof("227 entering Passive Mode (") - 1;
		inet_ntop(AF_INET, &serv.sin_addr, &user->buffer[user->rw_cur], 20);
		user->rw_cur = strlen(user->buffer);
		user->rw_cur += sprintf(&user->buffer[user->rw_cur], ",%d,%d)\r\n\0", port / 256, port % 256);
		// replace all '.' to ',' 
		char *ptr_dot = user->buffer;
		while((ptr_dot = strstr(ptr_dot, ".")) && (*ptr_dot++ = ','));
		user->flush();

		// listened
		try{
			if(listen(user->dsockfd, 5) == -1)
				throw std::runtime_error("[-] listen失败!");
			reply("150 file status okay, will open data connection.\r\n");
			//user->flush();
		}catch(std::runtime_error err){
			std::cerr << err.what() << " line: " << __LINE__ << std::endl;
			std::cerr << "Port:" << ntohs(serv.sin_port)-1 << std::endl;
			reply("425 can't bind socket, please send PASV again\r\n");
		}
		user->mode = MODEPASV;
	}
};
class CmdLIST: public Command{
private:
	std::vector<std::string> list;
	const std::string parse_file(const fs::path& p){
		fs::file_status result = symlink_status(p);
		fs::perms perms= result.permissions();
		bool flag_d = false, flag_l = false, flag_x = false;
		std::string at;
		// 先看文件类型
		switch(result.type()){
			case fs::directory_file: at = 'd'; flag_d = true; break;
			case fs::symlink_file: at = 'l'; flag_l = true; break; 
			case fs::block_file: at = 'b'; break;
			case fs::character_file: at = 'c'; break;
			case fs::fifo_file: at = 'f'; break;
			case fs::socket_file: at = 's'; break;
			default: at = '-'; break;
		}
		// 查看属主权限
		at += perms & fs::owner_read ? "r" : "-";
		at += perms & fs::owner_write ? "w" : "-";
		at += perms & fs::owner_exe ? flag_x = true, "x" : "-";

		at += perms & fs::group_read ? "r" : "-";
		at += perms & fs::group_write ? "w" : "-";
		at += perms & fs::group_exe ? flag_x = true, "x" : "-";

		at += perms & fs::others_read ? "r" : "-";
		at += perms & fs::others_write ? "w" : "-";
		at += perms & fs::others_exe ? flag_x = true, "x" : "-";

		try{
			using boost::format;
			struct stat file_stat;
			if((stat(p.c_str(), &file_stat)) == -1)
				throw std::runtime_error("stat error");
			struct passwd *ptr = getpwuid(file_stat.st_uid);
			struct group *str = getgrgid(file_stat.st_gid);
			at += (format(" %4d") % file_stat.st_nlink).str();
			// 接下来把它转为ls的格式即可, string format
			// 加上属主和属组
			at += (format(" %s") % ptr->pw_name).str();
			at += (format(" %s") % str->gr_name).str();
			at += (format(" %8d") % file_stat.st_size).str();
			at += (format(" %.12s ") % (4 + ctime(&file_stat.st_mtime))).str();
			if(flag_l){
				at += (format("\033[47;36m%s\033[0m") % p.filename().c_str()).str();
			}else if(flag_d){
				at += (format("\033[40;34m%s\033[0m") % p.filename().c_str()).str();
			}else if(flag_x){
				at += (format("\033[40;32m%s\033[0m") % p.filename().c_str()).str();
			}else{
				at += p.filename().string();
			}
		}catch(std::runtime_error err){
			std::cerr << err.what() << " line:" << __LINE__ << std::endl;
		}catch(std::exception const& e){
			std::cerr << e.what() << "line:" << __LINE__ << std::endl;
		}
		return at;
	}
	void parse_list(fs::path& p, std::vector<std::string>& list, bool flag = false){
		// using boost::filesystem::directory_iterator;
			if(fs::is_directory(p)){ // 如果是目录,返回目录信息
				if(!flag){
					for(fs::directory_entry& x: fs::directory_iterator(p)){
						list.push_back(parse_file(x.path()));
					}
				}else{
					for(fs::directory_entry& x: fs::directory_iterator(p)){
						list.push_back(x.path().filename().string());
					}
				}
			}else{
				list.push_back(parse_file(p));
			}
	}
public:
	CmdLIST(User* usr): Command(usr){}
	void process(){
		char* buf = &user->buffer[user->rw_cur];
		fs::path p(user->path);
		bool flag = false;
		if((buf[0] == '\r' && buf[1] == '\n') || buf[0] == '\n'){ flag = true; }
		if(*buf == '/'){ p = "/"; ++buf; }
		check(buf);
		p /= buf;
		if(!fs::exists(p)){
			reply("431 no such file or directory\r\n");
			return;
		}
		std::vector<std::string> list;
		parse_list(p, list, flag);
		std::vector<std::string>::iterator it = ++list.begin();
		if(!flag){
			for(; it != list.end(); ++it){
				list[0] += "\n" + *it;
			}
		}else{
			// 如果是当前目录
			for(int i = 0; it != list.end(); ++it, ++i){
				if(i % 5 == 0) list[0] += '\n';
				list[0] += '\t' + *it;
			}
		}
		list[0] += "\r\n";
		/*检测数据socket是否有连接*/
		struct tcp_info info;
		int len = sizeof(info);
		getsockopt(user->dsockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*)&len);
		if(info.tcpi_state == TCP_CLOSE){
			close(user->dsockfd);
		}
		if(user->mode == MODEPORT){
			// connected
			try{
				if(send(user->dsockfd, list[0].c_str(), list[0].length(), 0) == -1)
					throw std::runtime_error("send list error");
			}catch(std::runtime_error err){
				std::cerr << err.what() << " line: " << __LINE__ << std::endl;
				reply("425 can't open data connection, please send PORT command\r\n");
				return;
			}
		}else{
			struct sockaddr_in client_address;
			socklen_t clen = sizeof(client_address);
			// 应设置为非阻塞//阻塞死了,整个连接断掉
			user->csockfd = accept(user->dsockfd, (struct sockaddr*)&client_address, &clen);
			reply("125 data connection already open, transfer starting.\r\n");
			user->flush();
			send(user->csockfd, list[0].c_str(), list[0].length(), 0);
			reply("250 request file action okay, completed.\r\n");
			user->flush();
			close(user->csockfd);
		}
		close(user->dsockfd);
		reply("226 closed data connection\r\n");
	}
};
class CommandFactory{
private:
	User* user;
public:
	CommandFactory(User* usr) { user = usr;}
	Command* CreateCmd(COMMAND cmd){
		Command* ptr_cmd = nullptr;
		switch(cmd){
			case USER:
				ptr_cmd = new CmdUSER(user);
				break;
			case PASS:
				ptr_cmd = new CmdPASS(user);
				break;
			case CWD:
				ptr_cmd = new CmdCWD(user);
				break;
			case CDUP:
				ptr_cmd = new CmdCDUP(user);
				break;
//			case REIN:
//				ptr_cmd = new CmdREIN(user);
//				break;
			case QUIT:
				ptr_cmd = new CmdQUIT(user);
				break;
			case PORT:
				ptr_cmd = new CmdPORT(user);
				break;
			case PASV:
				ptr_cmd = new CmdPASV(user);
				break;
//			case TYPE:
//				ptr_cmd = new CmdTYPE(user);
//				break;
//			case STRU:
//				ptr_cmd = new CmdSTRU(user);
//				break;
//			case MODE:
//				ptr_cmd = new CmdMODE(user);
//				break;
//			case SIZE:
//				ptr_cmd = new CmdSIZE(user);
//				break;
//			case RETR:
//				ptr_cmd = new CmdRETR(user);
//				break;
//			case STOR:
//				ptr_cmd = new CmdSTOR(user);
//				break;
//			case ALLO:
//				ptr_cmd = new CmdALLO(user);
//				break;
//			case REST:
//				ptr_cmd = new CmdREST(user);
//				break;
//			case RNFT:
//				ptr_cmd = new CmdRNFT(user);
//				break;
//			case RNTO:
//				ptr_cmd = new CmdRNTO(user);
//				break;
//			case ABOR:
//				ptr_cmd = new CmdABOR(user);
//				break;
//			case DELE:
//				ptr_cmd = new CmdDELE(user);
//				break;
			case RMD:
				ptr_cmd = new CmdRMD(user);
				break;
			case MKD:
				ptr_cmd = new CmdMKD(user);
				break;
			case PWD:
				ptr_cmd = new CmdPWD(user);
				break;
			case LIST:
				ptr_cmd = new CmdLIST(user);
				break;
//			case NLST:
//				ptr_cmd = new CmdNLST(user);
//				break;
			case SYST:
				ptr_cmd = new CmdSYST(user);
				break;
//			case HELP:
//				ptr_cmd = new CmdHELP(user);
//				break;
			case NOOP:
				ptr_cmd = new CmdNOOP(user);
				break;
			default:
				ptr_cmd = new CmdIllegal(user);
				break;
		}
		return ptr_cmd;
	}
};


} // end namespace MidCHeck


#endif
