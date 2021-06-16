#ifndef MY_PACKET_H_
#define MY_PACKET_H_




/*
*	Ŭ���̾�Ʈ�� ������ �������� ����� ��Ŷ�Դϴ�.
*/
enum EPacketHeader
{
	/*
	*	Default Value
	*/
	Null=0,
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

