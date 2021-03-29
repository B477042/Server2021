
#include "SingleTCPServer.h"
// ���� �Լ� ���� ��� �� ����
static void err_quit(const char* msg)
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
static void err_display(const char* msg)
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


USingleTCPServer::USingleTCPServer()
{

}
int USingleTCPServer::RunServer()
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1]="";



	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Ŭ���̾�Ʈ�� ������ ���
		while (1) {
			// ������ �ޱ�
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;
			

			// ���� ������ ���
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), buf);
			//printf("Retval : %d\n", retval);

			//���� �߰�
			addAditionalText(buf," from Server",retval);
			
			//printf("before send : %s\n", buf);
			// ������ ������
			

			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
			
			
			//printCurrentTime();
			//printf("after send : %s\t%d\n",buf,retval);
			
		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

int USingleTCPServer::run1()
{// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	
	createSocket();
	
	

	//���� ������
	//vector<FClientSocket*>::iterator eraseIndex;
	FClientSocket* eraseIndex=nullptr;
	bool bResult = true;
	while (1)
	{
		//�׻� 2���� ���� ��ų ��
		int reconnect = 2 - ClientSockets.size();
		if(reconnect!=0)
		for (int i = 0; i < reconnect; ++i)
		{
			printf("���� �����........%d / %d\n", ClientSockets.size(), 2);
			acceptSocket(&ServerSockets[0]);
		}
		printf("���� �Ϸ�\n");

		while (1)
		{
			if (!bResult)break;

			for (auto it : ClientSockets)
				if (receiveData(it) == false)
				{
					bResult = false;
					eraseIndex = it;
					break;
				}
			for (auto it : ClientSockets)
				if (sendData(it) == false) 
				{ 
					eraseIndex = it;
					break; 
				}

		}
		// closesocket()
		closesocket(eraseIndex->sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(eraseIndex->addr.sin_addr), ntohs(eraseIndex->addr.sin_port));
		/*delete(eraseIndex);
		eraseIndex = nullptr;*/
		for (int i=0;i < ClientSockets.size();++i)
		{
			if (ClientSockets[i] == eraseIndex)
			{
				delete(ClientSockets[i]);
				eraseIndex = nullptr;
				ClientSockets.erase(ClientSockets.begin()+i);
				//printf("Erase ClientSocket\n");
			}
		}

		
	}
	
	// closesocket()
	closesocket(ServerSockets[0]);

	// ���� ����
	WSACleanup();
	return 0;
}


int USingleTCPServer::run2()
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	// closesocket()
	closesocket(ServerSockets[0]);

	// ���� ����
	WSACleanup();
	return 0;
}


void USingleTCPServer::createSocket()
{
	int retval;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	


	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	
	ServerSockets.push_back(sock);


}

void USingleTCPServer::acceptSocket(SOCKET* sock)
{
	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	

	// accept()
	addrlen = sizeof(clientaddr);
	client_sock = accept(*sock, (SOCKADDR*)&clientaddr, &addrlen);
	if (client_sock == INVALID_SOCKET) {
		err_display("accept()");
		return;
	}

	// ������ Ŭ���̾�Ʈ ���� ���
	printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	FClientSocket* clientSocket=new FClientSocket();
	clientSocket->sock = client_sock;
	clientSocket->addr = clientaddr;

	ClientSockets.push_back(clientSocket);
}

bool USingleTCPServer::receiveData(FClientSocket* cs)
{
	// ������ �ޱ�
	cs->retval = recv(cs->sock, cs->buf, BUFSIZE, 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (cs->retval == 0)
		return false;


	// ���� ������ ���
	cs->buf[cs->retval] = '\0';
	printf("[TCP/%s:%d] %s\n", inet_ntoa(cs->addr.sin_addr),
		ntohs(cs->addr.sin_port), cs->buf);
	//���� �߰�
	addAditionalText(cs->buf, " from Server", cs->retval);

	return true;
}

bool USingleTCPServer::sendData(FClientSocket * cs)
{
	// ������ ������


	cs->retval = send(cs->sock, cs->buf, cs->retval, 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("send()");
		return false;
	}
	return true;
}



void USingleTCPServer::addAditionalText(char * inputBuf, const char* text, int& retval)
{
	if (!inputBuf) { printf("nullptr\n"); return; }

	//char Text[] = " from Server";
	
	strcat_s(inputBuf,BUFSIZE, text);

	//printf("NEw Length : %d\n", strlen(inputBuf));
	retval = strlen(inputBuf);
	//inputBuf[strlen(inputBuf) + strlen(Text)] = '\0';
	

}

//������ Ŭ���̾�Ʈ�� �ְ� �޴� �ð��� ��Ȯ�� �˰� �;
//���ͳݿ��� �˻��ؼ� ����ϴ� ���¸� �����߽��ϴ�
//https://jhnyang.tistory.com/305
void USingleTCPServer::printCurrentTime()
{
	auto stime = std::chrono::system_clock::now();
	auto mill = std::chrono::duration_cast<std::chrono::milliseconds>(stime.time_since_epoch());

	long long currentTimeMillis = mill.count();
	int msc = currentTimeMillis % 1000;

	struct tm t;
	time_t timer; // �ð�����
	timer = time(NULL); // ���� �ð��� �� ������ ���
	localtime_s(&t, &timer); // �� ������ �ð��� �и��Ͽ� ����ü�� �ֱ� 
	printf("���� �ð��� "); printf("%d�� %d�� %d�� %d�� %d�� %d�� %d milsec�Դϴ�.\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,msc);



}




