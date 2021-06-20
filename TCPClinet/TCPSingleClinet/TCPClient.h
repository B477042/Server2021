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
#include "MyPacket.h"
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



class UTCPClient;
static void err_quit(const char* msg);
static void err_display(const char* msg);



//전역변수/함수 호출을 줄이기 위해서 만든 구조체
typedef struct ConnectionData
{
public:
	UTCPClient* Client;
	SOCKET Sock;

}ConnectionData;




//송수신 과정에서 사용되는 구조체
//송수신 하고 싶은 데이터들을 담습니다
typedef struct CommunicationData
{
public:

	char buf_Message[BUFSIZE + 1] = "";
	char buf_IP[BUFSIZE + 1] = "";
	int Share;

}CommunicationData;



class UTCPClient
{
public:
	UTCPClient();
	int RunClient();

	




private:


	//사용자 인터페이스 스레드 함수
	static unsigned int WINAPI procInteraction(LPVOID IpParam);
	//수신 스레드 함수
	static unsigned int WINAPI procRecieve(LPVOID IpParam);
	//문자열을 서버로 보냅니다
	bool writeMessage(SOCKET& sock, UTCPClient* client,int& retval);
	//서버에게 로그를 보내줄 것을 요청합니다
	bool requestReadMessage(SOCKET& sock, UTCPClient* client, int& retval);
	


//	bool sendData(int&retval,SOCKET& sock, char* buf, int length, int flags);
//	bool receiveData(int&retval, SOCKET& sock, char* buf, int length, int flags);


	//int recvn(SOCKET s, char* buf, int len, int flags);
	void addAditionalText(char* inputBuf,const char* text);
	void printCurrentTime();


	//Buffer에 적힌 내용들을 다 지워줍니다.
	static void clearBuffer(char*buf);

	//Share Value를 읽어들입니다. 만약 못 찾으면 무한대 리턴
	//static int findShare(const char* buf);
	
	//https://www.tutorialspoint.com/how-to-get-the-ip-address-of-local-computer-using-c-cplusplus
	static const char* getIPAdrress();




private:
	/*static char buf_Send[BUFSIZE];
	static char buf_Receive[BUFSIZE];*/

	//Share값이 들어오면 true가 돼서 send proc에서 Share 값을 발송합니다. 발송 후 false
	bool bIsNewMessage;
	
	//CommunicationData* communicationData;
	//Static value packet
	FStaticPacket* sPacket;
	//Dynmaic value packet
	FDynamicPacket* dPacket;
	//MyPacket* Packet;

	DWORD dwThreadId[NUM_OF_THREAD];

	HANDLE hThread[NUM_OF_THREAD];

	CRITICAL_SECTION hCritical;
};