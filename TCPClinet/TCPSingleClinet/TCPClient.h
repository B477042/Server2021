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
#include <Windows.h>
#include <process.h>
#include <tchar.h>
using namespace std;
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

//thread 2개를 사용합니다
#define NUM_OF_THREAD 2
//0번 Thread에 receive 할당
#define Idx_Receive  0
//1번 쓰레드에 send 할당
#define  Idx_Send  1



static void err_quit(const char* msg);
static void err_display(const char* msg);

static CRITICAL_SECTION hCritical;

class UTCPClient
{
public:
	UTCPClient();
	int RunClient();

	




private:


	//Input으로 socket 넘겨줄 것
	static unsigned int WINAPI procSend(LPVOID IpParam);
	static unsigned int WINAPI procRecieve(LPVOID IpParam);

	bool sendData(int&retval,SOCKET& sock, char* buf, int length, int flags);
	bool receiveData(int&retval, SOCKET& sock, char* buf, int length, int flags);


	int recvn(SOCKET s, char* buf, int len, int flags);
	void addAditionalText(char* inputBuf,const char* text);
	void printCurrentTime();


	//Buffer에 적힌 내용들을 다 지워줍니다.
	static void clearBuffer(char*buf);

	//Share Value를 읽어들입니다. 만약 못 찾으면 무한대 리턴
	static int findShare(const char* buf);
	



private:
	static char buf_Send[BUFSIZE];
	static char buf_Receive[BUFSIZE];

	//share값이 들어오면 true가 돼서 send proc에서 Share 값을 발송합니다. 발송 후 false
	static bool bIsNewMessage;



	DWORD dwThreadId[NUM_OF_THREAD];

	HANDLE hThread[NUM_OF_THREAD];

	
};