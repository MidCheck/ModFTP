/*************************************************************************
    > File Name: ftp.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年07月03日 星期三 01时01分06秒
 ************************************************************************/
#include "FTP_Client.h"
#include <netdb.h>
int main(int argc, char** argv){
	if(argc < 3){
		std::cout << "参数太少" << std::endl
			<< "Usage: ftp [ip] [port]"
			<< std::endl;
		return 1;
	}
	char ip[32];
	if(!isdigit(argv[1][0]) && !isdigit(argv[1][1])){
		hostent* host = gethostbyname(argv[1]);
		strcpy(ip, inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
	}else{
		strcpy(ip, argv[1]);
	}
	MidCHeck::FTP_Client client(ip, atoi(argv[2]));
	client.start();
	return 0;
}
