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
#include <sys/sendfile.h>
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
this->user->rw_cur = sizeof(x) - 1

/*
 * 检查x是否以'\n'或者'\r\n'结束
 * 必须在有user成员的类成员函数中使用
 */
#define check(x) if(!replace(x)){\
	reply("501 Syntax error in parameters or arguments\r\n");\
	this->user->status = SYNTAXERR;\
	return;\
}
/*
 * 检测权限
 */
#define IsLegal(x) if(x != LEGAL){\
	reply("332 illegal operation, need account login\r\n");\
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
			user->name = db_usr->name;
			user->passwd = db_usr->passwd;
			user->path = user->home = db_usr->home_path;
			
			user->status = NOTLOGGED;
			user->auth = LEGAL;
			sd->AddUser(user->sockfd, user);
			
			reply("331 Need password!\r\n");
			return;
		}

		user->auth = ILLEGAL;
		reply("332 has no this acount!\r\n");
	}
};
class CmdPASS: public Command{
public:
	CmdPASS(User *usr):Command(usr){}
	void process(){
		char *buffer = &user->buffer[user->rw_cur];
		if(user->auth == LEGAL){
			check(buffer);
			if(user->passwd == buffer){
				user->status = LOGGED;
				reply("230 User logged in, proceed\r\n");
				fs::current_path(fs::path(user->path));
				return;
			}
			user->auth = ILLEGAL;
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
		IsLegal(user->auth);
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
		IsLegal(user->auth);
		user->rw_cur = sprintf(user->buffer, "%d-\"%s\" directory already exists;\n%d taking no action.\r\n", 521, this->user->path.c_str(), 521);
		return;
	}
};

class CmdQUIT: public Command{
public:
	CmdQUIT(User* usr):Command(usr) {}
	void process(){
		// quit不用管用户是否合法
		Shardata *sd = Shardata::GetEntity();

		std::unique_lock<std::mutex> lk(user->mut);
		if(user->status == TRANSING){
			// 如果用户在传输数据,必须等待传输完成
			reply("200 waiting for data transmission to complete\r\n");
			user->flush();
			user->wait_data.wait(lk);
		}
		reply("221 closing connection\r\n");
		user->flush();
		close(user->dsockfd);
		shutdown(user->sockfd, SHUT_RDWR);
		Debug("QUIT关闭连接套接字:%d", user->sockfd);
		//close(user->sockfd);
		sd->DelUser(user->sockfd);
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
		IsLegal(user->auth);
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
			user->rw_cur = sprintf(user->buffer, "431 no such directory %s\r\n", p.c_str());
			return;
		}
		if(!fs::is_directory(p)){
			user->rw_cur = sprintf(user->buffer, "%d %s is not a directory\r\n", 550, p.c_str());
			return;
		}
		//current_path(p);
		user->path = p.string();
		user->rw_cur = sprintf(user->buffer, "200 working directory changed to %s\r\n", p.c_str());
	}
};

class CmdMKD: public Command{
public:
	CmdMKD(User* usr): Command(usr){}
	void process(){
		IsLegal(user->auth);
		char* buf = &user->buffer[user->rw_cur];
		fs::path p(user->path);
		if(buf[0] == '/'){ 
			p = "/";
			++buf;
		}
		check(buf);
		p /= buf;

		if(fs::exists(p)){
			user->rw_cur = sprintf(user->buffer, "%d-\"%s\" directory already exists;\n%d taking no action.\r\n", 521, p.c_str(), 521);
			return;
		}
		try{
			bool flag = strstr(buf, "/") == nullptr
				?fs::create_directory(p)
				:fs::create_directories(p);
			if(flag){
				user->rw_cur = sprintf(user->buffer, "%d %s directory created\r\n", 257, p.c_str());	
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
		IsLegal(user->auth);
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
			user->rw_cur = sprintf(user->buffer, "%d %lu files removed\r\n", 250, removed);
		}catch(...){
			reply("550 removed failed, unknown error\r\n");
		}
	}
};
class CmdCDUP: public Command{
public:
	CmdCDUP(User* usr): Command(usr){}
	void process(){
		IsLegal(user->auth);
		fs::path p(user->path);
		user->rw_cur = sprintf(user->buffer, "200 %s\r\n", p.parent_path().c_str());
	}
};
class CmdNOOP: public Command{
public:
	CmdNOOP(User *usr): Command(usr){}
	void process(){
		IsLegal(user->auth);
		reply("200 OK");
	}
};
/*
 * 在PORT模式只完成这几项工作:
 * 1. 如果当前客户第一次指定此模式，则创建套接字并绑定控制端口－１，尝试去连接客户
 *　　假定客户已经打开PORT指定的端口，则成功
 * 2. 如果当前客户重新指定此模式，则检测套接字是否被关闭，如果关闭回到第一步，
 *	　否则直接用此套接字去连接客户端
 * 3. 如果是由PASV模式切换而来，则检测套接字是否关闭，如果关闭，回到第一步，
 *	　如果没有关闭，检测是否可以绑定套接字端口，如果不能，直接连接
 *	-------------------------------------------------------------------------
 *	更改方案: 每当用户指定PORT模式,随机打开一个端口去连接用户
 */
class CmdPORT: public Command{
private:
	char guest_ip[20];
public:
	CmdPORT(User *usr): Command(usr){
		IsLegal(user->auth);
		socklen_t guest_len = sizeof(user->guest);
		getpeername(user->sockfd, (struct sockaddr*)&user->guest, &guest_len);
		inet_ntop(AF_INET, &user->guest.sin_addr, guest_ip, sizeof(guest_ip));
		try{
			if((user->dsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
				mcthrow("socket创建出错!");
		}catch(MCErr err){
			std::cerr << err.what() << std::endl;
			reply("421 can't create socket, close connection");
			user->flush();
			close(user->sockfd);
		}
	}
	void process(){
		IsLegal(user->auth);
		char* buf = &user->buffer[user->rw_cur];
		check(buf);
		parse_ip(buf, user->port); // buf里存放ip
		user->guest.sin_port = htons(user->port);
		
		if(strcmp(buf, guest_ip)){ // 如果ip不相等
			user->guest.sin_addr.s_addr = inet_addr(buf);
		}
		reply("200 ready for connect");
		user->mode = MODEPORT;
	}
};

/*检测数据socket是否有连接*/
//struct tcp_info info;
//int len = sizeof(info);
//getsockopt(user->dsockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*)&len);
//if(info.tcpi_state != TCP_CLOSE){
//	close(user->dsockfd);
//}

/* PASV模式:
 * 1. 在全局绑定监听地址
 * 2. 其它用户异步连接此端口传输数据
 */
class CmdPASV: public Command{
private:
	socklen_t serv_len;
public:
	CmdPASV(User* usr): Command(usr), serv_len(sizeof(user->serv)){ }
	void process(){
		Debug("user->auth: %d", user->auth);
		IsLegal(user->auth);
		if(user->mode == MODEPASV){ 
			Debug("重复绑定");
			reply("150 file status okay, will open data connection.\r\n");
			return;
		}
		getsockname(user->sockfd, (struct sockaddr*)&user->serv, &serv_len);
		user->port = ntohs(user->serv.sin_port) - 1;
		user->serv.sin_port = htons(user->port);

		if((user->dsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
			mcthrow("PASV socket创建出错!");
		try{
			if(bind(user->dsockfd, (struct sockaddr*)&user->serv, sizeof(struct sockaddr_in)) == -1)
				mcthrow("PASV 绑定数据端口出错!");
		}catch(MCErr e){
			std::cerr << e.what() << std::endl 
				<< "\t可能重复绑定，程序继续..." << std::endl;
		}

		try{
			if(listen(user->dsockfd, 5) == -1)
				mcthrow("[-] listen失败!");
			reply("150 file status okay, will open data connection.\r\n");
			//user->flush();
		}catch(MCErr err){
			std::cerr << err.what() << std::endl;
			std::cerr << "Port:" << user->port << std::endl;
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
		Debug("auth: %d", user->auth);
		IsLegal(user->auth);
		char* buf = &user->buffer[user->rw_cur];
		fs::path p(user->path);
		bool flag = false;
		if((buf[0] == '\r' && buf[1] == '\n') || buf[0] == '\n'){ flag = true; }
		if(*buf == '/'){ p = "/"; ++buf; }
		check(buf);
		p /= buf;
		Debug("p:%s", p.c_str());
		if(!fs::exists(p)){
			reply("550 no such file or directory\r\n");
			return;
		}
		std::vector<std::string> list;
		parse_list(p, list, flag);
		if(list.size() < 1){
			reply("550 no such file\r\n");
			return;
		}
		std::vector<std::string>::iterator it = ++list.begin();
		Debug("list size:%d", list.size());
		if(!flag){
			for(; it != list.end(); ++it){
				list[0] += "\n" + *it;
			}
		}else{
			// 如果是当前目录
			for(int i = 0; it != list.end(); ++it, ++i){
				if((i+1) % 5 == 0)
					list[0] += '\n' + *it;
				else
					list[0] += '\t' + *it;
			}
		}
		list[0] += "\r\n";

		if(user->mode == MODEPORT){
			// connected
			try{
				if(connect(user->dsockfd, (struct sockaddr*)&user->guest, sizeof(user->guest)) == -1)
					mcthrow("List 连接失败");
				if(send(user->dsockfd, list[0].c_str(), list[0].length(), 0) == -1)
					mcthrow("send list error");
				close(user->dsockfd);
			}catch(MCErr err){
				std::cerr << err.what() << std::endl
					<< "port:" << ntohs(user->guest.sin_port) << std::endl;
				reply("425 can't open data connection, please send PORT command before use LIST command\r\n");
				return;
			}
		}else if(user->mode == MODEPASV){
			// 进入PASV模式
			// 给客户端发送IP以及端口
			strcpy(user->buffer, "227 entering Passive Mode (");
			//user->rw_cur = sizeof("227 entering Passive Mode (") - 1;
			//inet_ntop(AF_INET, &user->serv.sin_addr, &user->buffer[user->rw_cur], 20);
			Shardata* sd = Shardata::GetEntity();
			strcat(user->buffer, sd->data_ip);
			user->rw_cur = strlen(user->buffer);
			user->rw_cur += sprintf(&user->buffer[user->rw_cur], ",%d,%d)\r\n", user->port / 256, user->port % 256);
			// replace all '.' to ',' 
			char *ptr_dot = user->buffer;
			while((ptr_dot = strstr(ptr_dot, ".")) && (*ptr_dot++ = ','));
			Debug("list reply:[%s]", user->buffer);
			user->flush();
			
			struct sockaddr_in client_address;
			socklen_t clen = sizeof(client_address);
			// 应设置为非阻塞//阻塞死了,整个连接断掉
			int conn = accept(user->dsockfd, (struct sockaddr*)&client_address, &clen);
			Debug("accept dsockfd:%d", user->dsockfd);
			reply("125 data connection already open, transfer starting.\r\n");
			user->flush();
			send(conn, list[0].c_str(), list[0].length(), 0);
			reply("250 request file action okay, completed.\r\n");
			user->flush();
			close(conn);
		}else{
			reply("226 need to specify mode.\r\n");
			return;
		}
		reply("226 closed data connection\r\n");
	}
};

// 实现下一个命令
class CmdRETR: public Command{
public:
	CmdRETR(User *usr): Command(usr){}
	void process(){
		IsLegal(user->auth);
		char* buf = &user->buffer[user->rw_cur];
		fs::path p(user->path);
		if(*buf == '/'){ p = "/"; ++buf; }
		check(buf);
		p /= buf;
		if(!fs::exists(p)){
			reply("550 no such file or directory\r\n");
			return;
		}
		if(fs::is_directory(p)){
			reply("450 not allow directory\r\n");
			return;
		}
		int filefd = 0;
		try{
			filefd = open(p.c_str(), O_RDONLY);
			if(filefd <= 0) mcthrow("can't open file!");
			//reply("150 file status okay, will open data connection\r\n");
			//user->flush();
		}catch(MCErr err){
			std::cerr << err.what() << std::endl;
			reply("450 can't open file!");
			return;
		}
		if(user->mode == MODEPORT){
			// connected
			try{
				if(connect(user->dsockfd, (struct sockaddr*)&user->guest, sizeof(user->guest)) == -1)
					mcthrow("连接失败");
				reply("125 data connection already open, transfer starting.\r\n");
				user->flush();
				if(sendfile(user->dsockfd, filefd, NULL, fs::file_size(p)) == -1)
					mcthrow("sendfile error");
				close(filefd);
				close(user->dsockfd);
			}catch(MCErr err){
				std::cerr << err.what() << std::endl;
				reply("425 can't open data connection, please send PORT command before use RETR command\r\n");
				return;
			}
		}else{
			// 进入PASV模式
			// 给客户端发送IP以及端口
			strcpy(user->buffer, "227 entering Passive Mode (");
			//user->rw_cur = sizeof("227 entering Passive Mode (") - 1;
			//inet_ntop(AF_INET, &user->serv.sin_addr, &user->buffer[user->rw_cur], 20);
			//strcat(user->buffer, _IP);
			Shardata* sd = Shardata::GetEntity();
			strcat(user->buffer, sd->data_ip);
			user->rw_cur = strlen(user->buffer);
			user->rw_cur += sprintf(&user->buffer[user->rw_cur], ",%d,%d)\r\n", user->port / 256, user->port % 256);
			// replace all '.' to ',' 
			char *ptr_dot = user->buffer;
			while((ptr_dot = strstr(ptr_dot, ".")) && (*ptr_dot++ = ','));
			user->flush();
			
			struct sockaddr_in client_address;
			socklen_t clen = sizeof(client_address);
			// 应设置为非阻塞//阻塞死了,整个连接断掉
			int conn = accept(user->dsockfd, (struct sockaddr*)&client_address, &clen);
			reply("125 data connection already open, transfer starting.\r\n");
			user->flush();
			try{
				if(sendfile(conn, filefd, NULL, fs::file_size(p)) == -1)
					mcthrow("sendfile error");
				reply("250 request file action okay, completed.\r\n");
				user->flush();
				close(filefd);
				close(conn);
			}catch(MCErr err){
				std::cerr << err.what() << std::endl;
				reply("421 can't sendfile, close connection\r\n");
				close(conn);
				return;
			}
		}
		reply("226 closed data connection\r\n");
	}
};
class CmdSTOR:public Command{
public:
	CmdSTOR(User *usr): Command(usr){}
	void process(){
		IsLegal(user->auth);
		char* buf = &user->buffer[user->rw_cur];
		fs::path p(user->path);
		if(*buf == '/'){ p = "/"; ++buf; }
		check(buf);
		p /= buf;
		bool  pip_flag = true;

		int filefd = 0;
		// 打开要存储的文件，接收传输内容并将其写入，设置相应权限
		// 根据相应用户，设置相应权限
		// 得到用户权限
		// get_user_mask() 0744
		//flag |= (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		// 创建一个管道，用于splice函数
		int pipefd[2];
		int ret = pipe(pipefd);
		int pipe_size = fpathconf(pipefd[0], _PC_PIPE_BUF);
		try{
			if(!fs::exists(p)){
				filefd = open(p.c_str(),
					O_CREAT|O_WRONLY,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			}else{
				filefd = open(p.c_str(), O_WRONLY | O_TRUNC); 
			}
			if(filefd <= 0) mcthrow("can't open file!");
			//reply("150 file status okay, will open data connection\r\n");
			//user->flush();
		}catch(MCErr err){
			std::cerr << err.what() << std::endl;
			reply("450 can't open file!");
			return;
		}
		if(user->mode == MODEPORT){
			// connected
			try{
				if(connect(user->dsockfd, (struct sockaddr*)&user->guest, sizeof(user->guest)) == -1)
					mcthrow("连接失败");
				reply("125 data connection already open, transfer starting.\r\n");
				user->flush();
				do{
					ret = splice(user->dsockfd, NULL, pipefd[1], NULL, pipe_size, SPLICE_F_MORE | SPLICE_F_MOVE);
					if(ret == -1) mcthrow("splice写入管道出错!");
					else if(ret == 0) break;
					ret = splice(pipefd[0], NULL, filefd, NULL, pipe_size, SPLICE_F_MORE | SPLICE_F_MOVE);
					if(ret == -1) mcthrow("splice从管道读入出错!");
				}while(ret && ret != -1);
				close(filefd);
				close(user->dsockfd);
			}catch(MCErr err){
				std::cerr << err.what() << std::endl;
				reply("425 can't open data connection, please send PORT command before use STOR command\r\n");
				return;
			}
		}else if(user->mode == MODEPASV){
			// 进入PASV模式
			// 给客户端发送IP以及端口
			strcpy(user->buffer, "227 entering Passive Mode (");
			//user->rw_cur = sizeof("227 entering Passive Mode (") - 1;
			//inet_ntop(AF_INET, &user->serv.sin_addr, &user->buffer[user->rw_cur], 20);
			//strcat(user->buffer, _IP);
			Shardata* sd = Shardata::GetEntity();
			strcat(user->buffer, sd->data_ip);
			user->rw_cur = strlen(user->buffer);
			user->rw_cur += sprintf(&user->buffer[user->rw_cur], ",%d,%d)\r\n", user->port / 256, user->port % 256);
			// replace all '.' to ',' 
			char *ptr_dot = user->buffer;
			while((ptr_dot = strstr(ptr_dot, ".")) && (*ptr_dot++ = ','));
			user->flush();
			
			struct sockaddr_in client_address;
			socklen_t clen = sizeof(client_address);
			// 应设置为非阻塞//阻塞死了,整个连接断掉
			int conn = accept(user->dsockfd, (struct sockaddr*)&client_address, &clen);
			reply("125 data connection already open, transfer starting.\r\n");
			user->flush();
			try{
				do{
					// 陷入阻塞
					ret = splice(conn, NULL, pipefd[1], NULL, pipe_size, SPLICE_F_MORE | SPLICE_F_MOVE);
					if(ret == -1) mcthrow("splice写入管道出错!");
					else if(ret == 0) break;
					ret = splice(pipefd[0], NULL, filefd, NULL, pipe_size, SPLICE_F_MORE | SPLICE_F_MOVE);
				}while(ret && ret != -1); // 如果小于管道大小，说明已传输完毕
				reply("250 request file action okay, completed.\r\n");
				user->flush();
				close(filefd);
				close(conn);
			}catch(MCErr err){
				std::cerr << err.what() << std::endl;
				reply("421 can't sendfile, close connection\r\n");
				close(conn);
				return;
			}
		}else{
			reply("226 need to specify mode.\r\n");
			return;
		}
		reply("226 closed data connection\r\n");
	}
}; 

class CmdDELE: public Command{
public:
	CmdDELE(User *usr): Command(usr){}
	void process(){
		IsLegal(user->auth);
		char* buf = &user->buffer[user->rw_cur];
		fs::path p(user->home);
		if(*buf == '/'){ p = "/"; ++buf; }
		check(buf);
		p /= buf;
		if(!fs::exists(p)){
			reply("550 no such this file or directory\r\n");
		}else{
			user->rw_cur = sprintf(user->buffer, "200 %lu files removed\r\n", fs::remove_all(p));
		}
	}
};

class CmdSIZE: public Command{
public:
	CmdSIZE(User *usr): Command(usr){}
	void process(){
		IsLegal(user->auth);
		char* buf = &user->buffer[user->rw_cur];
		fs::path p(user->home);
		if(*buf == '/'){ p = "/"; ++buf; }
		check(buf);
		p /= buf;
		if(!fs::exists(p)){
			reply("550 no such file or directory\r\n");
		}else{
			user->rw_cur = sprintf(user->buffer, "200 size: %lu, %s\r\n", fs::file_size(p), p.c_str());
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
			case SIZE:
				ptr_cmd = new CmdSIZE(user);
				break;
			case RETR:
				ptr_cmd = new CmdRETR(user);
				break;
			case STOR:
				ptr_cmd = new CmdSTOR(user);
				break;
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
			case DELE:
				ptr_cmd = new CmdDELE(user);
				break;
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
