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
#include <fstream>
#include <string>
#include <sstream>

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

class UMultiThreadServer;

//Ŭ���̾�Ʈ�� ����� ���� Ŭ���̾�Ʈ ������ ���� ���� ����ü. 
typedef struct ClientSocket
{
public:
	SOCKET sock;
	SOCKADDR_IN addr;
	int retval;

	

}ClentsSocket;


//�ۼ��� �������� ���Ǵ� ����ü. 
//*����� Client Socket�� �̿��ϴ� ������ idx�� �ִ�
typedef struct ConnectionData
{
public:
	//�Ҹ� �׽�Ʈ
	//~FConnectionData() { cout << "\ngood bye" << endl; }


	UMultiThreadServer* Server;

	//����ϰ� �� Thread�� idx
	int idx_Thread;
	//ClientSockets�� ������
	ClentsSocket* idx_Sockets;
}ConnectionData;

//�ۼ��� �������� ���Ǵ� ����ü
//�ۼ��� �ϰ� ���� �����͵��� ����ϴ�
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

	
	//winsock �ʱ�ȭ + ���� ���� ����, ũ��Ƽ�� ���� �ʱ�ȭ. ������ 0 ��ȯ
	int initServer();

	//Wait Thread�� Accept Thread�� ������ش�
	int RunMultiThreadServer();
	//
	int CloseServer();


	//idx��° ��� �����带 ������ش�. ���н� return false
	bool CreateCommunicationRoom(void* inputParam, int idx_t);
private:
	//���� ���� ����
	void createServerSocket();
	//========================================================
	//				������ ���� �Լ� 
	

	//��� ������ �Լ�
	static unsigned int WINAPI procWait(LPVOID lpParam);

	//Accept�� �������ִ� �Լ�
	static unsigned int WINAPI procAccept(LPVOID lpParam);

	//Ŭ���̾�Ʈ�� ����ϴ� ������ �Լ�
	static unsigned int WINAPI procCommunication(LPVOID lpParam);


	
	//========================================================
	//============ �����峻�� ��� �Լ� ============
	//Ŭ���̾�Ʈ accept
	ClientSocket* acceptSocket(SOCKET* sock);
	//�ش� Ŭ���̾�Ʈ�κ��� ������ ����
	bool receiveData(ClientSocket* cs, CommunicationData* cd);
	//�ش� Ŭ���̾�Ʈ�� ������ ����
	bool sendData(ClientSocket*cs, CommunicationData* cd);
	//���ϴ� ������ �߰�
	void addAditionalText(char* inputBuf, const char* text, int& retval);






	//========================================================
	//==============Share Value ���� �Լ�===========
	//� �����忡������ Share���� ���ϸ� ȣ��ȴ�. ���� ����� ��� ���� Ŭ���̾�Ʈ���� ������.
	void syncShareValue();



	//========================================================
	//			File ó�� ����
	//	����� ���� ������ �ҷ��ɴϴ�. ���� �̸��� FileAddress�� ����
	//	
	void initFileStreamer();
	
	//	�ҷ����� ������ �ݽ��ϴ�
	void closeFileStreamer();




	//========================================================
	//================ ��Ÿ �Լ� ==================
	//Ŭ���̾�Ʈ�� ������ ����Ǹ� Ŭ���̾�Ʈ ������ ������ �߰����ݴϴ�
	int addClient();
	//Ŭ���̾�Ʈ�� ���� ����. �������� ������ �����ݴϴ�
	int removeClient(int num);
	//txt file�� �α׸� ����ϴ� 
	void writeLog(const char* Input);
	// ���� �ð��� Ȯ��, string���� ��ȯ���ݴϴ�
	string getCurrentTime_ToString();


private:	//Variable

	//���߿� �þ�� ��찡 ������� vector�� �߽��ϴ�
	vector<SOCKET>ServerSockets;

	//�������� ����/����
	vector<ClientSocket*>ClientSockets;
	//Ŭ���̾�Ʈ �ִ� ��
	const int Max_Clients = 8;
	
	//������� �ִ� ���ð� 10��
	const DWORD Max_WaitTime = 10000;
	//���� ��� Ÿ�̸�
	DWORD WaitTimer;
	//������ ���� ��ü �����
	bool bPower;

	//�۾� ������� ��ġ ������ ���� �ε���
	queue<int> RemainThreadSlot;
	unsigned short n_Client;
	//unsigned 



	DWORD dwThreadId[NUM_OF_THREAD];

	HANDLE hThread[NUM_OF_THREAD];

	//=======================================================
	//	���� ����� ������
	// static���� ó�� �� �� ����. Thread ���ο��� Server ��ü�� �����ͷ� ����ų �� �־ 
	ifstream ReadFile;
	ofstream WriteFile;
	//	�ҷ����� ���� �ּ�. �� �ڿ� ���� �ð��� �� ���� string�� ����� �̿��մϴ�
	const  char* FileAddress = "test.txt";

	//���� ������ Share, 0���� �ʱ�ȭ
	static int Share;
	
	//========================================================

	//ProcAccept ������ ���� Ŭ���̾�Ʈ�� ���ÿ� �����ϰ� �� ���,
	//����ִ� Thread�� Ŭ���̾�Ʈ�� Thread�� �� ���ϴ� �۾� ��
	//�� ���ٿ� ���� �浹�� �Ͼ �� �ֱ⿡ ����ȭ�� �����ݴϴ�. 
	CRITICAL_SECTION	hCS_ProcAccept;
	//Thread�� �Ҵ��� �� CleintSocket�� memeber variable�� ClientSockets�� �־��� ��
	//���� ó���� Thread���� �־��ֱ� ���ؼ� ����ȭ�� �����ݴϴ�.
	CRITICAL_SECTION	hCS_AcceptSocket;
	//Ŭ���̾�Ʈ�� ���� ���� �� ó���� ���������� �����ϱ� ���ؼ� ����ȭ�� ��ŵ�ϴ�.
	CRITICAL_SECTION	hcs_ReceiveData;
	//Ŭ���̾�Ʈ�� ������ �����ϴ� �� �� ���� �浹�� �����ϱ� ���ؼ� ����ȭ ��ŵ�ϴ�
	CRITICAL_SECTION	hCS_DeleteCS;
	//Log File�� �ۼ��� �� ������� �ۼ��� �� �ְ� ����ȭ ��Ű�� �������� ��������ϴ�.
	CRITICAL_SECTION	hCS_FileAccess;

};

