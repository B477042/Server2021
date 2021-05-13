#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <chrono>
#include <limits>
#include <algorithm>
#include <thread>
using namespace std;
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

static void err_quit(const char* msg);
static void err_display(const char* msg);



class UTCPClient
{
public:
	UTCPClient();
	int RunClient();

	
	bool SendData(int&retval,SOCKET& sock, char* buf, int length, int flags);
	bool ReceiveData(int&retval, SOCKET& sock, char* buf, int length, int flags);




private:

	


	int recvn(SOCKET s, char* buf, int len, int flags);
	void addAditionalText(char* inputBuf,const char* text);
	void printCurrentTime();

	//Share Value를 읽어들입니다. 만약 못 찾으면 무한대 리턴
	int findShare(const char* buf);

	

};