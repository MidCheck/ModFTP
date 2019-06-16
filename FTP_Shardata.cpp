/*************************************************************************
    > File Name: FTP_Shardata.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月17日 星期一 00时12分44秒
 ************************************************************************/
#include "FTP_Shardata.h"

namespace MidCHeck{
std::mutex Shardata::m;
Shardata* Shardata::GetEntity(){
	m.lock();
	static Shardata obj;
	m.unlock();
	return &obj;
}
Shardata::Shardata(){
	cmd_map["USER"] = USER;
	cmd_map["PASS"] = PASS;
	cmd_map["CWD"] = CWD;
	cmd_map["QUIT"] = QUIT;
	cmd_map["REIN"] = REIN;
	cmd_map["POST"] = POST;
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
} // end namespace MidCHeck
