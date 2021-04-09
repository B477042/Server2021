#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
//#include <stdlib.h>
#include <iostream>
#include<time.h>
#include<chrono>
#include<vector>
#include<Windows.h>


#include "SingleTCPServer.h"
using namespace std;




class MultiThreadServer
{
public:

	MultiThreadServer();

	int RunServer();



private:
	//서버 소켓 생성
	void createServerSocket();


	//===============쓰레드 함수=============
	//대기 쓰레드 함수
	void procWait();
	//Accept만 수행해주는 함수
	void procAccept();



	//============ 쓰레드내부 통신 함수 ============
	//클라이언트 accept
	void acceptSocket(SOCKET* sock);
	//해당 클라이언트로부터 데이터 수신
	bool receiveData(FClientSocket* cs);
	//해당 클라이언트로 데이터 전송
	bool sendData(FClientSocket*cs);
	//원하는 문구를 추가
	void addAditionalText(char* inputBuf, const char* text, int& retval);


	void printCurrentTime();
private:
	//나중에 늘어나는 경우가 있을까봐 vector로 했습니다
	std::vector<SOCKET>ServerSockets;

	//동적으로 생성/삭제
	std::vector<FClientSocket*>ClientSockets;
	//클라이언트 최대 수
	const int M_Clients = 8;
	
	//응답없음 최대 대기시간 1분
	const float M_waitTime = 60.0f;
	//응답 대기 타이머
	float waitTimer;
	//쓰레드 루프 전체 종료용
	bool bPower;

};

