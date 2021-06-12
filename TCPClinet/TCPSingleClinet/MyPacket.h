#ifndef MY_PACKET_H_
#define MY_PACKET_H_



/*
*	클라이언트와 서버가 공용으로 사용할 패킷입니다.
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
