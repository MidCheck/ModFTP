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
#include<boost/format.hpp>
//using boost::format;
//using namespace std;
using std::cout;
using std::endl;
//using std::string;
namespace fs = boost::filesystem;

int main(int argc, char **argv){
/*
	fs::path p(argv[1]);
	fs::file_status st(fs::status(p));
	cout << "type:" << st.type() << endl
		<< "perms:" << st.permissions() << endl;
	if(fs::others_read & st.permissions()){
		cout << "r" << endl;
	}else
		cout << "not r" <<endl;
	std::string s;
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
	std::string new_str = "-rwxrwxrwx";
	new_str += str(boost::format("%+2d ") % buf.st_nlink);
	new_str += str(boost::format("%s ") % my_info->pw_name);
	new_str += str(boost::format("%s ") % grp->gr_name);
	new_str += str(boost::format("%8.d ") % buf.st_size);
	new_str += str(boost::format("%.12s ") % (4 + ctime(&buf.st_mtime)));
	new_str += str(boost::format("%s") % argv[1]);
	cout << new_str << endl;
*/

	fs::path p(argv[1]);
	fs::file_status result = symlink_status(p);
	fs::file_type file_type= result.type();
	fs::perms perms= result.permissions();
	bool flag_d = false, flag_l = false, flag_x = false;
	std::string at;
	switch(result.type()){
		case fs::directory_file: at = 'd'; flag_d = true; break;
		case fs::block_file: at = 'b'; break;
		case fs::character_file: at = 'c'; break;
		case fs::fifo_file: at = 'f'; break;
		case fs::socket_file: at = 's'; break;
		case fs::symlink_file: at = 'l'; flag_l = true; break; // 不能识链接
		default: at = '-'; break;
	}
		
	std::cout << at << std::endl;
	if(is_symlink(p))
		std::cout << "是链接" << std::endl;
	else
		std::cout << "不是链接" << std::endl;
	// 先看文件类型
	if(file_type == fs::directory_file){
		flag_d = true;
		at = 'd';
	}else if(file_type == fs::symlink_file){
		flag_l = true;
		at = 'l';
	}else if(file_type == fs::block_file){
		at = 'b';
	}else if(file_type == fs::character_file){
		at = 'c';
	}else if(file_type == fs::fifo_file){
		at = 'f';
	}else if(file_type == fs::socket_file){
		at = 's';
	}else{
		at = '_';
	}
	// 查看属主权限
	//if(perms == fs::all_all){
	//	flag_x = true;
	//	at += "rwxrwxrwx";
	//	std:: cout << " [file]: " << p.string() << " is all_all" << std::endl;
//	}else{
	//	if(perms == fs::owner_all){
	//	std:: cout << " [file]: " << p.string() << " is owner_all" << std::endl;
	//		flag_x = true;
	//		at += "rwx";
	//	}else{
			if(perms & fs::owner_read){
				at += "r";
			}else{
				at += "-";
			}
			if(perms & fs::owner_write){
				at += "w";
			}else{
				at += "-";
			}
			if(perms & fs::owner_exe){
				flag_x = true;
				at += "x";
			}else{
				at += "-";
			}
	//	}

	//	if(perms & fs::group_all){
	//		flag_x = true;
	//		at += "rwx";
	//	}else{
			if(perms & fs::group_read){
				at += "r";
			}else{
				at += "-";
			}
			if(perms & fs::group_write){
				at += "w";
			}else{
				at += "-";
			}
			if(perms & fs::group_exe){
				flag_x = true;
				at += "x";
			}else{
				at += "-";
			}
	//	}

	//	if(perms & fs::others_all){
	//		flag_x = true;
	//		at += "rwx";
	//	}else{
			if(perms & fs::others_read){
				at += "r";
			}else{
				at += "-";
			}
			if(perms & fs::others_write){
				at += "w";
			}else{
				at += "-";
			}
			if(perms & fs::others_exe){
				flag_x = true;
				at += "x";
			}else{
				at += "-";
			}
	//	}
//	}
	return 0;
}
