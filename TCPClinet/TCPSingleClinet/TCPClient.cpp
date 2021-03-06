
#include"TCPClient.h"

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}



// 소켓 함수 오류 출력
void err_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}



UTCPClient::UTCPClient()
{
	
	InitializeCriticalSection(&hCritical);


	//printf("Size of struct : %d", sizeof(CommunicationData));
}




int UTCPClient::RunClient()
{
	int retval;
	
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	
	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// 데이터 통신에 사용할 변수
	ConnectionData* CD=new ConnectionData();
	CD->Client = this;
	CD->Sock = sock;
	
	//Packet 초기화
	sPacket = new FStaticPacket();
	dPacket = new FDynamicPacket();

	////IPTest
	//auto ip = getIPAdrress();
	//cout << "IP : " << ip << endl;
	//cout << "sizeof ip : " << sizeof(ip) << endl;
	//cout << "strlen : " << strlen(ip) << endl;


	//송신 스레드 생성
	hThread[Idx_Send] = (HANDLE)
		_beginthreadex(
			NULL,
			0,
			UTCPClient::procInteraction,
			(LPVOID)(CD),
			CREATE_SUSPENDED,
			(unsigned *)&dwThreadId[Idx_Send]
		);
	if (hThread[Idx_Send] == NULL)
	{
		_tprintf(_T("Thread creation fault! \n"));
		return -1;
	}
	//수신 스레드 생성
	hThread[Idx_Receive] = (HANDLE)
		_beginthreadex(
			NULL,
			0,
			UTCPClient::procRecieve,
			(LPVOID)(CD),
			CREATE_SUSPENDED,
			(unsigned *)&dwThreadId[Idx_Receive]
		);
	if (hThread[Idx_Receive] == NULL)
	{
		_tprintf(_T("Thread creation fault! \n"));
		return -1;
	}




	ResumeThread(hThread[Idx_Send]);
	ResumeThread(hThread[Idx_Receive]);
	
	//WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	WaitForSingleObject(hThread[Idx_Send], INFINITE);








	delete CD;
	delete sPacket;
	delete dPacket;
	// closesocket()
	closesocket(sock);
	DeleteCriticalSection(&hCritical);
	printf("Colse Socket\n");
	// 윈속 종료
	WSACleanup();
	return 0;
}

/*
*	Server로 데이터를 보냅니다
*/
unsigned int __stdcall UTCPClient::procInteraction(LPVOID IpParam)
{
	//ConectionData
	auto CD = (ConnectionData*)IpParam;
	if (!CD)return false;


	
	//수신용 지역변수들
	int retval = 0;
	int len = 0;
	auto client = CD->Client;
	
	auto sock = CD->Sock;

	char actionKey;
	
	// 서버와 데이터 통신
	while (1) {
		cout << endl;
		cout << "===========================" << endl;
		cout << "|     Press Key           |" << endl;
		cout << "|     1. Send Message     |" << endl;
		cout << "|     2. Confirm Log      |" << endl;
		cout << "|     3. Exit             |" << endl;
		cout << "===========================" << endl;
		//action Key를 받고 입력 버퍼를 지웁니다
		cin >> actionKey;
		cin.ignore();

		switch (actionKey)
		{
		case '1':
			cout << endl;
			
			cout << "[Send Message]" << endl;
			client->writeMessage(sock, client, retval);
			break;
		case '2':

			cout << endl;
			cout << "[Confirm Server Log]" << endl;
			client->requestReadMessage(sock, client, retval);
			Sleep(1000);
			break;
		case '3':
			cout << "[Exit]" << endl;
			return 0;
			break;
		default:
			cout << "[Notify] 잘못된 입력입니다" << endl;
			break;
		}

		//===============================
		//	구 버전
		//// 데이터 입력	buf_Message
		//printf("\n[보낼 데이터] ");
		//if (fgets(CD->Client->communicationData->buf_Message, BUFSIZE + 1, stdin) == NULL)
		//	break;
		//// '\n' 문자 제거
		//len = strlen(CD->Client->communicationData->buf_Message);
		//if (CD->Client->communicationData->buf_Message[len - 1] == '\n')
		//	CD->Client->communicationData->buf_Message[len - 1] = '\0';
		//if (strlen(CD->Client->communicationData->buf_Message) == 0)
		//	break;
		////buf_IP와 share값 조정
		//strcpy_s(CD->Client->communicationData->buf_IP , getIPAdrress());
		//CD->Client->communicationData->Share += 10;
		////데이터 보내기
		//retval = send(CD->Sock, (char*)(CD->Client->communicationData), sizeof(CommunicationData), 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("send()");
		//	return false;
		//}
		////출력 문구가 겹치는 것을 방지하기 위해서
		//EnterCriticalSection(&CD->Client->hCritical);
		//printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
		//LeaveCriticalSection(&CD->Client->hCritical);

		

		 
	}

	return 0;
}

/*
*	Server로부터 메시지를 받습니다
*/

unsigned int __stdcall UTCPClient::procRecieve(LPVOID IpParam)
{

	//ConectionData
	auto CD = (ConnectionData*)IpParam;
	if (!CD)return false;

	
	
	auto sock = CD->Sock;
	auto hCritical = CD->Client->hCritical;
	int retval=0;
	auto client = CD->Client;
	while (1)
	{
		//===================================================
		//		구 버전
		//clearBuffer(CD->Client->communicationData->buf_Message);
		//clearBuffer(CD->Client->communicationData->buf_IP);
		//retval = recv(CD->Sock, (char*)CD->Client->communicationData, sizeof(CommunicationData), 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("recv()");
		//	return false;
		//}
		//else if (retval == 0)
		//	return false;
		////출력이 밀리는 것을 방지
		//EnterCriticalSection(&CD->Client->hCritical);
		////Share 값 동기화
		//if (retval < BUFSIZE)
		//{
		//	//printf("sync test\n");
		//	CD->Client->communicationData->Share = atoi(CD->Client->communicationData->buf_Message);
		//}
		//else 
		//{
		//	// 받은 데이터 출력
		////buf[retval] = '\0';
		//	printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		//	printf("[받은 데이터] %s\t\n", CD->Client->communicationData->buf_Message);
		//	//	CD->Client->communicationData->Share = atoi(CD->Client->communicationData->Share);
		//	printf("[받은 데이터] %s\tShare = %d\n", CD->Client->communicationData->buf_IP, CD->Client->communicationData->Share);
		//}
		//LeaveCriticalSection(&CD->Client->hCritical);
		//Static Packet 수신

		//Static Packet 수신
		retval = recv(sock, (char*)client->sPacket, sizeof(FStaticPacket), 0);
		if ( retval == SOCKET_ERROR) {
			err_display("recv()");
			return false;
		}
		else if ( retval == 0)
			return false;
		

		
		EnterCriticalSection(&hCritical);

		switch (client->sPacket->Header)
		{
		case EPacketHeader::Null:
			cout << "Header is NULL" << endl;
			break;
		//클라이언트는 못 받습니다
		case EPacketHeader::req_read_log_CtoS:
			cout << "[오류] 클라이언틑는 req_read_log_CtoS를 받을 수 없습니다" << endl;

			break;

		//서버가 보낸 로그를 읽어 들입니다.
		case EPacketHeader::req_read_log_StoC:
			retval = recv(sock, (char*)client->dPacket, client->sPacket->Length, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				return false;
			}
			//cout << "[Notify] 받아온 서버의 Log 입니다." << endl;
			cout << client->dPacket->CString << endl;

			break;
		//클라이언트는 못 받습니다
	   	 case EPacketHeader::send_msg_CtoS:
			cout << "[오류] 클라이언틑는 send_msg_CtoS를 받을 수 없습니다" << endl;
			
			break;

			//서버로부터 메시지를 보낸 것에 대한 확인을 받습니다
		case EPacketHeader::send_msg_StoC:
			//dPacket 수신
			retval = recv(sock, (char*)client->dPacket, client->sPacket->Length, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				return false;
			}
			else if (retval == 0)
				return false;
			//확인 메시지 출력
			cout << "\n[Notify] " << retval << " 바이트를 받았습니다" << endl;
			cout << "[Notify] " << client->dPacket->CString << endl;


			break;
		}

		
		LeaveCriticalSection(&hCritical);
	}
	
	return true;
}

bool UTCPClient::writeMessage(SOCKET& sock, UTCPClient * client, int& retval)
{
	string inputStr;
	
	

	getline(cin, inputStr);


	//보낼 데이터 준비 작업
	int strLen = inputStr.length() + 1;
	auto sPacket = client->sPacket;
	auto dPacket = client->dPacket;
	//Static packet 값 조정
	sPacket->Length = strLen;
	sPacket->Header = EPacketHeader::send_msg_CtoS;
	//Dynamic packet 값 조정

	strncpy_s(dPacket->CString, strLen, inputStr.c_str(), inputStr.length());


	EnterCriticalSection(& client->hCritical);

	//packet->Header = EPacketHeader::send_msg_CtoS;
	//Static Packet 보내기
	retval = send(sock, (char*)sPacket, sizeof(FStaticPacket), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return false;
	}
	cout << "[Notify] Static Packet " << retval << " 바이트를 보냈습니다" << endl;
	retval = send(sock, (char*)dPacket, sPacket->Length, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return false;
	}
	cout << "[Notify] Dynamic Packet " << retval << " 바이트를 보냈습니다" << endl;
	cout << "[보낸 메시지] " << dPacket->CString << endl;
	LeaveCriticalSection(&client->hCritical);

	Sleep(66);
	return true;
}

bool UTCPClient::requestReadMessage(SOCKET & sock, UTCPClient * client, int & retval)
{
	auto sPacket = client->sPacket;
	auto dPacket = client->dPacket;
	
	sPacket->Header = EPacketHeader::req_read_log_CtoS;
	sPacket->Length = BUFSIZ;

	retval = send(sock, (char*)sPacket, sizeof(FStaticPacket), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return false;
	}



	return false;
}


//// 사용자 정의 데이터 수신 함수. 
//int UTCPClient::recvn(SOCKET s, char* buf, int len, int flags)
//{
//	//   int received;
//	//   char* ptr = buf;
//	//   int left = len;
//
//	   ////printf("\nReceived string length : %d\n", len);
//
//	   ////수신 받은 문자열의 길이를 반환
//	   //received = recv(s, ptr, left, flags);
//	   //if (received == SOCKET_ERROR)
//	   //	return SOCKET_ERROR;
//	   //else if (received == 0)
//	   //	return(len - left);
//
//	   //left -= received;
//
//	   //ptr += received;
//	   //printf("Left : %d\tptr : %s\n", left, ptr);
//	//   while (left > 0) {
//	//       
//	   //printf("Received string length : %d\n", received);
//	   //	//printf("left : %d\n", left);
//	//       if (received == SOCKET_ERROR)
//	//           return SOCKET_ERROR;
//	//       else if (received == 0)
//	//           break;
//
//	//       left -= received;
//
//	//       ptr += received;
//	   //	printf("Left : %d\tptr : %s\n", left, ptr);
//	//   }
//	   //printf("return value : %d\n", (len - left));
//	return 0;
//}


void UTCPClient::addAditionalText(char * inputBuf,const char* text)
{
	if (!inputBuf) { printf("nullptr\n"); return; }

	//char Text[] = " from Client";

	strcat_s(inputBuf, BUFSIZE, text);
}



void UTCPClient::printCurrentTime()
{
	auto stime = std::chrono::system_clock::now();
	auto mill = std::chrono::duration_cast<std::chrono::milliseconds>(stime.time_since_epoch());

	long long currentTimeMillis = mill.count();
	int msc = currentTimeMillis % 1000;

	struct tm t;
	time_t timer; // 시간측정
	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	localtime_s(&t, &timer); // 초 단위의 시간을 분리하여 구조체에 넣기 
	printf("현재 시간은 "); printf("%d년 %d월 %d일 %d시 %d분 %d초 %d milsec입니다.\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, msc);

}

void UTCPClient::clearBuffer(char * buf)
{
	if (!buf)return;
	memset(buf, 0,  BUFSIZE);
	
}

//int UTCPClient::findShare(const char* buf)
//{
//	//찾는데 실패하면 무한으로 리턴
//	int value= INT_MAX;
//	//Share value string
//	char* valueStr=nullptr;
//	
//	for (int i = 0; i < strlen(buf); ++i)
//	{
//		if (buf[i] == '=')
//		{
//			int valueLen = strlen(buf) - i - 1;
//			//숫자 자리 수 만큼 공간 확보
//			valueStr = new char[valueLen];
//
//			//i는 =의 위치니까 다음 칸부터
//			memcpy(valueStr, buf + i+1, valueLen);
//
//			//Share값을 찾았으니
//			bIsNewMessage = true;
//
//			//string to int
//			value = atoi(valueStr);
//			printf("\nfind : %d\n",value);
//
//
//			delete[] valueStr;
//			break;
//		}
//	}
//	
//	//value += 10;
//
//
//
//	return value;
//}

//https://www.tutorialspoint.com/how-to-get-the-ip-address-of-local-computer-using-c-cplusplus
const char * UTCPClient::getIPAdrress()
{

	char host[256];
	char *IP;
	struct hostent *host_entry;
	int hostname;
	hostname = gethostname(host, sizeof(host)); //find the host name
	//check_host_name(hostname);
	host_entry = gethostbyname(host); //find host information
	//check_host_entry(host_entry);
	IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); //Convert into IP string
	//printf("Current Host Name: %s\n", host);
	//printf("Host IP: %s\n", IP);

	return IP;
}


