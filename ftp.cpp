/*************************************************************************
    > File Name: ftp.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年07月03日 星期三 01时01分06秒
 ************************************************************************/
#include "FTP_Client.h"

int main(int argc, char** argv){
	if(argc < 2){
		std::cout << "参数太少" << std::endl
			<< "Usage: ftp [ip] [port]"
			<< std::endl;
		return 1;
	}
	MidCHeck::FTP_Client client(argv[1], atoi(argv[2]));
	client.start();
	return 0;
}
