
#include"TCPClient.h"

// ���� �Լ� ���� ��� �� ����
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



// ���� �Լ� ���� ���
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

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1] = "";
	int len;

	// ������ ������ ���
	while (1) {
		// ������ �Է�
		printf("\n[���� ������] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

	

		//������ ������ �ޱ�
		SendData(retval, sock, buf, strlen(buf), 0);
		ReceiveData(retval, sock, buf, BUFSIZE, 0);


		//�ؽ�Ʈ �߰�
		addAditionalText(buf, " from Client");

		//������ ������ �ޱ�
		SendData(retval, sock, buf, strlen(buf), 0);
		ReceiveData(retval, sock, buf, BUFSIZE, 0);

	}

	// closesocket()
	closesocket(sock);
	printf("Colse Socket\n");
	// ���� ����
	WSACleanup();
	return 0;
}

bool UTCPClient::SendData(int&retval, SOCKET & sock, char * buf, int length, int flags)
{
	// ������ ������
	retval = send(sock, buf, strlen(buf), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return false;
	}
	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
	return true;
}

bool UTCPClient::ReceiveData(int&retval, SOCKET & sock, char * buf, int length, int flags)
{
	// ������ �ޱ�
	retval = recvn(sock, buf, BUFSIZE, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (retval == 0)
		return false;

	// ���� ������ ���
	buf[retval] = '\0';
	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
	printf("[���� ������] %s\t\n", buf);
	return true;
}


// ����� ���� ������ ���� �Լ�
int UTCPClient::recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

	//printf("\nReceived string length : %d\n", len);

	//�ѹ��� �޾ƿ´�
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
	time_t timer; // �ð�����
	timer = time(NULL); // ���� �ð��� �� ������ ���
	localtime_s(&t, &timer); // �� ������ �ð��� �и��Ͽ� ����ü�� �ֱ� 
	printf("���� �ð��� "); printf("%d�� %d�� %d�� %d�� %d�� %d�� %d milsec�Դϴ�.\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, msc);

}



//�� ���� �ڵ�
/*

	//// ������ ������
		//retval = send(sock, buf, strlen(buf), 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("send()");
		//	break;
		//}
		//printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);


		//// ������ �ޱ�
		//retval = recvn(sock, buf, BUFSIZE, 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("recv()");
		//	break;
		//}
		//else if (retval == 0)
		//	break;

		//// ���� ������ ���
		//buf[retval] = '\0';
		//printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		//printf("[���� ������] %s\t\n", buf);
//// ������ ������
		//retval = send(sock, buf, strlen(buf), 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("send()");
		//	break;
		//}
		//printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

		//// ������ �ޱ�
		//retval = recvn(sock, buf, BUFSIZE, 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("recv()");
		//	break;
		//}
		//else if (retval == 0)
		//	break;

		//// ���� ������ ���
		//buf[retval] = '\0';
		//printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		//printf("[���� ������] %s\t\n", buf);
*/