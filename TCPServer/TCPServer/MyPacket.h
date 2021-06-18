#pragma once

#include<iostream>
#include <string>
using namespace std;
/*
*	클라이언트와 서버가 공용으로 사용할 패킷입니다.
*/
enum EPacketHeader
{
	/*
	*	Default Value
	*/
	Null = 0,
	/*
	*	send message client to server
		클라이언트가 작성한 메시지를 서버로 보냅니다
	*/
	send_msg_CtoS,
	/*
	*	send message server to client
	*	서버가 클라이언트에게 메시지 발송이 성공 했다고 알려줍니다
	*/
	send_msg_StoC,
	/*
	*	request read log client to server
	*	클라이언트가 서버에 있는 log를 읽기 위해 알려달라고 호출합니다.
	*
	*/
	req_read_log_CtoS,
	/*
	*	request read log server to client
	*	서버가 클라이언트에게 log 내역을 보내줍니다.
	*/
	req_read_log_StoC


};
//선행으로 보내는 고정 길이 데이터
struct FStaticPacket
{
public:
	//후속으로 올 가변길이 패킷의 길이
	int Length;
	//EPacketHeader Data
	int Header;

};
//가변 길이 데이터가 들어있는 패킷
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



