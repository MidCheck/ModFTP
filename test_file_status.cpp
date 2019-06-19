/*************************************************************************
    > File Name: test_file_status.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月19日 星期三 01时20分09秒
 ************************************************************************/
#include<iostream>
#include<string>
#include<boost/filesystem.hpp>
#include<pwd.h>
#include<grp.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

using namespace std;

namespace fs = boost::filesystem;

int main(int argc, char **argv){
	fs::path p(argv[1]);
	fs::file_status st(fs::status(p));
	cout << "type:" << st.type() << endl
		<< "perms:" << st.permissions() << endl;
	if(fs::others_read & st.permissions()){
		cout << "r" << endl;
	}else
		cout << "not r" <<endl;
	string s;
	s += '-';
	s += "rwxrwxrwx";
	cout << s << " " << p.filename() << endl;
	struct stat buf;
	struct passwd *my_info;
	struct group *grp;
	if(stat(argv[1], &buf) == -1){
		fprintf(stderr, "line:%d", __LINE__);
		perror("stat");
		exit(1);
	}
	my_info = getpwuid(buf.st_uid);
	grp =getgrgid(buf.st_gid);
	printf("文件所有者的信息\n");
	printf( "my name = [%s]\n", my_info->pw_name );
	printf( "my passwd = [%s]\n", my_info->pw_passwd );
	printf( "my uid = [%d]\n", my_info->pw_uid );
	printf( "my gid = [%d]\n", my_info->pw_gid );
	printf( "my gecos = [%s]\n", my_info->pw_gecos);
	printf( "my dir = [%s]\n", my_info->pw_dir );
	printf( "my shell = [%s]\n", my_info->pw_shell );
	printf("文件所属组的信息:");
	printf("group name =[%s]\n",grp->gr_name);
	printf("group passwd = [%s]\n",grp->gr_passwd);
	printf("组识别码 = [%d]\n",grp->gr_gid);
	printf("组成员账号 = [%s]\n",*(grp->gr_mem));
	printf("时间 = [%.12s]\n", 4 + ctime(&buf.st_mtime));
	printf("链接数 = [%d]\n", buf.st_nlink);
	
	return 0;
}
