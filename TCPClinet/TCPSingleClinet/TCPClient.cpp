
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


char UTCPClient::buf_Send[BUFSIZE] = "0";
char UTCPClient::buf_Receive[BUFSIZE] = "0";
bool UTCPClient::bIsNewMessage = false;
UTCPClient::UTCPClient()
{
	clearBuffer(UTCPClient::buf_Send);
	clearBuffer(UTCPClient::buf_Receive);
	InitializeCriticalSection(&hCritical);
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
	


	//�۽� ������ ����
	hThread[Idx_Send] = (HANDLE)
		_beginthreadex(
			NULL,
			0,
			UTCPClient::procSend,
			(LPVOID)(sock),
			CREATE_SUSPENDED,
			(unsigned *)&dwThreadId[Idx_Send]
		);
	if (hThread[Idx_Send] == NULL)
	{
		_tprintf(_T("Thread creation fault! \n"));
		return -1;
	}
	//���� ������ ����
	hThread[Idx_Receive] = (HANDLE)
		_beginthreadex(
			NULL,
			0,
			UTCPClient::procRecieve,
			(LPVOID)(sock),
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
	
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);



	//// ������ ������ ���
	//while (1) {
	//	// ������ �Է�
	//	printf("\n[���� ������] ");
	//	if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
	//		break;

	//	// '\n' ���� ����
	//	len = strlen(buf);
	//	if (buf[len - 1] == '\n')
	//		buf[len - 1] = '\0';
	//	if (strlen(buf) == 0)
	//		break;
	//	

	//	
	//	//������ ������ �ޱ�
	//	sendData(retval, sock, buf, strlen(buf), 0);
	//	receiveData(retval, sock, buf, BUFSIZE, 0);
	//	//receiveData(retval, sock, buf, BUFSIZE, 0);

	//	////�ؽ�Ʈ �߰�
	//	//addAditionalText(buf, " from Client");

	//	////������ ������ �ޱ�
	//	//sendData(retval, sock, buf, strlen(buf), 0);
	//	//receiveData(retval, sock, buf, BUFSIZE, 0);

	//}







	// closesocket()
	closesocket(sock);
	DeleteCriticalSection(&hCritical);
	printf("Colse Socket\n");
	// ���� ����
	WSACleanup();
	return 0;
}

unsigned int __stdcall UTCPClient::procSend(LPVOID IpParam)
{

	auto sock = (SOCKET)IpParam;
	if (!sock)return false;



	//������ ���ڿ��� ���̰� �ȴ�
	int retval = 0;
	int len = 0;

	// ������ ������ ���
	while (1) {
	
		// ������ �Է�
		printf("\n[���� ������] ");
		if (fgets(buf_Send, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(buf_Send);
		if (buf_Send[len - 1] == '\n')
			buf_Send[len - 1] = '\0';
		if (strlen(buf_Send) == 0)
			break;



		//������ ������
		retval = send(sock, buf_Send, strlen(buf_Send), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			return false;
		}
		//��� ������ ��ġ�� ���� �����ϱ� ���ؼ�
		EnterCriticalSection(&hCritical);
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
		LeaveCriticalSection(&hCritical);
		//Receieve �Լ� ����� ���� ��� �����ݴϴ�.
		Sleep(66);
	}

	return 0;
}


unsigned int __stdcall UTCPClient::procRecieve(LPVOID IpParam)
{

	auto sock = (SOCKET)IpParam;
	if (!sock)return false;


	int retval=0;
	while (1)
	{
		
		clearBuffer(buf_Receive);
		retval = recv(sock, buf_Receive, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			return false;
		}
		else if (retval == 0)
			return false;
		EnterCriticalSection(&hCritical);

		// ���� ������ ���
		//buf[retval] = '\0';
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		printf("[���� ������] %s\t\n", buf_Receive);
		int share = findShare(UTCPClient::buf_Receive);
		if(share!=INT_MAX)
		share += 10;

		LeaveCriticalSection(&hCritical);
	}
	
	return true;
}


bool UTCPClient::sendData(int&retval, SOCKET & sock, char * buf, int length, int flags)
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

bool UTCPClient::receiveData(int&retval, SOCKET & sock, char * buf, int length, int flags)
{
	// ������ �ޱ�. 
	/*retval = recvn(sock, buf, BUFSIZE, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (retval == 0)
		return false;*/

	retval = recv(sock, buf, BUFSIZE, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (retval == 0)
		return false;


	// ���� ������ ���
	//buf[retval] = '\0';
	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
	printf("[���� ������] %s\t\n", buf);
	return true;
}


// ����� ���� ������ ���� �Լ�. 
int UTCPClient::recvn(SOCKET s, char* buf, int len, int flags)
{
	//   int received;
	//   char* ptr = buf;
	//   int left = len;

	   ////printf("\nReceived string length : %d\n", len);

	   ////���� ���� ���ڿ��� ���̸� ��ȯ
	   //received = recv(s, ptr, left, flags);
	   //if (received == SOCKET_ERROR)
	   //	return SOCKET_ERROR;
	   //else if (received == 0)
	   //	return(len - left);

	   //left -= received;

	   //ptr += received;
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
	return 0;
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

void UTCPClient::clearBuffer(char * buf)
{
	if (!buf)return;
	memset(buf, 0, sizeof buf);
	
}

int UTCPClient::findShare(const char* buf)
{
	//ã�µ� �����ϸ� �������� ����
	int value= INT_MAX;
	//Share value string
	char* valueStr;
	
	for (int i = 0; i < sizeof(buf); ++i)
	{
		if (buf[i] == '=')
		{
			int valueLen = sizeof(buf) - i - 1;
			//���� �ڸ� �� ��ŭ ���� Ȯ��
			valueStr = new char[valueLen];

			//i�� =�� ��ġ�ϱ� ���� ĭ����
			memcpy(valueStr, buf + i+1, valueLen);

			//share���� ã������
			bIsNewMessage = true;

			//string to int
			value = atoi(valueStr);
			printf("\nfind : %d\n",value);


			delete[] valueStr;
			break;
		}
	}
	
	//value += 10;



	return value;
}


