/*************************************************************************
    > File Name: main.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月16日 星期日 19时54分41秒
 ************************************************************************/
#include<iostream>
#include "FTP_Server.h"
#include "debug.h"
using namespace MidCHeck;

namespace DataTest{
void query(const char* name, MidCHeck::Database* db){
	const MidCHeck::Tabusr* res = db->query(name);
	std::cout << "Result: " << name << std::endl;
	if(res == nullptr)
		std::cout << "No result" << std::endl;
	else
		std::cout << "Name: " << res->name << std::endl
			<< "Pass: " << res->passwd << std::endl
			<< "Home: " << res->home_path << std::endl
			<< std::endl;
}
class Test{
private:
	int i;
public:
	Test(int i): i(i){
		std::cout << "Test in " << i << std::endl;
	}
	void echo(){
		std::cout << "Test->" << i << std::endl;
	}
	~Test(){
		std::cout << "Test out " << i << std::endl;
	}
};
int main(int argc, char **argv){
	// test something
	Debug("Test 1");
	Debug("Test, %d", 2);
	Debug("Test %d %s", 3, "void");
	Debug("%s,%s",__DATE__,__TIME__);
	Debug("1" __DATE__ "-" __TIME__ "2");
	int num = 0;
	std::unordered_map<int, Test*> list;
	for(int i = 0; i < 10; ++i){
		Test *new_t2 = new Test(i);
		list[i] = new_t2;
	}
	std::unordered_map<int, Test*>::iterator it = list.find(2), it2 = list.find(5);
	if(it == list.end()) return 0;
	Test *ptr  = it->second;
	it->second->echo();
	list.erase(it);
	std::cout << "erase 2 before echo\n";
	ptr->echo();
	std::cout << "erase 2 after echo\n";
	std::cout << std::endl;
	std::cout << "erase 5, delete:\n";
	ptr = it2->second;
	delete it2->second;
	std::cout << "erase 5-1, after delete\n";
	list.erase(it2);
	std::cout << "erase 5-2, after erase\n";
	ptr->echo();
	std::cout << "erase 5-3, after erase ptr->echo\n";
	return 0;
}
} // end namespace MidCHeck
int main(int argc, char** argv){
	return DataTest::main(argc, argv);
}
