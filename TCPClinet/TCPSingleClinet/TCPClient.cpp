
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
	
	InitializeCriticalSection(&hCritical);


	//printf("Size of struct : %d", sizeof(CommunicationData));
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
	ConnectionData* CD=new ConnectionData();
	CD->Client = this;
	CD->Sock = sock;
	

	communicationData = new CommunicationData();
	
	communicationData->Share = 0;
	clearBuffer(communicationData->buf_Message);
	clearBuffer(communicationData->buf_IP);


	//�۽� ������ ����
	hThread[Idx_Send] = (HANDLE)
		_beginthreadex(
			NULL,
			0,
			UTCPClient::procSend,
			(LPVOID)(CD),
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
	
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);









	delete CD;
	delete communicationData;
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
	//ConectionData
	auto CD = (ConnectionData*)IpParam;
	if (!CD)return false;



	//������ ���ڿ��� ���̰� �ȴ�
	int retval = 0;
	int len = 0;

	// ������ ������ ���
	while (1) {
	
		//Share ���� ���� ���� �ִ�
		if (CD->Client->bIsNewMessage)
		{

			CD->Client->bIsNewMessage = false;



		}


		// ������ �Է�	buf_Message
		printf("\n[���� ������] ");
		if (fgets(CD->Client->communicationData->buf_Message, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(CD->Client->communicationData->buf_Message);
		if (CD->Client->communicationData->buf_Message[len - 1] == '\n')
			CD->Client->communicationData->buf_Message[len - 1] = '\0';
		if (strlen(CD->Client->communicationData->buf_Message) == 0)
			break;

		//buf_IP�� share�� ����
		strcpy_s(CD->Client->communicationData->buf_IP , getIPAdrress());

		CD->Client->communicationData->Share += 10;



		//������ ������
		retval = send(CD->Sock, (char*)(CD->Client->communicationData), sizeof(CommunicationData), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			return false;
		}
		//��� ������ ��ġ�� ���� �����ϱ� ���ؼ�
		EnterCriticalSection(&CD->Client->hCritical);
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
		LeaveCriticalSection(&CD->Client->hCritical);

		////======================���� ������ ���� �ǽ�==================
		////������ ���� �ǽ�
		//HeaderUserInfo headerUser;
		//UserInfoData user;

		//headerUser.messageLen = strlen(CD->Client->communicationData ->buf_Message)+1;

		//user.message = nullptr;
		//user.message = new char[headerUser.messageLen];

		//strcpy_s(user.message, headerUser.messageLen,CD->Client->communicationData->buf_Message);
		//user.id = rand() % 100;
		//user.x= rand() % 100;
		//user.y = rand() % 100;
		//user.z = rand() % 100;

		//headerUser.dataSize = sizeof(user)+headerUser.messageLen;

		//retval = send(CD->Sock, (char*)(&headerUser), sizeof(headerUser), 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("send()");
		//	return false;
		//}
		//retval = send(CD->Sock, (char*)(&user), headerUser.dataSize, 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("send()");
		//	return false;
		//}

		//delete[]user.message;

		////Receieve �Լ� ����� ���� ��� �����ݴϴ�.
		//Sleep(66);
	}

	return 0;
}


unsigned int __stdcall UTCPClient::procRecieve(LPVOID IpParam)
{

	//ConectionData
	auto CD = (ConnectionData*)IpParam;
	if (!CD)return false;


	int retval=0;
	while (1)
	{
		
		clearBuffer(CD->Client->communicationData->buf_Message);
		clearBuffer(CD->Client->communicationData->buf_IP);

		retval = recv(CD->Sock, (char*)CD->Client->communicationData, sizeof(CommunicationData), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			return false;
		}
		else if (retval == 0)
			return false;


		//����� �и��� ���� ����
		EnterCriticalSection(&CD->Client->hCritical);

		//Share �� ����ȭ
		if (retval < BUFSIZE)
		{
			//printf("sync test\n");
			CD->Client->communicationData->Share = atoi(CD->Client->communicationData->buf_Message);
		}

		else {
			// ���� ������ ���
		//buf[retval] = '\0';
			printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
			printf("[���� ������] %s\t\n", CD->Client->communicationData->buf_Message);
			//	CD->Client->communicationData->Share = atoi(CD->Client->communicationData->Share);
			printf("[���� ������] %s\tShare = %d\n", CD->Client->communicationData->buf_IP, CD->Client->communicationData->Share);




		}
		


		LeaveCriticalSection(&CD->Client->hCritical);
	}
	
	return true;
}

//
//bool UTCPClient::sendData(int&retval, SOCKET & CD, char * buf, int length, int flags)
//{
//	// ������ ������
//	retval = send(CD, buf, strlen(buf), 0);
//	if (retval == SOCKET_ERROR) {
//		err_display("send()");
//		return false;
//	}
//	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
//	return true;
//}
//
//bool UTCPClient::receiveData(int&retval, SOCKET & CD, char * buf, int length, int flags)
//{
//	// ������ �ޱ�. 
//	/*retval = recvn(CD, buf, BUFSIZE, 0);
//	if (retval == SOCKET_ERROR) {
//		err_display("recv()");
//		return false;
//	}
//	else if (retval == 0)
//		return false;*/
//
//	retval = recv(CD, buf, BUFSIZE, 0);
//	if (retval == SOCKET_ERROR) {
//		err_display("recv()");
//		return false;
//	}
//	else if (retval == 0)
//		return false;
//
//
//	// ���� ������ ���
//	//buf[retval] = '\0';
//	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
//	printf("[���� ������] %s\t\n", buf);
//	return true;
//}
//

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
	memset(buf, 0,  BUFSIZE);
	
}

//int UTCPClient::findShare(const char* buf)
//{
//	//ã�µ� �����ϸ� �������� ����
//	int value= INT_MAX;
//	//Share value string
//	char* valueStr=nullptr;
//	
//	for (int i = 0; i < strlen(buf); ++i)
//	{
//		if (buf[i] == '=')
//		{
//			int valueLen = strlen(buf) - i - 1;
//			//���� �ڸ� �� ��ŭ ���� Ȯ��
//			valueStr = new char[valueLen];
//
//			//i�� =�� ��ġ�ϱ� ���� ĭ����
//			memcpy(valueStr, buf + i+1, valueLen);
//
//			//Share���� ã������
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


