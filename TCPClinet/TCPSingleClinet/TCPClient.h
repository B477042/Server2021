#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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

	//Share Value�� �о���Դϴ�. ���� �� ã���� ���Ѵ� ����
	int findShare(const char* buf);

	

};