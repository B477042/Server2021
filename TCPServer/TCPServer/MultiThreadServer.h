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
#include <queue>

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

class MultiThreadServer;

//Ŭ���̾�Ʈ�� ����� ���� Ŭ���̾�Ʈ ������ ���� ���� ����ü. 
typedef struct FClientSocket
{
public:
	SOCKET sock;
	SOCKADDR_IN addr;
	int retval;
	char buf[BUFSIZE + 1] = "";
	

}ClentsSocket;


//�ۼ��� �������� ���Ǵ� ����ü. 
//*����� Client Socket�� �̿��ϴ� ������ idx�� �ִ�
typedef struct FCommunicationData
{
public:
	//�Ҹ� �׽�Ʈ
	//~FCommunicationData() { cout << "\ngood bye" << endl; }


	MultiThreadServer* Server;

	//����ϰ� �� Thread�� idx
	int idx_Thread;
	//ClientSockets�� ������
	ClentsSocket* idx_Sockets;
}CommunicationData;



class MultiThreadServer
{
public:

	MultiThreadServer();

	
	//winsock �ʱ�ȭ + ���� ���� ����, ũ��Ƽ�� ���� �ʱ�ȭ. ������ 0 ��ȯ
	int initServer();

	//Wait Thread�� Accept Thread�� ������ش�
	int RunMultiThreadServer();
	//
	int closeServer();


	//idx��° ��� �����带 ������ش�. ���н� return false
	bool createCommunicationRoom(void* inputParam, int idx_t);
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
	FClientSocket* acceptSocket(SOCKET* sock);
	//�ش� Ŭ���̾�Ʈ�κ��� ������ ����
	bool receiveData(FClientSocket* cs);
	//�ش� Ŭ���̾�Ʈ�� ������ ����
	bool sendData(FClientSocket*cs);
	//���ϴ� ������ �߰�
	void addAditionalText(char* inputBuf, const char* text, int& retval);


	//================ ��Ÿ �Լ� ==================
	//Ŭ���̾�Ʈ�� ���� �ż� Ŭ���̾�Ʈ�� ���õ� ������ ������Ʈ ���ݴϴ�.
	int addClient();
	//Ŭ���̾�Ʈ�� ���� ����.
	int removeClient(int num);


	//============== ���� �ð� Ȯ�� =============
	void printCurrentTime();
private:
	//���߿� �þ�� ��찡 ������� vector�� �߽��ϴ�
	std::vector<SOCKET>ServerSockets;

	//�������� ����/����
	vector<FClientSocket*>ClientSockets;
	//Ŭ���̾�Ʈ �ִ� ��
	const int M_Clients = 8;
	
	//������� �ִ� ���ð� 10��
	const DWORD M_waitTime = 10000;
	//���� ��� Ÿ�̸�
	DWORD waitTimer;
	//������ ���� ��ü �����
	bool bPower;

	//�۾� ������� ��ġ ������ ���� �ε���
	queue<int> remainThreadSlot;
	unsigned short n_Client;
	//unsigned 



	DWORD dwThreadId[NUM_OF_THREAD];

	HANDLE hThread[NUM_OF_THREAD];

	

	//���� ������ share, 0���� �ʱ�ȭ
	static int share;


	CRITICAL_SECTION   hCS_ProcAccept;
	CRITICAL_SECTION   hCS_AcceptSocket;
	CRITICAL_SECTION   hCS_ReceiveData;
	CRITICAL_SECTION   hCS_SendData;
	CRITICAL_SECTION   hCS_DeleteCS;
};

