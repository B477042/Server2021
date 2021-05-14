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

//thread 2���� ����մϴ�
#define NUM_OF_THREAD 2
//0�� Thread�� receive �Ҵ�
#define Idx_Receive  0
//1�� �����忡 send �Ҵ�
#define  Idx_Send  1



static void err_quit(const char* msg);
static void err_display(const char* msg);


typedef struct F
{

};


class UTCPClient
{
public:
	UTCPClient();
	int RunClient();

	




private:

	static unsigned int WINAPI procRecieve(LPVOID IpParam);
	static unsigned int WINAPI procSend(LPVOID IpParam);


	bool sendData(int&retval,SOCKET& sock, char* buf, int length, int flags);
	bool receiveData(int&retval, SOCKET& sock, char* buf, int length, int flags);


	int recvn(SOCKET s, char* buf, int len, int flags);
	void addAditionalText(char* inputBuf,const char* text);
	void printCurrentTime();


	//Buffer�� ���� ������� �� �����ݴϴ�.
	void clearBuffer(char*buf);
	kk
	//Share Value�� �о���Դϴ�. ���� �� ã���� ���Ѵ� ����
	int findShare(const char* buf);
	

private:
	char buf_Receive[BUFSIZE];
	char buf_Send[BUFSIZE];
	
};