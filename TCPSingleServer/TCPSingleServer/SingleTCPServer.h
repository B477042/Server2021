#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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

//Ŭ���̾�Ʈ�� ����� ���� ���� ����� ����ü
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
	//���� ���� ����
	void createServerSocket();
	//Ŭ���̾�Ʈ accept
	void acceptSocket(SOCKET* sock);
	//�ش� Ŭ���̾�Ʈ�κ��� ������ ����
	bool receiveData(FClientSocket* cs);
	//�ش� Ŭ���̾�Ʈ�� ������ ����
	bool sendData(FClientSocket*cs);
	//���ϴ� ������ �߰�
	void addAditionalText(char* inputBuf,const char* text, int& retval);

	


	//������ Ŭ���̾�Ʈ�� �ְ� �޴� �ð��� ��Ȯ�� �˰� �;
	//���ͳݿ��� �˻��ؼ� ����ϴ� ���¸� �����߽��ϴ�
	//https://jhnyang.tistory.com/305
	void printCurrentTime();
private:
	//���߿� �þ�� ��찡 ������� vector�� �߽��ϴ�
	std::vector<SOCKET>ServerSockets;

	//�������� ����/����
	std::vector<FClientSocket*>ClientSockets;
	//Ŭ���̾�Ʈ ��
	const int n_Client=2;
};