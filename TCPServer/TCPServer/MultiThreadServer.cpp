#include "MultiThreadServer.h"


 int  UMultiThreadServer::Share = 0;

UMultiThreadServer::UMultiThreadServer()
{
	waitTimer = 0;
	bPower = true;
	n_Client = 0;

	n_Client = 0;
	for (int i = Idx_thread; i < NUM_OF_THREAD; ++i)
		remainThreadSlot.push(i);

	
}



int UMultiThreadServer::initServer()
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;


	createServerSocket();

	InitializeCriticalSection(&hCS_ProcAccept);
	InitializeCriticalSection(&hCS_AcceptSocket);
	
	InitializeCriticalSection(&hcs_ReceiveData);
	InitializeCriticalSection(&hCS_DeleteCS);


	return 0;
}

int UMultiThreadServer::RunMultiThreadServer()
{
	//Wait Thread ����
	hThread[Idx_Wait] = (HANDLE)
		_beginthreadex(
			NULL,
			0,
			UMultiThreadServer::procWait,
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
			UMultiThreadServer::procAccept,
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

	cout << "ByeBye" << endl;
	//cout << "GJ" << endl;
	return 0;
}

int UMultiThreadServer::closeServer()
{

	// closesocket()
	closesocket(ServerSockets[0]);

	DeleteCriticalSection(&hCS_ProcAccept);
	DeleteCriticalSection(&hCS_AcceptSocket);
	DeleteCriticalSection(&hcs_ReceiveData);

	DeleteCriticalSection(&hCS_DeleteCS);
	// ���� ����
	WSACleanup();
	return 0;
}

void UMultiThreadServer::createServerSocket()
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

	//ServerSockets.push_back(sock);

	
}

/*
	� Ŭ���̾�Ʈ�͵� ������ ���� �ʾҴٸ� Ÿ�̸Ӹ� �۵���Ų��.
	Ŭ���̾�Ʈ���� ��� �ð��� const float M_waitTimer�� �������� �Ѵ�.(�⺻ 10��)

	- Ŭ���̾�Ʈ�� ����� �����尡 ���ٸ� Ÿ�̸Ӹ� ������. 
	- ����� ���¶�� �����带 ������. 
*/
unsigned int __stdcall UMultiThreadServer::procWait(LPVOID lpParam)
{
	


//	cout << "\n====procWait Thread id : " << this_thread::get_id() << "========" << endl;
	auto server = (UMultiThreadServer*)lpParam;
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

		//������ �ƴٸ�
		if (server->n_Client > 0) { 
			//Ÿ�̸� �ʱ�ȭ
			server->waitTimer = 0;


			continue; 
		}
		
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


//Power �������� ������
unsigned int __stdcall  UMultiThreadServer::procAccept(LPVOID lpParam)
{
	//cout << "\n====proc Accept Thread id : " << this_thread::get_id() << "========" << endl;
	auto server = (UMultiThreadServer*)lpParam;
	if (!server) {
		cout << " Casting Failed : procAccept" << endl;
	}

	while (server->bPower)
	{

		


		//accept�� �޾ƿ´�. return value clientsocket
		auto cSocket = server->acceptSocket(&server->ServerSockets[0]);

		if (!cSocket) { /*cout << "cSocket is nullptr" << endl;*/ continue; }


		FConnectionData* Data = new FConnectionData();
		int idx_t=-1;
		Data->Server = server;
		Data->idx_Sockets = cSocket;

		//queue<int> remainThreadSlot �� �����Ͽ� �����ִ� Thread index�� �޾ƿ´�.
		//���� �����忡�� ���ÿ� �����ϸ� ���̴ϱ� ����ȭ ���ѵд�
		EnterCriticalSection(&server->hCS_ProcAccept);


		if (server->remainThreadSlot.empty())
		{
			LeaveCriticalSection(&server->hCS_ProcAccept);
			continue;
		}

		//client�� �÷��ش�. 
		idx_t = server->addClient();

		Data->idx_Thread = idx_t;


		LeaveCriticalSection(&server->hCS_ProcAccept);
		//thread�� �Ҵ��ؼ� ����� ��Ų��
		server->createCommunicationRoom(Data, idx_t);

	}


	return 0;
}

unsigned int __stdcall  UMultiThreadServer::procCommunication(LPVOID lpParam)
{
	
	auto Data = (FConnectionData*)lpParam;

	if (!Data)
	{
		cout << "Fail to Casting Data" << endl;
		delete Data;
		return -1;
	}




	//�ۼ��� �� ������ �߻��Ǹ� �ߴ��Ѵ�
	bool bResult = true;

	auto server = Data->Server;
	auto socket = Data->idx_Sockets;

	FCommunicationData* CD= new FCommunicationData();
	memset(CD->buf_Message,0,BUFSIZE+1);
	memset(CD->buf_IP, 0, BUFSIZE + 1);
	CD->Share = Share;

	//������ �ۼ��� ����
	while (1)
	{
		//������ ����� �ۼ��� �ߴ�
		if (!bResult)break;

		//����
		
		if (server->receiveData(socket,CD) == false)
		{
				bResult = false;
				
		}
		//������ ����� �ۼ��� �ߴ�
		if (!bResult)break;

		//�۽�
		
		if (server->sendData(socket,CD) == false)
		{
			bResult = false;
			
		}

		//server->sendShare(socket);


	}


		//client sockets���� ���� �� Ŭ���̾�Ʈ���� ������ �����. 
	EnterCriticalSection(&server->hCS_DeleteCS);
	
		// closesocket()
		closesocket(socket->sock);
		::printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(socket->addr.sin_addr), ntohs(socket->addr.sin_port));

		server->removeClient(Data->idx_Thread);
		
		int idx_remove = -1;
		//ClinetsSockets���� ã�Ƽ� ����� ã�´�. 
		for (unsigned int i = 0; i < server->ClientSockets.size(); ++i)
		{
			if (server->ClientSockets[i] == Data->idx_Sockets)
				idx_remove = i;
		}


		
		delete(server->ClientSockets[idx_remove]);
		server->ClientSockets[idx_remove] = nullptr;
		server->ClientSockets.erase(server->ClientSockets.begin() + idx_remove);
		//printf("Erase ClientSocket\n"); //��� Ȯ��

	LeaveCriticalSection(&server->hCS_DeleteCS);
		delete Data;
		delete CD;

	
	
	return 0;
}




bool UMultiThreadServer::createCommunicationRoom(void* inputParam, int idx_t)
{
	auto Data = (FConnectionData*)inputParam;


	if (!Data)
	{
		cout << "\nCasting failed " << endl;
		return false;
	}

	//Idx üũ
	if (idx_t > NUM_OF_THREAD)
	{
		cout << "Thread Overflow " << endl;
		return false;
	}

	if (idx_t < Idx_thread)
	{
		cout << "Thread Underflow" << endl;
		return false;
	}



	hThread[idx_t] = (HANDLE)
		_beginthreadex(
			NULL,
			0,
			UMultiThreadServer::procCommunication,
			(LPVOID)(Data),
			CREATE_SUSPENDED,
			(unsigned *)&dwThreadId[idx_t]
		);
	
	if (hThread[idx_t] == NULL)
	{
		_tprintf(_T("Thread creation fault! \n"));
		return false;
	}

	ResumeThread(hThread[idx_t]);


	return true;
}


//
FClientSocket* UMultiThreadServer::acceptSocket(SOCKET * sock)
{
	//����ó�� - �����ο����� �� ���� Ŭ���̾�Ʈ�� ���� �õ���
	//		   - power�� ������ ��



	if (!bPower) { cout << "\n===== power off =====\n"; return nullptr; }

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;


	//���� ������� ������ �����带 �켱���� �Ѵ�
	
	EnterCriticalSection(&hCS_AcceptSocket);

	// accept()
	addrlen = sizeof(clientaddr);
	client_sock = accept(*sock, (SOCKADDR*)&clientaddr, &addrlen);
	if (client_sock == INVALID_SOCKET) {
		err_display("accept()");
		return nullptr;
	}

	if (!bPower) { 
		cout << "\n===== power off =====\n"; 
		LeaveCriticalSection(&hCS_AcceptSocket);
		return nullptr;
	}

	//NUM_OF_THREAD - Idx_thread �������� �޴´�
	if (n_Client > NUM_OF_THREAD - Idx_thread-1)
	{
		//�� á���� 1�� ����
		cout << "Sleep" << endl;
		Sleep(1000);
		return nullptr;
	}

	// ������ Ŭ���̾�Ʈ ���� ���
	printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	
	FClientSocket* clientSocket = new FClientSocket();
	clientSocket->sock = client_sock;
	clientSocket->addr = clientaddr;

	ClientSockets.push_back(clientSocket);

	//LeaveCriticalSection(&hCriticalSection);
	LeaveCriticalSection(&hCS_AcceptSocket);
	
	return clientSocket;
}

bool UMultiThreadServer::receiveData(FClientSocket* cs,FCommunicationData* cd)
{
	EnterCriticalSection(&hcs_ReceiveData);
	
	// ������ �ޱ�
	cs->retval = recv(cs->sock,(char*) cd, sizeof(CommunicationData), 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (cs->retval == 0)
		return false;


	// ���� �޽��� ������ ���
	cd->buf_Message[strlen(cd->buf_Message)] = '\0';
	printf("[TCP/%s:%d] %s\n", inet_ntoa(cs->addr.sin_addr),
		ntohs(cs->addr.sin_port), cd->buf_Message);

	printf("[TCP/%s:%d] Ŭ���̾�Ʈ IP : %s\tShare = %d\n", inet_ntoa(cs->addr.sin_addr),
		ntohs(cs->addr.sin_port), cd->buf_IP,cd->Share);

	Share = cd->Share;


	//memset(cd->buf_Message, 0, BUFSIZE + 1);
	//memset(cd->buf_IP, 0, BUFSIZE + 1);


	LeaveCriticalSection(&hcs_ReceiveData);
	
	


	return true;
}

bool UMultiThreadServer::sendData(FClientSocket * cs, FCommunicationData* cd)
{
	//EnterCriticalSection(&hCriticalSection);


	//���� �߰�
	addAditionalText(cd->buf_Message, " from Server", cs->retval);
	//Share�� �߰�
	cd->Share = Share;

	// ������ ������
	cs->retval = send(cs->sock, (char*)cd, cs->retval, 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("send()");
		//LeaveCriticalSection(&hCriticalSection);
	
		return false;
	}

	


	//LeaveCriticalSection(&hCriticalSection);
	
	return true;
}

void UMultiThreadServer::addAditionalText(char * inputBuf, const char * text, int & retval)
{
	if (!inputBuf) { printf("nullptr\n"); return; }

	

	strcat_s(inputBuf, BUFSIZE, text);
	
	
	retval = strlen(inputBuf);


}
//
//void UMultiThreadServer::sendShare(FClientSocket * cs)
//{
//	//buf_Message�� �ʱ�ȭ
//	memset(cs->buf_Message, 0, sizeof cs->buf_Message);
//	////Client ip�ּ� ���ֱ�
//	//addAditionalText(cs->buf_Message, inet_ntoa(cs->addr.sin_addr), cs->retval);
//	////Share text ���ֱ�
//	//addAditionalText(cs->buf_Message, " Share=", cs->retval);
//
//	////Test value 10
//	//addAditionalText(cs->buf_Message, "10", cs->retval);
//
//	//���� �޽��� �ۼ�
//	char message[BUFSIZE];
//	sprintf(message, "%s Share=10", inet_ntoa(cs->addr.sin_addr));
//	memcpy(cs->buf_Message, message, strlen(message));
//
//	printf("\nSend Message : %s\n", message);
//	cs->retval = send(cs->sock, cs->buf_Message, strlen(cs->buf_Message), 0);
//	if (cs->retval == SOCKET_ERROR)
//	{
//		err_display("send()");
//
//	}
//
//
//}

int UMultiThreadServer::addClient()
{
	++n_Client;
	int retval = remainThreadSlot.front();
	remainThreadSlot.pop();
	return retval;
}

int UMultiThreadServer::removeClient(int num)
{
	if (num < Idx_thread || num >= NUM_OF_THREAD)
		return -1;
	--n_Client;
	remainThreadSlot.push(num);

	return 0;
}

//// num_of_thread���� �� ���� ���°� �����Ѵ�
//bool UMultiThreadServer::checkThreadSpace()
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

void UMultiThreadServer::printCurrentTime()
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
