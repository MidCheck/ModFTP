/*************************************************************************
    > File Name: debug.h
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年07月05日 星期五 09时11分41秒
 ************************************************************************/
#ifndef MIDCHECK_DEBUG_H
#define MIDCHECK_DEBUG_H
#include <stdio.h>
#include <stdarg.h>
namespace MidCHeck{
inline int MCDebug(const char* fname, const unsigned long line,const char *func, const char *format, ...){
	va_list ptr_arg;
	int count = 0;
	count = printf("[D] " __TIME__ "%s->%s->%lu: ", fname, func, line);
	va_start(ptr_arg, format); // 获取可变参数列表
	fflush(stdout);
	count += vfprintf(stderr, format, ptr_arg);
	va_end(ptr_arg);
	count += printf("\n");
	return count;
}

#ifdef DEBUG
#define Debug(format, args...) MCDebug(__FILE__, __LINE__, __FUNCTION__, format, ##args)
#else
#define Debug
#endif

} // end namespace MidCHeck
#endif
