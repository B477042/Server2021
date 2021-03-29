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
//using namespace std;



#define SERVERPORT 9000
#define BUFSIZE    512
static void err_quit(const char* msg);
static void err_display(const char* msg);

//클라이언트와 통신을 위한 정보 저장용 구조체
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
	
private:
	//서버 소켓 생성
	void createServerSocket();
	//클라이언트 accept
	void acceptSocket(SOCKET* sock);
	//해당 클라이언트로부터 데이터 수신
	bool receiveData(FClientSocket* cs);
	//해당 클라이언트로 데이터 전송
	bool sendData(FClientSocket*cs);
	//원하는 문구를 추가
	void addAditionalText(char* inputBuf,const char* text, int& retval);

	


	//서버와 클라이언트가 주고 받는 시간을 정확히 알고 싶어서
	//인터넷에서 검색해서 출력하는 형태를 복사했습니다
	//https://jhnyang.tistory.com/305
	void printCurrentTime();
private:
	//나중에 늘어나는 경우가 있을까봐 vector로 했습니다
	std::vector<SOCKET>ServerSockets;

	//동적으로 생성/삭제
	std::vector<FClientSocket*>ClientSockets;
	//클라이언트 수
	const int n_Client=2;
};