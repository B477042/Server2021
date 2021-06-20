#pragma once

#include<iostream>
#include <string>
using namespace std;
/*
*	Ŭ���̾�Ʈ�� ������ �������� ����� ��Ŷ�Դϴ�.
*/
enum EPacketHeader
{
	/*
	*	Default Value
	*/
	Null = 0,
	/*
	*	send message client to server
		Ŭ���̾�Ʈ�� �ۼ��� �޽����� ������ �����ϴ�
	*/
	send_msg_CtoS,
	/*
	*	send message server to client
	*	������ Ŭ���̾�Ʈ���� �޽��� �߼��� ���� �ߴٰ� �˷��ݴϴ�
	*/
	send_msg_StoC,
	/*
	*	request read log client to server
	*	Ŭ���̾�Ʈ�� ������ �ִ� log�� �б� ���� �˷��޶�� ȣ���մϴ�.
	*
	*/
	req_read_log_CtoS,
	/*
	*	request read log server to client
	*	������ Ŭ���̾�Ʈ���� log ������ �����ݴϴ�.
	*/
	req_read_log_StoC


};
//�������� ������ ���� ���� ������
struct FStaticPacket
{
public:
	//�ļ����� �� �������� ��Ŷ�� ����
	int Length;
	//EPacketHeader Data
	int Header;

};
//���� ���� �����Ͱ� ����ִ� ��Ŷ
struct FDynamicPacket
{
public:
	FDynamicPacket()
	{

	}
	//�Ҹ��� �� �ڵ����� ���� ���ش�
	~FDynamicPacket()
	{
		delete CString;
	}
	//CString �迭 �ʱ�ȭ�� �������� ���ش�
	void InitCString(int Length)
	{
		CString = new char[Length];
		memset(CString, NULL, Length);
	}
	void ResetCString()
	{
		delete CString;
	}

	//CString
	char* CString;
};

