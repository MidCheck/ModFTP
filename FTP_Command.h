/*************************************************************************
    > File Name: FTP_Command.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月12日 星期三 17时11分11秒
 ************************************************************************/
#include <regex>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>

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
		if((user->dsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
			throw std::runtime_error("socket创建出错!");
		//serv.sin_family = AF_INET;
		serv.sin_port = htons(ntohs(serv.sin_port)-1);
		//serv.sin_addr.s_addr = inet_addr(serv_ip);
		if(bind(user->dsockfd, (struct sockaddr*)serv, sizeof(struct sockaddr_in)) == -1)
			throw std::runtime_error("绑定数据端口出错!");
	}
	void process(){
		char* buf = &user->buffer[&user->rw_cur];
		check(buf);
		int port;
		parse_ip(buf, &port); // buf里存放ip
		guest.sin_port = htons(port);
		if(strcmp(buf, guest_ip)){ // 如果ip不相等
			guest.sin_addr.s_addr = inet_addr(buf);
		}
		if(connect(user->dsockfd, (struct sockaddr*)guest, sizeof(guest)) == -1)
			throw std::runtime_error("连接数据端口失败");
		reply("200 connect ok!");
	}
};
class CmdPASV: public Command{
public:
	CmdPASV(User* usr): Command(usr){}
	void process(){
		int port = 0;
		struct sockaddr_in serv;
		socklen_t serv_len = sizeof(serv);
		getsockname(user->sockfd, (struct sockaddr*)&serv, &serv_len);
		/*检测数据socket是否有连接*/
		struct tcp_info info;
		int len = sizeof(info);
		getsockopt(user->dsockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*)&len);
		
		if(info.tcpi_state == TCP_CLOSE){
			close(user->dsockfd);
		}
		if((user->dsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
			throw std::runtime_error("socket创建出错!");
		uint16_t port = ntohs(serv.sin_port) - 1;
		serv.sin_port = htons(port);
		if(bind(user->dsockfd, (struct sockaddr*)serv, sizeof(struct sockaddr_in)) == -1)
			throw std::runtime_error("绑定数据端口出错!");
		listen(user->dsockfd, 5);
	}
};
class CmdLIST: public Command{
private:
	std::vector<std::string> list;
	void parse_file(fs::path& p, std::string& at){
		fs::file_status result = status(p);
		fs::file_type = result.type();
		fs::perms = result.permissions();
		// 先看文件类型
		if(file_type & fs::regular_file){
			at += '-';
		}else if(file_type & fs::directory_file){
			at += 'd';
		}else if(file_type & fs::sysmlink_file){
			at += 'l';
		}else if(file_type & fs::block_file){
			at += 'b';
		}else if(file_type & fs::character_file){
			at += 'c';
		}else if(file_type & fs::fifo_file){
			at += 'f';
		}else if(file_type & fs::socket_file){
			at += 's';
		}else{
			at += '_';
		}
		// 查看属主权限
		if(perms & fs::all_all){
			at += "rwxrwxrwx";
		}else{
			if(perms & fs::owner_all){
				at += "rwx";
			}else{
				if(perms & fs::owner_read){
					at += "r";
				}else{
					at += "-";
				}
				if(perms & fs::owner_write){
					at += "w";
				}else{
					at += "-";
				}
				if(perms & fs::owner_exe){
					at += "x";
				}else{
					at += "-";
				}
			}
			if(perms & fs::group_all){
				at += "rwx";
			}else{
				if(perms & fs::group_read){
					at += "r";
				}else{
					at += "-";
				}
				if(perms & fs::group_write){
					at += "w";
				}else{
					at += "-";
				}
				if(perms & fs::group_exe){
					at += "x";
				}else{
					at += "-";
				}
			}
			if(perms & fs::others_all){
				at += "rwx";
			}else{
				if(perms & fs::others_read){
					at += "r";
				}else{
					at += "-";
				}
				if(perms & fs::others_write){
					at += "w";
				}else{
					at += "-";
				}
				if(perms & fs::others_exe){
					at += "x";
				}else{
					at += "-";
				}
			}
		}
		try{
			struct stat file_stat;
			if((ret = stat(p.c_str(), &file_stat)) == -1)
				throw std::runtime_error("stat error");
			struct passwd *ptr = getwuid(file_stat.st_uid);
			struct group *str = getgrgid(file_stat.st_gid);
			at += ' ';
			at += to_string(file_stat.st_nlink);
			// 接下来把它转为ls的格式即可, string format
			// 加上属主和属组
			at += 
		}catch(std::runtime_error err){
			std::cerr << err.what() << " line:" << __LINE__ << std::endl;
		}
	}
	void parse_list(fs::path& p, std::vector<std::string>& list){
			if(fs::is_regular_file(p)){ // 如果是普通文件
				// 返回文件信息

			}else if(fs::is_directory(p)){ // 如果是目录,返回目录信息
				
			}else{

			}
	}
public:
	CmdLIST(User* usr): Command(usr){}
	void process(){
		char* buf = &user->buffer[user->rw_cur];
		check(buf);
		char* ptr = buf;
		fs::path p(user->path);
		if((ptr = strstr(ptr, " ")) != nullptr){
			if(*ptr == "/"){ p = "/"; ++ptr; }
			p /= ptr;
			if(!fs::exists(p)){
				reply("431 no such file or directory\r\n");
				return;
			}
		}

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
//			case LIST:
//				ptr_cmd = new CmdLIST(user);
//				break;
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
