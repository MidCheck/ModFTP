/*************************************************************************
    > File Name: test.cpp
    > Author: MidCHeck
    > Mail: midcheck@foxmail.com 
    > Created Time: 2019年06月17日 星期一 23时00分59秒
 ************************************************************************/
#include<iostream>
#include<thread>
#include<mutex>
#include<unistd.h>
#include<condition_variable>
using namespace std;
std::mutex mtx;
std::condition_variable cv;
bool ready = true;

void do_print_id(int id){
	std::unique_lock<std::mutex> lck(mtx);
	cout << "[" << id << "] wait..." << endl;
	if(!ready){
		cv.wait(lck);
		cout << "[" << id << "] wait end..." << endl;
	}
	cout<< "thread " << id << endl;
}

void go(){
	std::unique_lock<std::mutex> lck(mtx);
	ready = false;
	cout<< " notify: sleep 2 seconds..." << endl;
	sleep(2);
	cv.notify_all(); 
}

int main(){
	std::thread threads[10];
	for(int i = 0; i < 10; ++i)
		threads[i] = std::thread(do_print_id, i);
	std::cout << " 10 threads ready to race, sleep 3 seconds..." << endl;
	sleep(3);
	go();
	for(auto &th: threads)
		th.join();
	return 0;
}
