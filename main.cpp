/*************************************************************************
    > File Name: main.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月16日 星期日 19时54分41秒
 ************************************************************************/
#include<iostream>
#include "FTP_Server.h"
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
int main(int argc, char **argv){
	// test something
	return 0;
}
} // end namespace MidCHeck
int main(int argc, char** argv){
	return DataTest::main(argc, argv);
}
