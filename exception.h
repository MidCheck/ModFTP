/*************************************************************************
    > File Name: exception.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月20日 星期四 17时16分46秒
 ************************************************************************/
#ifndef MIDCHECK_EXCEPTION_H
#define MIDCHECK_EXCEPTION_H
#include<exception>
#include<string>

namespace MidCHeck{
#define mcthrow(x) throw MidCHeck::MCErr(x, __LINE__, __FILE__)
class MCErr: public std::exception{
private:
	std::string msg;
	const unsigned long line;
	std::string file;
public:
	const char* what() const throw(){
		return msg.c_str();
	}
	explicit MCErr(const std::string& s, const unsigned long line, const char* file): msg("[-] MCError"), line(line), file(file){
		msg += ", \"" + s + "\",";
		msg += "\n  -> file: " + this->file;
		msg += " ----> " + std::to_string(this->line);
	}
};
} // end namespace MidCHeck

#endif
