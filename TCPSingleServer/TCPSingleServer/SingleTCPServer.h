#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include<time.h>
#include<chrono>

#include<vector>
using namespace std;



#define SERVERPORT 9000
#define BUFSIZE    512
static void err_quit(const char* msg);
static void err_display(const char* msg);

typedef struct FClientSocket
{
public:
	SOCKET sock;
	SOCKADDR_IN addr;
	int retval;
	char buf[BUFSIZE + 1] = "";
}ClentsSocket;


class USingleTCPServer
{
public:
	USingleTCPServer();

	int RunServer();
	int run1();
	int run2();
private:
	
	void createSocket();
	void acceptSocket(SOCKET* sock);
	
	bool receiveData(FClientSocket* cs);
	bool sendData(FClientSocket*cs);
	void addAditionalText(char* inputBuf,const char* text, int& retval);

	//void clearArraies();



	void printCurrentTime();
private:
	vector<SOCKET>ServerSockets;
	vector<FClientSocket*>ClientSockets;
	//vector<SOCKET&>ClientSockets;
	//vector<SOCKADDR_IN&>ClientsAddrs;
};