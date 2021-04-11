#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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

using namespace std;

//��Ʈ�� cpu�� 8������� 8�� �����߽��ϴ�
#define NUM_OF_THREAD 8

//0�� �����忡 wait �Ҵ�
#define  Idx_Wait  0
//1�� �����忡 accept �Ҵ�
#define Idx_Accept  1
//2�� ��������� ��Ű��� �Ҵ�
#define Idx_thread  2







#define SERVERPORT 9000
#define BUFSIZE    512
// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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


//Ŭ���̾�Ʈ�� ����� ���� ���� ����� ����ü
typedef struct FClientSocket
{
public:
	SOCKET sock;
	SOCKADDR_IN addr;
	int retval;
	char buf[BUFSIZE + 1] = "";
}ClentsSocket;






class MultiThreadServer
{
public:

	MultiThreadServer();

	//int RunServer();
	//winsock �ʱ�ȭ + ���� ���� ����, ũ��Ƽ�� ���� �ʱ�ȭ. ������ 0 ��ȯ
	int initServer();

	//Wait Thread�� Accept Thread�� ������ش�
	int RunMultiThreadServer();
	//
	int closeServer();



	//idx��° ��� �����带 ������ش�. ���н� return false
	bool createCommunicationRoom(void* inputParam, int idx);
private:
	//���� ���� ����
	void createServerSocket();

	//===============������ ���� �Լ�=============
//��� ������ �Լ�
	static unsigned int WINAPI procWait(LPVOID lpParam);
	//Accept�� �������ִ� �Լ�
	static unsigned int WINAPI procAccept(LPVOID lpParam);
	//Ŭ���̾�Ʈ�� ����ϴ� ������ �Լ�
	static unsigned int WINAPI procCommunication(LPVOID lpParam);


	


	


	//============ �����峻�� ��� �Լ� ============
	//Ŭ���̾�Ʈ accept
	void acceptSocket(SOCKET* sock);
	//�ش� Ŭ���̾�Ʈ�κ��� ������ ����
	bool receiveData(FClientSocket* cs);
	//�ش� Ŭ���̾�Ʈ�� ������ ����
	bool sendData(FClientSocket*cs);
	//���ϴ� ������ �߰�
	void addAditionalText(char* inputBuf, const char* text, int& retval);


	//================ ��Ÿ �Լ� ==================
	////�����忡 �ڸ��� �ִ��� Ȯ���ϴ� �Լ�. �ڸ��� �ִٸ� true
	//bool checkThreadSpace();


	//============== ���� �ð� Ȯ�� =============
	void printCurrentTime();
private:
	//���߿� �þ�� ��찡 ������� vector�� �߽��ϴ�
	std::vector<SOCKET>ServerSockets;

	//�������� ����/����
	vector<FClientSocket*>ClientSockets;
	//Ŭ���̾�Ʈ �ִ� ��
	const int M_Clients = 8;
	
	//������� �ִ� ���ð� 1��
	const float M_waitTime = 60.0f;
	//���� ��� Ÿ�̸�
	float waitTimer;
	//������ ���� ��ü �����
	bool bPower;

	CRITICAL_SECTION   hCriticalSection;

	DWORD dwThreadId[NUM_OF_THREAD];
	//vector<DWORD> dwThreadId;
	HANDLE hThread[NUM_OF_THREAD];
	//vector<HANDLE> hThread;
	


};

