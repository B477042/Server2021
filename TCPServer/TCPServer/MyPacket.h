#pragma once


#include <string>

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


class MyPacket
{

public:
	MyPacket();
	~MyPacket();
	unsigned int CalcLength();

public:

	std::string Data;
	unsigned int Length=0;
	EPacketHeader Header=EPacketHeader::Null;
	const unsigned char EndChar = 0xff;

};



