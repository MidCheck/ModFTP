/*************************************************************************
    > File Name: 1.cpp
    > Author: MidCHeck
    > Mail: MidCheck@foxmail.com 
    > Created Time: Mon 20 May 2019 02:43:03 PM CST
 ************************************************************************/
#include<boost/thread.hpp>
#include<boost/bind.hpp>
#include<boost/asio.hpp>
#include<iostream>

using namespace boost::asio;
io_service service;
void func(int i){
	std::cout << "func called, i = " << i << std::endl;
}

void worker_thread(){
	service.run();
}

int main(int argc, char **argv){
	io_service::strand strand_one(service), strand_two(service);
	for(int i = 0; i < 5; ++i){
		service.post(strand_one.wrap(boost::bind(func,i)));
	}
	for(int i = 5; i < 10; ++i){
		service.post(strand_two.wrap(boost::bind(func, i)));
	}
	boost::thread_group threads;
	for(int i = 0; i < 3; ++i){
		threads.create_thread(worker_thread);
	}
	boost::this_thread::sleep(boost::posix_time::millisec(500));
	threads.join_all();
	return 0;
}
