/*************************************************************************
    > File Name: Database.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月16日 星期日 15时46分27秒
 ************************************************************************/
#include<string>
#include<unordered_map>
#include<iostream>

namespace MidCHeck{
/*模拟数据库*/
struct Tabusr{
	std::string name;
	std::string passwd;
	std::string home_path;
	Tabusr(){};
	Tabusr(const char* na, const char* pa, const char* ho):
		name(na), passwd(pa), home_path(ho)
	{}
};
class Database{
private:
	std::unordered_map<std::string,  MidCHeck::Tabusr> users;
public:
	const Tabusr* query(const char* name){
		std::unordered_map<std::string, MidCHeck::Tabusr>::const_iterator it = 
			users.find(name);
		std::cout << " [d] query in" << std::endl;
		return it != users.end() ? &it->second : nullptr;
	}
	void add(const char* nam, const char* pass, const char* home = "."){
		users[nam] = MidCHeck::Tabusr(nam, pass, home);
	}
	bool del(const char* name){
		const MidCHeck::Tabusr* re = query(name);
		if(re == nullptr) return false;
		users.erase(name);
		return true;
	}
};
} // end namespace MidCHeck
