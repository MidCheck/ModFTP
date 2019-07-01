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
	InitMap(cmd_map);
}
} // end namespace MidCHeck
