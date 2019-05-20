/*************************************************************************
    > File Name: FTPServer.cpp
    > Author: MidCHeck
    > Mail: MidCheck@foxmail.com 
    > Created Time: Sat 18 May 2019 05:00:47 PM CST
 ************************************************************************/
#include<iostream>
#include <boost/asio.hpp>
using namespace boost::asio;
char buff[512];
int offset = 0;
size_t up_to_enter(const boost::system::error_code &, size_t bytes){
	for(size_t i = 0; i < bytes; ++i){
		if(buff[i+offset]=='\n')
			return 0;
	}
	return 1;
}
void on_read(const boost::system::error_code&, size_t){
	std::cout<<"Buff:"<<std::endl<<buff<<std::endl;
}
int main(){
	io_service service;
	ip::tcp::socket sock(service);
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"),80);
	sock.open(ip::tcp::v4());
	try{
		sock.connect(ep);
	}catch(boost::system::system_error const &e){
		std::cout<<"Warning: could not connect :"<< e.what() <<std::endl;
		exit(0);
	}
		std::cout<<"connect success!"<<std::endl;
	try{
		sock.write_some(buffer("GET /index.html\r\n"));
	}catch(boost::system::system_error const &e){
		std::cout<<"Warning: could not connect :"<< e.what() <<std::endl;
		exit(0);
	}
		std::cout<<"write success!"<<std::endl;
	try{
		async_read(sock, buffer(buff), up_to_enter, on_read);
		//size_t read = sock.async_read_some(buffer(buff));
		//std::cout<<"Buf["<<read<<"]:\n"<<buff<<std::endl;
	}catch(boost::system::system_error const &e){
		std::cout<<"Warning: could not connect :"<< e.what() <<std::endl;
		exit(0);
	}

	std::cout<<"read success!"<<std::endl<<"Buf:"<<std::endl<<buff<<std::endl;
	sock.shutdown(ip::tcp::socket::shutdown_receive);
	sock.close();
	
/*	using namespace boost::asio;
	io_context io;
	steady_timer t(io, chrono::seconds(5));
	t.wait();
	std::cout<<"HelloAsio"<<std::endl;
*/
/*	using namespace boost::asio;
	io_service service;
	ip::tcp::resolver resolver(service);
	ip::tcp::resolver::query query("www.baidu.com", "80");
	ip::tcp::resolver::iterator iter = resolver.resolve(query);
	ip::tcp::endpoint ep = *iter;
	std::cout<<ep.address().to_string() << std::endl;
*/
	return 0;
}
