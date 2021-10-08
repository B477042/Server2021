exe 파일과 프로젝트 파일 원본을 넣었습니다. 
https://github.com/B477042/Server2021

기존에 만들었던 멀티스레드 프로젝트를 이용하여 기능을 추가해서 만들었습니다.

===================================================================
[패킷]
두 프로젝트에 공용으로 사용되는 패킷은 MyPacket.h에 정의 되어있습니다

FStaticPacket과 FDynamciPacket이라 정하였습니다.

둘로 나눈 이유는 가변길이 데이터인 FDynamciPacket을 따로 관리하고 싶기 때문입니다.

FStaticPacket의 구성은
_____________________________
| int | Length | int | Header|

총 8바이트로 Length는 뒤에 도착할 DynamcPacket의 길이를 담고 있습니다.
Header는 EPacketHeader의 값을 전달해서 어떻게 처리하면 되는지 알려줍니다.

FDyanmicPacket의 구성은
___________________________
| char | CString | BUFSIZE|

BUFSIZE 512만큼의 고정 배열입니다. 사용이 끝난 후 항상 memset을 통해 null 상태로 만들어 둡니다.
Length만큼의 정보만 보내고 받으면 512 바이트를 전부 보내지 않아도 됩니다.
=====================================================================
[패킷 헤더]
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
=============================================================================
[파일 입출력]
파일을 작성하는 ofstream 변수로 Write파일을 이용하고 메시지를 작성할 때 hCS_FileAccess란 임계영역에 들어가서
작성하게 됩니다. 파일을 작성하는 방법을 여러가지로 시도하고 싶어서 
sprintf를 이용해 char 배열로 메시지를 옮겨서 void writeLog(const char* Input) 함수를 통해 작성하거나
ofstream 변수에 << 연산자를 이용해 작성하는 방식을 사용했습니다. 

읽는 작업은 req_read_log_StoC 작업에서만 일어나고 클라이언트에 보낼 때 txt파일의 한줄씩 보내줍니다.

=============================================================================

