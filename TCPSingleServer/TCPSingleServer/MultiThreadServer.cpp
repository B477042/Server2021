#include "MultiThreadServer.h"




MultiThreadServer::MultiThreadServer()
{
	waitTimer = 0;
	bPower = true;
	n_Client = 0;

	n_Client = 0;
	for (int i = Idx_thread; i < NUM_OF_THREAD; ++i)
		remainThreadSlot.push(i);
}

//int MultiThreadServer::RunServer()
//{
//
//	
//
//	//���� ���� 
//	while (1)
//	{
//		//Ŭ���̾�Ʈ ������ ����ٸ� ��� ���·� ���� 
//		if(ClientSockets.empty())
//		while (waitTimer >= M_waitTime)
//		{
//			
//			
//
//
//			//���� ms, 1 s = 1000 ms
//			//sleep()
//		}
//
//
//
//
//	}
//	
//	return 0;
//}

int MultiThreadServer::initServer()
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;


	createServerSocket();

	InitializeCriticalSection(&hCriticalSection);

	

	return 0;
}

int MultiThreadServer::RunMultiThreadServer()
{
	//Wait Thread ����
	hThread[Idx_Wait] = (HANDLE)
		_beginthreadex(
			NULL,
			0,
			MultiThreadServer::procWait,
			this,
			CREATE_SUSPENDED,
			(unsigned *)&dwThreadId[Idx_Wait]
		);
	if (hThread[Idx_Wait] == NULL)
	{
		_tprintf(_T("Thread creation fault! \n"));
		return -1;
	}

	//Accept Thread ����
	hThread[Idx_Accept] = (HANDLE)
		_beginthreadex(
			NULL,
			0,
			MultiThreadServer::procAccept,
			this,
			CREATE_SUSPENDED,
			(unsigned *)&dwThreadId[Idx_Accept]
		);
	if (hThread[Idx_Accept] == NULL)
	{
		_tprintf(_T("Thread creation fault! \n"));
		return -1;
	}

	ResumeThread(hThread[Idx_Wait]);
	ResumeThread(hThread[Idx_Accept]);

	//cout << "stop!" << endl;
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	//cout << "GJ" << endl;
	return 0;
}

int MultiThreadServer::closeServer()
{

	// closesocket()
	closesocket(ServerSockets[0]);




	// ���� ����
	WSACleanup();
	return 0;
}

void MultiThreadServer::createServerSocket()
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
	retval = ::bind(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");
	
	// listen()
	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");


	ServerSockets.push_back(sock);

	ServerSockets.push_back(sock);

}

/*
	� Ŭ���̾�Ʈ�͵� ������ ���� �ʾҴٸ� Ÿ�̸Ӹ� �۵���Ų��.
	Ŭ���̾�Ʈ���� ��� �ð��� const float M_waitTimer�� �������� �Ѵ�.(�⺻ 10��)

	- Ŭ���̾�Ʈ�� ����� �����尡 ���ٸ� Ÿ�̸Ӹ� ������. 
	- ����� ���¶�� �����带 ������. 
*/
unsigned int __stdcall MultiThreadServer::procWait(LPVOID lpParam)
{
	


//	cout << "\n====procWait Thread id : " << this_thread::get_id() << "========" << endl;
	auto server = (MultiThreadServer*)lpParam;
	if (!server) {
		cout << " Casting Failed : procWait" << endl;
	}

	DWORD sleepTime = 100;

	//����� Ŭ���̾�Ʈ�� ���ٸ� 0.1�ʸ��� Ÿ�̸Ӹ� �ѹ��� �ø���. 
	while (1)
	{
		//power check
		if (!server ->bPower)break;
		// timer timeout		
		if (server->waitTimer >= server->M_waitTime)break;

		if (server->n_Client > 0)continue;
		
		Sleep(sleepTime);
		server->waitTimer += sleepTime;

		DWORD dur = server->M_waitTime - server->waitTimer;

		if (dur == server->M_waitTime / 2)
		{
			cout << server->M_waitTime / 2000 << "�� �� ���α׷� ����" << endl;
			continue;
		}

		if (dur == server->M_waitTime / 4)
		{
			cout << server->M_waitTime / 4000 << "�� �� ���α׷� ����" << endl;
			continue;
		}
	}

	
	//power ����
	server->bPower = false;
	

	return 0;
}



unsigned int __stdcall  MultiThreadServer::procAccept(LPVOID lpParam)
{
	//cout << "\n====proc Accept Thread id : " << this_thread::get_id() << "========" << endl;
	auto server = (MultiThreadServer*)lpParam;
	if (!server) {
		cout << " Casting Failed : procAccept" << endl;
	}

	while (server->bPower)
	{
		
		server->acceptSocket(&server->ServerSockets[0]);
	}


	return 0;
}

unsigned int __stdcall  MultiThreadServer::procCommunication(LPVOID lpParam)
{
	//cout << "\n====proc Communication Thread id : " << this_thread::get_id() << "========" << endl;

	//������ �ۼ��� ����
	while (1)
	{
		//������ ����� �ۼ��� �ߴ�
		if (!bResult)break;

		//����
		for (auto it : ClientSockets)
			if (receiveData(it) == false)
			{
				bResult = false;
				eraseIndex = it;
				++n_Disconnect;

			}


		//�۽�
		for (auto it : ClientSockets)
			if (sendData(it) == false)
			{
				bResult = false;
				eraseIndex = it;
				++n_Disconnect;
			}

	}
	//�Ѱ��� ���� �� ���
	if (n_Disconnect == 1)
	{
		// closesocket()
		closesocket(eraseIndex->sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(eraseIndex->addr.sin_addr), ntohs(eraseIndex->addr.sin_port));


		for (unsigned int i = 0; i < ClientSockets.size(); ++i)
		{
			if (ClientSockets[i] == eraseIndex)
			{
				delete(ClientSockets[i]);
				eraseIndex = nullptr;
				ClientSockets.erase(ClientSockets.begin() + i);
				//printf("Erase ClientSocket\n"); //��� Ȯ��
			}
		}
		printf("dis 1\n");

	}
	//2�� �� ��� �� �����
	if (n_Disconnect == 2)
	{
		for (auto it : ClientSockets)
		{
			closesocket(it->sock);
			printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
				inet_ntoa(it->addr.sin_addr), ntohs(it->addr.sin_port));
			delete(it);
			it = nullptr;
		}
		ClientSockets.clear();
		printf("dis 2\n");
	}






	return 0;
}




bool MultiThreadServer::createCommunicationRoom(void* inputParam, int idx)
{
	//Idx üũ
	if (idx > NUM_OF_THREAD)
	{
		cout << "Thread Overflow " << endl;
		return false;
	}

	if (idx < Idx_thread)
	{
		cout << "Thread Underflow" << endl;
		return false;
	}



	hThread[idx] = (HANDLE)
		_beginthreadex(
			NULL,
			0,
			MultiThreadServer::procCommunication,
			(LPVOID)(&inputParam),
			CREATE_SUSPENDED,
			(unsigned *)&dwThreadId[idx]
		);
	
	if (hThread[idx] == NULL)
	{
		_tprintf(_T("Thread creation fault! \n"));
		return false;
	}


	return true;
}

void MultiThreadServer::acceptSocket(SOCKET * sock)
{
	//����ó�� - �����ο����� �� ���� Ŭ���̾�Ʈ�� ���� �õ���
	//		   - power�� ������ ��

	if (!bPower) { cout << "\n===== power off =====\n"; return; }

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;


	//���� ������� ������ �����带 �켱���� �Ѵ�
	EnterCriticalSection(&hCriticalSection);

	// accept()
	addrlen = sizeof(clientaddr);
	client_sock = accept(*sock, (SOCKADDR*)&clientaddr, &addrlen);
	if (client_sock == INVALID_SOCKET) {
		err_display("accept()");
		return;
	}

	if (!bPower) { cout << "\n===== power off =====\n"; LeaveCriticalSection(&hCriticalSection); return; }

	// ������ Ŭ���̾�Ʈ ���� ���
	printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	FClientSocket* clientSocket = new FClientSocket();
	clientSocket->sock = client_sock;
	clientSocket->addr = clientaddr;

	ClientSockets.push_back(clientSocket);

	LeaveCriticalSection(&hCriticalSection);

}

bool MultiThreadServer::receiveData(FClientSocket* cs)
{
	EnterCriticalSection(&hCriticalSection);

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

	LeaveCriticalSection(&hCriticalSection);
	return true;
}

bool MultiThreadServer::sendData(FClientSocket * cs)
{
	EnterCriticalSection(&hCriticalSection);
	//���� �߰�
	addAditionalText(cs->buf, " from Server", cs->retval);

	// ������ ������
	cs->retval = send(cs->sock, cs->buf, cs->retval, 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("send()");
		LeaveCriticalSection(&hCriticalSection);
		return false;
	}
	LeaveCriticalSection(&hCriticalSection);
	return true;
}

void MultiThreadServer::addAditionalText(char * inputBuf, const char * text, int & retval)
{
	if (!inputBuf) { printf("nullptr\n"); return; }

	

	strcat_s(inputBuf, BUFSIZE, text);

	
	retval = strlen(inputBuf);


}

int MultiThreadServer::addClient()
{
	++n_Client;
	int retval = remainThreadSlot.front();
	remainThreadSlot.pop();
	return retval;
}

int MultiThreadServer::removeClient(int num)
{
	if (num < Idx_thread || num >= NUM_OF_THREAD)
		return -1;
	--n_Client;
	remainThreadSlot.push(num);

	return 0;
}

//// num_of_thread���� �� ���� ���°� �����Ѵ�
//bool MultiThreadServer::checkThreadSpace()
//{
//	int dwLeng =sizeof dwThreadId/sizeof dwThreadId[0] ;
//	int hLeng = sizeof hThread / sizeof hThread[0];
//
//
//	if (dwLeng > NUM_OF_THREAD)
//	{
//		cout << "dwThreadId length error" << endl;
//		return false;
//	}
//	if (hLeng > NUM_OF_THREAD)
//	{
//		cout<<"hThread length error"
//		return false;
//	}
//
//	return true;
//}

void MultiThreadServer::printCurrentTime()
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
