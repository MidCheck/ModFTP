/*************************************************************************
    > File Name: FTP_Command.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月12日 星期三 17时11分11秒
 ************************************************************************/
#include "FTP_Shardata.h"
#include <cstring>
namespace MidCHeck{

/*控制层*/
class Command{
protected:
	User* user;
public:
	Command(User* usr):user(usr){}
	virtual Usrstat process() = 0;
};
// 声明共享数据
//class Shardata;

class CmdUSER: public Command {
public:
	CmdUSER(User *usr):Command(usr){}
	Usrstat process(){
		char *buffer = &user->buffer[user->rw_cur];
		Shardata *sd = Shardata::GetEntity();
		std::cout << " [u] ready to qeury" << std::endl;
		std::cout << " [u] recv:" << buffer << std::endl;
		const char* ptr = strstr(buffer, "\n");
		char *buf = new char[ptr-buffer];
		memcpy(buf, buffer, ptr-buffer);
		const MidCHeck::Tabusr* db_usr = sd->db->query(buf);
		std::cout << " [u] query end" << std::endl;
		strcpy(user->buffer, "OK ");
		user->rw_cur = 2;
		
		if(db_usr != nullptr){
			this->user->name = db_usr->name;
			this->user->passwd = db_usr->passwd;
			this->user->path = this->user->home = db_usr->home_path;
			
			this->user->status = NOTLOGGED;
			this->user->auth = LEGAL;
			sd->AddUser(this->user->sockfd, this->user);
			
			return LEGAL;
		}
		delete[] buf;
		return this->user->auth = ILLEGAL;
	}
};
class CmdPASS: public Command{
public:
	CmdPASS(User *usr):Command(usr){}
	Usrstat process(){
		char *buffer = &this->user->buffer[user->rw_cur];
		std::cout << " [p] pass buf:[" << buffer << "]" << std::endl;
		if(this->user->auth == LEGAL){
			*strstr(buffer, "\n") = '\0';
			if(this->user->passwd == buffer){
				this->user->status = LOGGED;
				strcpy(this->user->buffer, "login success!");
				user->rw_cur = sizeof("login success!");
				return LEGAL;
			}
			strcpy(this->user->buffer, "login failed!");
			user->rw_cur = sizeof("login failed!");
			return ILLEGAL;
		}
		strcpy(this->user->buffer, "auth error!");
		user->rw_cur = sizeof("auth error!");
		return ILLEGAL;
	}
};
class CmdIllegal: public Command{
public:
	CmdIllegal(User *usr):Command(usr){}
	Usrstat process(){
		std::cout << "[-] illegal command" << std::endl;
		return ILLEGAL;
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
		/*
			case CWD:
				ptr_cmd = new CmdCWD(user);
				break;
			case REIN:
				ptr_cmd = new CmdREIN(user);
				break;
			case QUIT:
				ptr_cmd = new CmdQUIT(user);
				break;
			case POST:
				ptr_cmd = new CmdPOST(user);
				break;
			case PASV:
				ptr_cmd = new CmdPASV(user);
				break;
			case TYPE:
				ptr_cmd = new CmdTYPE(user);
				break;
			case STRU:
				ptr_cmd = new CmdSTRU(user);
				break;
			case MODE:
				ptr_cmd = new CmdMODE(user);
				break;
			case SIZE:
				ptr_cmd = new CmdSIZE(user);
				break;
			case RETR:
				ptr_cmd = new CmdRETR(user);
				break;
			case STOR:
				ptr_cmd = new CmdSTOR(user);
				break;
			case ALLO:
				ptr_cmd = new CmdALLO(user);
				break;
			case REST:
				ptr_cmd = new CmdREST(user);
				break;
			case RNFT:
				ptr_cmd = new CmdRNFT(user);
				break;
			case RNTO:
				ptr_cmd = new CmdRNTO(user);
				break;
			case ABOR:
				ptr_cmd = new CmdABOR(user);
				break;
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
			case NLST:
				ptr_cmd = new CmdNLST(user);
				break;
			case SYST:
				ptr_cmd = new CmdSYST(user);
				break;
			case HELP:
				ptr_cmd = new CmdHELP(user);
				break;
			case NOOP:
				ptr_cmd = new CmdNOOP(user);
				break;
		*/
			default:
				ptr_cmd = new CmdIllegal(user);
				break;
		}
		return ptr_cmd;
	}
};


} // end namespace MidCHeck
