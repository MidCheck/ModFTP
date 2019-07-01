/*************************************************************************
    > File Name: FTP.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年07月02日 星期二 01时02分53秒
 ************************************************************************/
#include "FTP.h"

namespace MidCHeck{

void parse_ip(char *src, uint16_t& port){
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
} // end namespace MidCHeck
