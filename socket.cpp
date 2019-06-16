/*************************************************************************
    > File Name: socket.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月17日 星期一 00时16分17秒
 ************************************************************************/
#include "socket.h"

namespace MidCHeck{
#ifdef __linux__
int setnonblocking(int fd){
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}
#endif
} // end namespace MidCHeck
