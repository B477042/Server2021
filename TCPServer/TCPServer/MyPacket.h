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
	FDynamicPacket(int Length = 0)
	{
		CString = new char[Length];
		memset(CString, NULL, Length);
	}
	~FDynamicPacket()
	{
		delete CString;
	}

	//CString
	char* CString;
};


#pragma pack(push, 8)
typedef struct FMyPacket
{
public:
	int Length;
	int Header;
	string Data;
	//const unsigned char EndChar = 0xff;
public:
	FMyPacket();
	~FMyPacket();
	unsigned int CalcLength();

}MyPacket;
#pragma pack(pop)



