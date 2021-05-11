#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <chrono>
#include <vector>
#include <array>
#include <thread>
#include <Windows.h>
#include <process.h>
#include <tchar.h>
#include <queue>

using namespace std;

//노트북 cpu가 8쓰레드라 8로 제한했습니다
#define NUM_OF_THREAD 8

//0번 쓰레드에 wait 할당
#define  Idx_Wait  0
//1번 쓰레드에 accept 할당
#define Idx_Accept  1
//2번 쓰레드부터 통신공간 할당
#define Idx_thread  2







#define SERVERPORT 9000
#define BUFSIZE    512
// 소켓 함수 오류 출력 후 종료
static void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
static void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

class MultiThreadServer;

//클라이언트와 통신을 위한 클라이언트 소켓의 정보 저장 구조체. 
typedef struct FClientSocket
{
public:
	SOCKET sock;
	SOCKADDR_IN addr;
	int retval;
	char buf[BUFSIZE + 1] = "";
	

}ClentsSocket;


//송수신 과정에서 사용되는 구조체. 
//*연결될 Client Socket과 이용하는 쓰레드 idx가 있다
typedef struct FCommunicationData
{
public:
	//소멸 테스트
	//~FCommunicationData() { cout << "\ngood bye" << endl; }


	MultiThreadServer* Server;

	//통신하게 될 Thread의 idx
	int idx_Thread;
	//ClientSockets의 포인터
	ClentsSocket* idx_Sockets;
}CommunicationData;



class MultiThreadServer
{
public:

	MultiThreadServer();

	
	//winsock 초기화 + 서버 소켓 생성, 크리티컬 섹션 초기화. 성공시 0 반환
	int initServer();

	//Wait Thread와 Accept Thread를 만들어준다
	int RunMultiThreadServer();
	//
	int closeServer();


	//idx번째 통신 쓰레드를 만들어준다. 실패시 return false
	bool createCommunicationRoom(void* inputParam, int idx_t);
private:
	//서버 소켓 생성
	void createServerSocket();

	//===============쓰레드 관련 함수=============
	

	//대기 쓰레드 함수
	static unsigned int WINAPI procWait(LPVOID lpParam);

	//Accept만 수행해주는 함수
	static unsigned int WINAPI procAccept(LPVOID lpParam);

	//클라이언트와 통신하는 과정의 함수
	static unsigned int WINAPI procCommunication(LPVOID lpParam);


	

	//============ 쓰레드내부 통신 함수 ============
	//클라이언트 accept
	FClientSocket* acceptSocket(SOCKET* sock);
	//해당 클라이언트로부터 데이터 수신
	bool receiveData(FClientSocket* cs);
	//해당 클라이언트로 데이터 전송
	bool sendData(FClientSocket*cs);
	//원하는 문구를 추가
	void addAditionalText(char* inputBuf, const char* text, int& retval);


	//================ 기타 함수 ==================
	//클라이언트와 연결 돼서 클라이언트와 관련된 정보를 업데이트 해줍니다.
	int addClient();
	//클라이언트와 연결 해제.
	int removeClient(int num);


	//============== 현재 시간 확인 =============
	void printCurrentTime();
private:
	//나중에 늘어나는 경우가 있을까봐 vector로 했습니다
	std::vector<SOCKET>ServerSockets;

	//동적으로 생성/삭제
	vector<FClientSocket*>ClientSockets;
	//클라이언트 최대 수
	const int M_Clients = 8;
	
	//응답없음 최대 대기시간 10초
	const DWORD M_waitTime = 10000;
	//응답 대기 타이머
	DWORD waitTimer;
	//쓰레드 루프 전체 종료용
	bool bPower;

	//작업 쓰레드로 배치 가능한 슬롯 인덱스
	queue<int> remainThreadSlot;
	unsigned short n_Client;
	//unsigned 



	DWORD dwThreadId[NUM_OF_THREAD];

	HANDLE hThread[NUM_OF_THREAD];

	

	//공유 데이터 share, 0으로 초기화
	static int share;


	CRITICAL_SECTION   hCS_ProcAccept;
	CRITICAL_SECTION   hCS_AcceptSocket;
	CRITICAL_SECTION   hCS_ReceiveData;
	CRITICAL_SECTION   hCS_SendData;
	CRITICAL_SECTION   hCS_DeleteCS;
};

