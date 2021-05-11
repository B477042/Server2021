
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
	char buf[BUFSIZE + 1] = "";
	int len;

	// 서버와 데이터 통신
	while (1) {
		// 데이터 입력
		printf("\n[보낼 데이터] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

	

		//데이터 보내고 받기
		SendData(retval, sock, buf, strlen(buf), 0);
		ReceiveData(retval, sock, buf, BUFSIZE, 0);


		//텍스트 추가
		addAditionalText(buf, " from Client");

		//데이터 보내고 받기
		SendData(retval, sock, buf, strlen(buf), 0);
		ReceiveData(retval, sock, buf, BUFSIZE, 0);

	}

	// closesocket()
	closesocket(sock);
	printf("Colse Socket\n");
	// 윈속 종료
	WSACleanup();
	return 0;
}

bool UTCPClient::SendData(int&retval, SOCKET & sock, char * buf, int length, int flags)
{
	// 데이터 보내기
	retval = send(sock, buf, strlen(buf), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return false;
	}
	printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
	return true;
}

bool UTCPClient::ReceiveData(int&retval, SOCKET & sock, char * buf, int length, int flags)
{
	// 데이터 받기
	retval = recvn(sock, buf, BUFSIZE, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (retval == 0)
		return false;

	// 받은 데이터 출력
	buf[retval] = '\0';
	printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
	printf("[받은 데이터] %s\t\n", buf);
	return true;
}


// 사용자 정의 데이터 수신 함수
int UTCPClient::recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

	//printf("\nReceived string length : %d\n", len);

	//한번만 받아온다
	received = recv(s, ptr, left, flags);
	if (received == SOCKET_ERROR)
		return SOCKET_ERROR;
	else if (received == 0)
		return(len - left);

	left -= received;

	ptr += received;
	//printf("Left : %d\tptr : %s\n", left, ptr);
 //   while (left > 0) {
 //       
	//printf("Received string length : %d\n", received);
	//	//printf("left : %d\n", left);
 //       if (received == SOCKET_ERROR)
 //           return SOCKET_ERROR;
 //       else if (received == 0)
 //           break;

 //       left -= received;

 //       ptr += received;
	//	printf("Left : %d\tptr : %s\n", left, ptr);
 //   }
	//printf("return value : %d\n", (len - left));
    return (len - left);
}

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



//안 쓰는 코드
/*

	//// 데이터 보내기
		//retval = send(sock, buf, strlen(buf), 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("send()");
		//	break;
		//}
		//printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);


		//// 데이터 받기
		//retval = recvn(sock, buf, BUFSIZE, 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("recv()");
		//	break;
		//}
		//else if (retval == 0)
		//	break;

		//// 받은 데이터 출력
		//buf[retval] = '\0';
		//printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		//printf("[받은 데이터] %s\t\n", buf);
//// 데이터 보내기
		//retval = send(sock, buf, strlen(buf), 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("send()");
		//	break;
		//}
		//printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

		//// 데이터 받기
		//retval = recvn(sock, buf, BUFSIZE, 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("recv()");
		//	break;
		//}
		//else if (retval == 0)
		//	break;

		//// 받은 데이터 출력
		//buf[retval] = '\0';
		//printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		//printf("[받은 데이터] %s\t\n", buf);
*/