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
#include <fstream>
#include <string>
#include <sstream>

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

class UMultiThreadServer;

//클라이언트와 통신을 위한 클라이언트 소켓의 정보 저장 구조체. 
typedef struct ClientSocket
{
public:
	SOCKET sock;
	SOCKADDR_IN addr;
	int retval;

	

}ClentsSocket;


//송수신 과정에서 사용되는 구조체. 
//*연결될 Client Socket과 이용하는 쓰레드 idx가 있다
typedef struct ConnectionData
{
public:
	//소멸 테스트
	//~FConnectionData() { cout << "\ngood bye" << endl; }


	UMultiThreadServer* Server;

	//통신하게 될 Thread의 idx
	int idx_Thread;
	//ClientSockets의 포인터
	ClentsSocket* idx_Sockets;
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




class UMultiThreadServer
{
public:

	UMultiThreadServer();

	
	//winsock 초기화 + 서버 소켓 생성, 크리티컬 섹션 초기화. 성공시 0 반환
	int initServer();

	//Wait Thread와 Accept Thread를 만들어준다
	int RunMultiThreadServer();
	//
	int CloseServer();


	//idx번째 통신 쓰레드를 만들어준다. 실패시 return false
	bool CreateCommunicationRoom(void* inputParam, int idx_t);
private:
	//서버 소켓 생성
	void createServerSocket();
	//========================================================
	//				쓰레드 관련 함수 
	

	//대기 쓰레드 함수
	static unsigned int WINAPI procWait(LPVOID lpParam);

	//Accept만 수행해주는 함수
	static unsigned int WINAPI procAccept(LPVOID lpParam);

	//클라이언트와 통신하는 과정의 함수
	static unsigned int WINAPI procCommunication(LPVOID lpParam);


	
	//========================================================
	//============ 쓰레드내부 통신 함수 ============
	//클라이언트 accept
	ClientSocket* acceptSocket(SOCKET* sock);
	//해당 클라이언트로부터 데이터 수신
	bool receiveData(ClientSocket* cs, CommunicationData* cd);
	//해당 클라이언트로 데이터 전송
	bool sendData(ClientSocket*cs, CommunicationData* cd);
	//원하는 문구를 추가
	void addAditionalText(char* inputBuf, const char* text, int& retval);






	//========================================================
	//==============Share Value 관련 함수===========
	//어떤 쓰레드에서든지 Share값이 변하면 호출된다. 변한 쉐어값을 모든 연결 클라이언트에게 보낸다.
	void syncShareValue();



	//========================================================
	//			File 처리 관련
	//	기록을 남길 파일을 불러옵니다. 파일 이름은 FileAddress에 적고
	//	
	void initFileStreamer();
	
	//	불러들인 파일을 닫습니다
	void closeFileStreamer();




	//========================================================
	//================ 기타 함수 ==================
	//클라이언트와 서버가 연결되면 클라이언트 정보를 서버에 추가해줍니다
	int addClient();
	//클라이언트와 연결 해제. 서버에서 정보를 지워줍니다
	int removeClient(int num);
	//txt file에 로그를 남깁니다 
	void writeLog(const char* Input);
	// 현재 시간을 확인, string으로 반환해줍니다
	string getCurrentTime_ToString();


private:	//Variable

	//나중에 늘어나는 경우가 있을까봐 vector로 했습니다
	vector<SOCKET>ServerSockets;

	//동적으로 생성/삭제
	vector<ClientSocket*>ClientSockets;
	//클라이언트 최대 수
	const int Max_Clients = 8;
	
	//응답없음 최대 대기시간 10초
	const DWORD Max_WaitTime = 10000;
	//응답 대기 타이머
	DWORD WaitTimer;
	//쓰레드 루프 전체 종료용
	bool bPower;

	//작업 쓰레드로 배치 가능한 슬롯 인덱스
	queue<int> RemainThreadSlot;
	unsigned short n_Client;
	//unsigned 



	DWORD dwThreadId[NUM_OF_THREAD];

	HANDLE hThread[NUM_OF_THREAD];

	//=======================================================
	//	파일 입출력 변수들
	// static으로 처리 안 한 이유. Thread 내부에서 Server 객체를 포인터로 가리킬 수 있어서 
	ifstream ReadFile;
	ofstream WriteFile;
	//	불러들일 파일 주소. 이 뒤에 실행 시간을 더 붙인 string을 만들어 이용합니다
	const  char* FileAddress = "test.txt";

	//공유 데이터 Share, 0으로 초기화
	static int Share;
	
	//========================================================

	//ProcAccept 내에서 여러 클라이언트가 동시에 접근하게 될 경우,
	//비어있는 Thread에 클라이언트의 Thread를 할 당하는 작업 중
	//값 접근에 대한 충돌이 일어날 수 있기에 동기화를 시켜줍니다. 
	CRITICAL_SECTION	hCS_ProcAccept;
	//Thread를 할당한 후 CleintSocket을 memeber variable인 ClientSockets에 넣어줄 때
	//먼저 처리된 Thread부터 넣어주기 위해서 동기화를 시켜줍니다.
	CRITICAL_SECTION	hCS_AcceptSocket;
	//클라이언트로 값을 받은 후 처리를 선착순으로 진행하기 위해서 동기화를 시킵니다.
	CRITICAL_SECTION	hcs_ReceiveData;
	//클라이언트와 연결을 해제하는 중 값 접근 충돌을 방지하기 위해서 동기화 시킵니다
	CRITICAL_SECTION	hCS_DeleteCS;
	//Log File에 작성할 때 순서대로 작성될 수 있게 동기화 시키는 목적으로 만들었습니다.
	CRITICAL_SECTION	hCS_FileAccess;

};

