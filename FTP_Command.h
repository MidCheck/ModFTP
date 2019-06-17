/*************************************************************************
    > File Name: FTP_Command.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月12日 星期三 17时11分11秒
 ************************************************************************/
#include <cstring>
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
				fs::current_path(fs::path(user->path));
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
public:
	CmdCWD(User* usr):Command(usr) {}
	void process(){
		char* buf = &user->buffer[user->rw_cur];
		check(buf);
		fs::path p(buf);
		if(!fs::exists(p)){
			reply("431 no such directory\r\n");
			return;
		}
		if(!fs::is_directory(p)){
			user->rw_cur = sprintf(user->buffer, "%d %s is not a directory\r\n\0", 550, p.c_str());
			return;
		}
		//current_path(p);
		user->path = p.string();
		reply("200 working directory changed\r\n");
		return;
	}
};

class CmdMKD: public Command{
public:
	CmdMKD(User* usr): Command(usr){}
	void process(){
		char* buf = &user->buffer[user->rw_cur];
		check(buf);
		fs::path p(buf);
		if(fs::exists(p)){
			user->rw_cur = sprintf(user->buffer, "%d-\"%s\" directory already exists;\n%d taking no action.\r\n\0", 521, p.c_str(), 521);
			return;
		}
		if(fs::create_directory(p)){
			user->rw_cur = sprintf(user->buffer, "%d %s directory created\r\n\0", 257, (user->path + p.string()).c_str());
			return;
		}
		reply("550 create failed, unknow error!");
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
//			case REIN:
//				ptr_cmd = new CmdREIN(user);
//				break;
			case QUIT:
				ptr_cmd = new CmdQUIT(user);
				break;
//			case POST:
//				ptr_cmd = new CmdPOST(user);
//				break;
//			case PASV:
//				ptr_cmd = new CmdPASV(user);
//				break;
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
//			case RMD:
//				ptr_cmd = new CmdRMD(user);
//				break;
//			case MKD:
//				ptr_cmd = new CmdMKD(user);
//				break;
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
//			case NOOP:
//				ptr_cmd = new CmdNOOP(user);
//				break;
			default:
				ptr_cmd = new CmdIllegal(user);
				break;
		}
		return ptr_cmd;
	}
};


} // end namespace MidCHeck
