/*************************************************************************
    > File Name: passwd.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年07月01日 星期一 22时28分13秒
 ************************************************************************/
#include "passwd.h"
namespace MidCHeck{
int mygetch()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
 
int getpasswd(char *passwd, int size)
{
    int c, n = 0;
    do
    {
        c = mygetch();
        if (c != '\n' && c != 'r' && c != 127)
        {
            passwd[n] = c;
            printf("*");
            n++;
        }
        else if ((c != '\n' | c != '\r') && c == 127)//判断是否是回车或则退格
        {
            if (n > 0)
            {
                n--;
                printf("\b \b");//输出退格
            }
        }
    }while (c != '\n' && c != '\r' && n < (size - 1));
    passwd[n] = '\0';//消除一个多余的回车
    return n;
}
} // end namespace MidCHeck
