#include "MyPacket.h"



FMyPacket::FMyPacket()
{
	Data = "";
	Length = 0;
	Header = EPacketHeader::Null;
}


FMyPacket::~FMyPacket()
{
}


unsigned int FMyPacket::CalcLength()
{
	unsigned int str_len = 0;

	return 0;
}
