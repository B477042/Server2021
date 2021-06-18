#include "MyPacket.h"



MyPacket::MyPacket()
{
	Data = "";
	Length = 0;
	Header = EPacketHeader::Null;
}


MyPacket::~MyPacket()
{
}


unsigned int MyPacket::CalcLength()
{
	unsigned int str_len = 0;

	return 0;
}
