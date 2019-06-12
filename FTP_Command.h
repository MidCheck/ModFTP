/*************************************************************************
    > File Name: FTP_Command.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月12日 星期三 17时11分11秒
 ************************************************************************/

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

/*用户状态: 合法用户，非法用户，未登录，已登录，已退出*/
typedef enum {ILLEGAL, LEGAL, NOTLOGGED, LOGGED, QUITED} UserStatus;

class Command{
public:
	virtual UserStatus process(const char*) = 0;
};
class CmdUSER{
public:
	UserStatus process(const char* arg){
	
	}
};
class CommandFactory{
public:
	CommandFactory() = delete;
	Command* CreateCmd(COMMAND cmd){
		Command* ptr_cmd = nullptr;
		switch(cmd){
			case USER:
				ptr_cmd = new CmdUSER();
				break;
			case PASS:
				ptr_cmd = new CmdPASS();
				break;
			case CWD:
				ptr_cmd = new CmdCWD();
				break;
			case REIN:
				ptr_cmd = new CmdREIN();
				break;
			case QUIT:
				ptr_cmd = new CmdQUIT();
				break;
			case POST:
				ptr_cmd = new CmdPOST();
				break;
			case PASV:
				ptr_cmd = new CmdPASV();
				break;
			case TYPE:
				ptr_cmd = new CmdTYPE();
				break;
			case STRU:
				ptr_cmd = new CmdSTRU();
				break;
			case MODE:
				ptr_cmd = new CmdMODE();
				break;
			case SIZE:
				ptr_cmd = new CmdSIZE();
				break;
			case RETR:
				ptr_cmd = new CmdRETR();
				break;
			case STOR:
				ptr_cmd = new CmdSTOR();
				break;
			case ALLO:
				ptr_cmd = new CmdALLO();
				break;
			case REST:
				ptr_cmd = new CmdREST();
				break;
			case RNFT:
				ptr_cmd = new CmdRNFT();
				break;
			case RNTO:
				ptr_cmd = new CmdRNTO();
				break;
			case ABOR:
				ptr_cmd = new CmdABOR();
				break;
			case DELE:
				ptr_cmd = new CmdDELE();
				break;
			case RMD:
				ptr_cmd = new CmdRMD();
				break;
			case MKD:
				ptr_cmd = new CmdMKD();
				break;
			case PWD:
				ptr_cmd = new CmdPWD();
				break;
			case LIST:
				ptr_cmd = new CmdLIST();
				break;
			case NLST:
				ptr_cmd = new CmdNLST();
				break;
			case SYST:
				ptr_cmd = new CmdSYST();
				break;
			case HELP:
				ptr_cmd = new CmdHELP();
				break;
			case NOOP:
				ptr_cmd = new CmdNOOP();
				break;
			default:
				ptr_cmd = new CmdIllegal();
				break;
		}
		return ptr_cmd;
	}
};


} // end namespace MidCHeck
