#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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

//thread 2���� ����մϴ�
#define NUM_OF_THREAD 2
//0�� Thread�� receive �Ҵ�
#define Idx_Receive  0
//1�� �����忡 send �Ҵ�
#define  Idx_Send  1



class UTCPClient;
static void err_quit(const char* msg);
static void err_display(const char* msg);



//��������/�Լ� ȣ���� ���̱� ���ؼ� ���� ����ü
typedef struct ConnectionData
{
public:
	UTCPClient* Client;
	SOCKET Sock;

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



class UTCPClient
{
public:
	UTCPClient();
	int RunClient();

	




private:


	//����� �������̽� ������ �Լ�
	static unsigned int WINAPI procInteraction(LPVOID IpParam);
	//���� ������ �Լ�
	static unsigned int WINAPI procRecieve(LPVOID IpParam);
	//���ڿ��� ������ �����ϴ�
	bool writeMessage(SOCKET& sock, UTCPClient* client,int& retval);
	//�������� �α׸� ������ ���� ��û�մϴ�
	bool requestReadMessage(SOCKET& sock, UTCPClient* client, int& retval);
	


//	bool sendData(int&retval,SOCKET& sock, char* buf, int length, int flags);
//	bool receiveData(int&retval, SOCKET& sock, char* buf, int length, int flags);


	//int recvn(SOCKET s, char* buf, int len, int flags);
	void addAditionalText(char* inputBuf,const char* text);
	void printCurrentTime();


	//Buffer�� ���� ������� �� �����ݴϴ�.
	static void clearBuffer(char*buf);

	//Share Value�� �о���Դϴ�. ���� �� ã���� ���Ѵ� ����
	//static int findShare(const char* buf);
	
	//https://www.tutorialspoint.com/how-to-get-the-ip-address-of-local-computer-using-c-cplusplus
	static const char* getIPAdrress();




private:
	/*static char buf_Send[BUFSIZE];
	static char buf_Receive[BUFSIZE];*/

	//Share���� ������ true�� �ż� send proc���� Share ���� �߼��մϴ�. �߼� �� false
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