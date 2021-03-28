#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
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


class USingleTCPServer
{
public:
	USingleTCPServer();

	int RunServer();

	int run2();
private:
	
	void createSocket();
	void acceptSocket(SOCKET& sock);
	
	void addAditionalText(char* inputBuf,const char* text, int& retval);

	



	void printCurrentTime();
private:
	vector<SOCKET&>ServerSockets;
	vector<SOCKET&>ClientSockets;
	vector<SOCKADDR_IN&>ClientsAddrs;
};