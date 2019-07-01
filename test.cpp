/*************************************************************************
    > File Name: test.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年07月01日 星期一 20时57分00秒
 ************************************************************************/
#include<iostream>
#include<string>
#ifdef _WIN32
#include<conio.h>
#elif __linux__
#include<curses.h>
#endif

using namespace std;
int main(){
//	char buffer[128] = {0};
//	cin.getline(buffer, 128);
//	cout<<"["<<buffer<<"]"<<endl;
//	int i = 1;
//	while(i){
//		cin >> i;
//		switch(i){
//			case 1: cout << "1" << endl;break;
//			case 2: i = 3; cout << "2->3" << endl; continue;
//			case 3: cout << "3" << endl;break;
//			default:
//					cout << "default"<<endl;
//		}
//		cout << "continue" << endl;
//		if(i == 5) break;
//	}
//	initscr();
//	cout<<"请输入密码： "<<endl;
//	string password;
//	int i=0;
//	char ch;
//	while (1){
//		ch = getch();
//		if(ch == 13) break;
//		password+=ch;//string对象重载了+=
//		cout<<"*";
//
//	}
//	cout<<endl<<"输入完毕！您输入的是： "<< password <<endl;
//	endwin();
	cout<<sizeof("250\r\n")<<endl;
}
