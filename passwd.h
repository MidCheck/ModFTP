/*************************************************************************
    > File Name: passwd.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年07月01日 星期一 22时32分31秒
 ************************************************************************/
#ifndef MIDCHECK_PASSWD_H
#define MIDCHECK_PASSWD_H
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

namespace MidCHeck{
extern "C"{
	int mygetch();
	int getpasswd(char*, int);
}

} // end namespace MidCHeck

#endif
