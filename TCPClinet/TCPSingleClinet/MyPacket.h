#ifndef MY_PACKET_H_
#define MY_PACKET_H_




/*
*	클라이언트와 서버가 공용으로 사용할 패킷입니다.
*/
enum EPacketHeader
{
	/*
	*	Default Value
	*/
	Null=0,
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

# pragma pack (1)
template <typename TData>
class MyPacket
{

public:
	MyPacket();
	~MyPacket();
	unsigned int CalcLength();

public:
	unsigned int Length;
	EPacketHeader Header;
	TData Data;
	const unsigned char EndChar = 0xff;
};

#pragma pack(pop)

#endif // MY_PACKET_H_

