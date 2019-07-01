/*************************************************************************
    > File Name: FTP_Client.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月29日 星期六 14时01分08秒
 ************************************************************************/
#include "FTP_Client.h"

namespace MidCHeck{
FTP_Client::FTP_Client(const char* ip, int port): Socket(ip, port){
	sockfd = sock;
	if(connect(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1)
		mcthrow("[-] 连接失败!");
	else{
		rw_cur = recv(sockfd, buffer, 128, 0);
		if(rw_cur != -1){
			buffer[rw_cur] = '\0';
			std::cout << buffer;
		}
	}
	InitMap(cmd_map);
	cmd_help[USER] = "开始新用户登录过程";
	cmd_help[PASS] = "发送密码";
	cmd_help[CWD] = "切换当前工作目录";
	cmd_help[CDUP] = "返回上层目录";
	cmd_help[QUIT] = "退出当前会话";
	cmd_help[REIN] = "重新开始一个用户";
	cmd_help[PORT] = "使用主动模式";
	cmd_help[PASV] = "使用被动模式";
	cmd_help[TYPE] = "设置传输文件类型";
	cmd_help[STRU] = "设置文件传输结构";
	cmd_help[MODE] = "设置传输模式";
	cmd_help[SIZE] = "返回文件大小";
	cmd_help[RETR] = "获取文件";
	cmd_help[STOR] = "存储文件";
	cmd_help[ALLO] = "保留足够空间";
	cmd_help[REST] = "重新开始";
	cmd_help[RNFT] = "重命名开始";
	cmd_help[RNTO] = "重命名为";
	cmd_help[ABOR] = "放弃先前的FTP服务命令和相关的传输的数据";
	cmd_help[DELE] = "删除指定文件";
	cmd_help[RMD] = "删除目录";
	cmd_help[MKD] = "新建目录";
	cmd_help[PWD] = "当前目录";
	cmd_help[LIST] = "列出文件";
	cmd_help[NLST] = "名字列表";
	cmd_help[SYST] = "操作系统类型";
	cmd_help[HELP] = "查看帮助";
	cmd_help[NOOP] = "空操作";
}

COMMAND FTP_Client::parse(){
	char *ptr = buffer;
	while(isspace(*ptr)) ++ptr;
	while(isspace(buffer[rw_cur - 1])) --rw_cur;
	int j = 0, blank_space = 0; 
	// 把输入分两部分，先把前一部分全大写，其次把帮助部分的参数全大写
	bool flag = false;
	for(char *i = ptr; i != &buffer[rw_cur]; ++i){
		if(*i == ' ') { 
			flag = true; 
			if(!blank_space) blank_space = j; // 记录首个空格位置
		}
		if(!flag)
			buffer[j++] = toupper(*i);
		else
			buffer[j++] = *i;
	}
	buffer[j] = '\0';
	rw_cur = j;

	if(blank_space != 0) buffer[blank_space] = '\0';
	
	std::unordered_map<std::string, COMMAND>::const_iterator it 
		= cmd_map.find(buffer);
	if(it == cmd_map.end()){
		std::cout << "-" << "非法指令:" << buffer << std::endl;
		return ERRCOMMAND;
	}

	if(blank_space != 0) buffer[blank_space] = ' ';
	history.push_back(buffer);
	return it->second;
}
void FTP_Client::CmdHelp(){
	//replace(buffer);
	char* ptr_sp = strstr(buffer, " ");
	if(ptr_sp == nullptr){
		std::cout << "支持如下命令:" << std::endl;
		std::cout 
			<< "CDUP CWD  DELE HELP LIST" << std::endl
			<< "MKD  NOOP PASS PASV PORT" << std::endl
			<< "PORT PWD  QUIT RETR RMD" << std::endl
			<< "SIZE STOR SYST USER" << std ::endl;
	}else{
		char *ptr = ptr_sp++;
		while(*(++ptr) != '\0') *ptr = toupper(*ptr); // 把help参数大写
		
		std::unordered_map<std::string, COMMAND>::const_iterator it_cmd
			= cmd_map.find(ptr_sp);
		if(it_cmd != cmd_map.end()){
			std::unordered_map<COMMAND, std::string>::const_iterator it
				= cmd_help.find(it_cmd->second);
			if(it != cmd_help.end()){
				std::cout << ptr_sp << "\t" << it->second << std::endl;
				return;
			}
		}
		std::cout << "-" << ptr_sp << " 非法帮助指令" << std::endl;
	}
}

void FTP_Client::CmdUser(){
	strcat(buffer, "\r\n");
	rw_cur += 2;
	send(sockfd, buffer, rw_cur, 0);
	memset(buffer, '\0', 128);
	if((rw_cur = recv(sockfd, buffer, 128, 0)) != -1){
		std::cout << buffer;
	}
	strcpy(buffer, "PASS ");
	rw_cur = sizeof("PASS ");
	std::cout << "password: " << std::flush;
	getpasswd(&buffer[rw_cur-1], 128 - rw_cur - 1);
	std::cout<<std::endl;
	rw_cur = strlen(buffer);
}

void FTP_Client::CmdList(){
	int ret = 0;
	uint16_t port = 0;
	char temp[128] = "PASV\r\n";
	send(sockfd, temp, 6, 0);
	memset(temp, '\0', 128);
	if((ret = recv(sockfd, temp, 128, 0)) == -1){
		mcthrow("[-] 数据接受失败");
	}else{
		std::cout << temp;
	}
	strcat(buffer, "\r\n");
	rw_cur += 2;
	send(sockfd, buffer, rw_cur, 0);
	memset(temp, '\0', 128);
	if((ret = recv(sockfd, temp, 128, 0)) == -1){
		mcthrow("[-] 数据接受失败");
	}
	std::cout << temp;
	char* ptr = strstr(temp, "(");
	if(ptr == nullptr) mcthrow("[-] 返回数据格式错误");
	parse_ip(++ptr, port);
	
	dsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = inet_addr(ptr);

	if(connect(dsockfd, (struct sockaddr*)&serv, sizeof(serv)) == -1)
		mcthrow("[-] 连接失败!");

	memset(buffer, '\0', 128);
	recv(sockfd, buffer, 128, 0);
	char* ptr_recv = replace(buffer);
	if(ptr_recv)
		std::cout << buffer << std::endl;

	memset(temp, '\0', 128);
	while((ret = recv(dsockfd, temp, 127, 0)) > 0){
		std::cout << temp;
		memset(temp, '\0', 127);
	}

	if(ptr_recv){
		while(char* ptr = replace(ptr_recv)){
			std::cout << ptr_recv << std::endl;
			ptr_recv = ptr;
		}
	}else{
		memset(buffer, '\0', 128);
		recv(sockfd, buffer, 128, 0);
		std::cout << buffer << std::endl;

		memset(buffer, '\0', 128);
		recv(sockfd, buffer, 128, 0);
		std::cout << buffer << std::endl;
	}
	close(dsockfd);
}
void FTP_Client::CmdRetr(){

}
void FTP_Client::CmdStor(){

}

void FTP_Client::start(){
	const char* bash = "ftp> ";
	while(true){
		std::cout << bash << std::flush;
		try{
			memset(buffer, '\0', 128);
			std::cin.getline(buffer, 128);
			rw_cur = strlen(buffer);
		}catch(...){
			std::cerr << "字符数太多，读取失败，重新输入" << std::endl;
			continue;
		}
		switch(parse()){
			case HELP: CmdHelp(); break;
			case LIST: CmdList(); break;
			case RETR: CmdRetr(); break;
			case STOR: CmdStor(); break;
			case QUIT: return;
			case USER: CmdUser();
			default:
				buffer[rw_cur++] = '\r';
				buffer[rw_cur++] = '\n';
				send(sockfd, buffer, rw_cur, 0);
				if((rw_cur = recv(sockfd, buffer, 128, 0)) != -1){
					std::cout << buffer;
				}
				break;
		}
	}
}

} // end namespace MidCHeck

int main(int argc, char** argv){
	if(argc < 2){
		std::cout << "参数太少" << std::endl
			<< "Usage: ftp [ip] [port]"
			<< std::endl;
		return 1;
	}
	std::cout << argv[0] << " " << argv[1] << " " << argv[2] << std::endl;
	MidCHeck::FTP_Client client(argv[1], atoi(argv[2]));
	client.start();
	return 0;
}
