/*************************************************************************
    > File Name: FTP_Client.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月29日 星期六 14时01分08秒
 ************************************************************************/
#include "FTP_Client.h"

namespace MidCHeck{
FTP_Client::FTP_Client(const char* ip, int port): Socket(ip, port){
	sockfd = sock;
}
bool FTP_Client::parse(){
	if(!strncmp(buffer, "HELP", 4) || !strncmp(buffer, "help", 4)){
		char *ptr_sp = strstr(buffer, " ");
		if(ptr_sp == nullptr){
			std::cout << "支持如下命令:" << std::endl;
			std::cout 
				<< "CDUP CWD  DELE HELP LIST" << std::endl
				<< "MKD  NOOP PASS PASV PORT" << std::endl
				<< "PORT PWD  QUIT RETR RMD" << std::endl
				<< "SIZE STOR SYST USER" << std ::endl;
		}else{
			++ptr_sp;

		}
	}
}
void FTP_Client::start(){
	const char* bash = "ftp> ";
	while(true){
		std::cout << bash << std::flush;
		try{
			memset(buffer, '\0', 128);
			std::cin.getline(buffer, 128);
		}catch(...){
			std::cerr << "字符数太多，读取失败，重新输入" << std::endl;
			continue;
		}
	}
}
} // end namespace MidCHeck
