#ifndef MY_PACKET_H_
#define MY_PACKET_H_



/*
*	Ŭ���̾�Ʈ�� ������ �������� ����� ��Ŷ�Դϴ�.
*/
enum EPacketHeader
{
	/*
	*
	*/
	Null=0


};
template <typename TData>
class MyPacket
{
public:
	unsigned int Length;
	EPacketHeader Header;
	TData Data;
	const unsigned char EndChar=0xff;

};



#endif // MY_PACKET_H_
