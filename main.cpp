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
int main(){
	MidCHeck::Database* db = new MidCHeck::Database;
	db->add("wangsan", "123456");
	db->add("zhangsi", "1sda2");
	db->add("lisi", "3dsda$");
	db->add("huangs", "ed33");

	//query("zhangsi", db);
	//db->del("zhangsi");
	//query("zhangsi", db);
	Shardata* sd = Shardata::GetEntity();
	sd->db = db;

	FTP_Server ftp("127.0.0.1", 8021);
	ftp.start();

	delete db;
	return 0;
}
} // end namespace MidCHeck
int main(){
	return DataTest::main();
}
