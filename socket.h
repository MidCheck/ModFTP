/*************************************************************************
    > File Name: socket.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月04日 星期二 12时56分01秒
 ************************************************************************/
/* socket类，对不同平台的socket的封装
 * 线程安全型
 */


#ifdef _WIN32
	//define something for Windows (32-bit and 64-bit, this part is common)
	#ifdef _WIN64
		//define something for Windows (64-bit only)
	#else
		//define something for Windows (32-bit only)
		#include<WinSock2.h>
		#pragmacomment(lib, "ws2_32.lib")
   #endif
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
    #else
    #   error "Unknown Apple platform"
    #endif
#elif __ANDROID__
    // android
#elif __linux__
  // linux
#include<sys/types.h>
#include<sys/socket.h> // 创建socket	
#include<arpa/inet.h>　// ip地址转换函数
#include<netinet/in.h> // 主机字节序与网络字节序转换
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif
#ifndef IPPROTO_TCP
#define IPPROTO_TCP 0
#endif

#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif

#include<iostream>
#include<string>

namespace MidCHeck{
class Socket{
protected:
#ifdef __linux__
	int sock;
#elif _WIN32
	WSADATA wsaData;
	SOCKET sock;
#endif
	sockaddr_in sock_addr;
public:
	Socket() = delete;
	Socket(const string& ip, int port){
		#ifdef _WIN32
		WORD sockVersion = MAKEWORD(2, 2);
		try{
			if((err = WSAStartup(sockVersion, &wsaData)) != 0)
				throw runtime_error("初始化WSADATA失败!");
			else if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion)!= 2){
				WSACleanup();
				throw runtime_error("不支持当前版本的socket!");
			}
		}catch(runtime_error err){
			// 初始化失败
			std::cerr << err.what()
				<< "\n请检查当前WSADATA初始化" << std::endl;
		}
		#endif
		try{
_SOCKET_:
			if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
				throw runtime_error("socket创建出错!");
			sock_addr.sin_family = AF_INET;
			sock_addr.sin_port = htons(port);
			sock_addr.sin_addr.s_addr = inet_addr(ip);
		}catch(runtime_error err){
			// 处理socket创建失败
			std::cout << "socket创建失败,是否重新创建socket?[Y/y | N/n]" << std::flush;
			char flag;
			std::cin >> flag;
			if(flag == 'y' || flag == 'Y')
				goto _SOCKET_;
			else if(flag == 'n' || flag == 'N'){
				std::cerr << err.what() << std::endl;
				exit(1);
			}else{
				std::cerr << "未知输入,结束程序" << std::endl;
				exit(1);
			}
		}catch(...){
			throw "绑定ip失败!";
		}
	}
	~Socket(){
		#ifdef _WIN32
		closesocket(sock);
		WSACleanup();
		#elif __linux__
		close(sock);
		#endif
	}
	#ifdef __linux__
	friend int nonblocking(int);
	#endif
};
#ifdef __linux__
int setnonblocking(int fd){
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_OONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}
#endif
} // 命名空间MidCHeck结束
