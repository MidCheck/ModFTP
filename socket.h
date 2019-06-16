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
// 创建socket	
#include<sys/socket.h> 
// ip地址转换函数
#include<arpa/inet.h> 
// 主机字节序与网络字节序转换
#include<netinet/in.h> 
#include<fcntl.h>
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
	Socket(const char* ip, int port){
		#ifdef _WIN32
		WORD sockVersion = MAKEWORD(2, 2);
		try{
			if((err = WSAStartup(sockVersion, &wsaData)) != 0)
				throw std::runtime_error("初始化WSADATA失败!");
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
		if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			throw std::runtime_error("socket创建出错!");
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_port = htons(port);
		sock_addr.sin_addr.s_addr = inet_addr(ip);
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
	friend int setnonblocking(int);
	#endif
};
#ifdef __linux__
int setnonblocking(int fd);
#endif
} // 命名空间MidCHeck结束
