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
		memset(CString, NULL, BUFSIZ);
	}

	void ResetCString()
	{
		memset(CString, NULL, BUFSIZ);
	}

	//CString, bufsiz 512
	char CString[BUFSIZ];
};

