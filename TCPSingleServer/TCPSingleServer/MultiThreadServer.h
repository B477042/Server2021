#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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
	//���� ���� ����
	void createServerSocket();


	//===============������ �Լ�=============
	//��� ������ �Լ�
	void procWait();
	//Accept�� �������ִ� �Լ�
	void procAccept();



	//============ �����峻�� ��� �Լ� ============
	//Ŭ���̾�Ʈ accept
	void acceptSocket(SOCKET* sock);
	//�ش� Ŭ���̾�Ʈ�κ��� ������ ����
	bool receiveData(FClientSocket* cs);
	//�ش� Ŭ���̾�Ʈ�� ������ ����
	bool sendData(FClientSocket*cs);
	//���ϴ� ������ �߰�
	void addAditionalText(char* inputBuf, const char* text, int& retval);


	void printCurrentTime();
private:
	//���߿� �þ�� ��찡 ������� vector�� �߽��ϴ�
	std::vector<SOCKET>ServerSockets;

	//�������� ����/����
	std::vector<FClientSocket*>ClientSockets;
	//Ŭ���̾�Ʈ �ִ� ��
	const int M_Clients = 8;
	
	//������� �ִ� ���ð� 1��
	const float M_waitTime = 60.0f;
	//���� ��� Ÿ�̸�
	float waitTimer;
	//������ ���� ��ü �����
	bool bPower;

};

