/*************************************************************************
    > File Name: FTP_User.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月12日 星期三 20时22分44秒
 ************************************************************************/
#include<string>
#include<vector>

namespace MidCHeck{
class User{
private:
	std::string name;
	std::string passwd;
	std::string home;
	std::vector<char> buffer;
public:
	User() = delete;
	User(string &name,  string& passwd){
		
	}
};
} // end namespace MidCHeck
