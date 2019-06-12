/*************************************************************************
    > File Name: FTP_FILEOP.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月11日 星期二 15时07分49秒
 ************************************************************************/
#include<unordered_map>

namespace MidCHeck{

class FILE{
private:
	std::unordered_map<COMMAND, const char *> op;
	
};
} // end the namespace MidCHeck
