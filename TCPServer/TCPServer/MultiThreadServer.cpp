#include "MultiThreadServer.h"


 //int  UMultiThreadServer::Share = 0;

UMultiThreadServer::UMultiThreadServer()
{
	WaitTimer = 0;
	bPower = true;
	n_Client = 0;

	n_Client = 0;
	for (int i = Idx_thread; i < NUM_OF_THREAD; ++i)
		RemainThreadSlot.push(i);
	
	
}



int UMultiThreadServer::initServer()
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//���� ���� �ʱ�ȭ
	createServerSocket();
	//ũ��Ƽ�� ���� �ʱ�ȭ
	InitializeCriticalSection(&hCS_ProcAccept);
	InitializeCriticalSection(&hCS_AcceptSocket);
	InitializeCriticalSection(&hcs_ReceiveData);
	InitializeCriticalSection(&hCS_DeleteCS);
	InitializeCriticalSection(&hCS_FileAccess);
	//���� �ý��� �ʱ�ȭ
	initFileStreamer();

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

int UMultiThreadServer::CloseServer()
{

	// closesocket()
	closesocket(ServerSockets[0]);

	DeleteCriticalSection(&hCS_ProcAccept);
	DeleteCriticalSection(&hCS_AcceptSocket);
	DeleteCriticalSection(&hcs_ReceiveData);
	DeleteCriticalSection(&hCS_FileAccess);
	DeleteCriticalSection(&hCS_DeleteCS);
	// ���� ����
	WSACleanup();
	closeFileStreamer();
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
		if (server->WaitTimer >= server->Max_WaitTime)break;

		//������ �ƴٸ�
		if (server->n_Client > 0) { 
			//Ÿ�̸� �ʱ�ȭ
			server->WaitTimer = 0;


			continue; 
		}
		
		Sleep(sleepTime);
		

		server->WaitTimer += sleepTime;

		DWORD dur = server->Max_WaitTime - server->WaitTimer;

		if (dur == server->Max_WaitTime / 2)
		{
			cout << server->Max_WaitTime / 2000 << "�� �� ���α׷� ����" << endl;
			continue;
		}

		if (dur == server->Max_WaitTime / 4)
		{
			cout << server->Max_WaitTime / 4000 << "�� �� ���α׷� ����" << endl;
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


		ConnectionData* Data = new ConnectionData();
		int idx_t=-1;
		Data->Server = server;
		Data->idx_Sockets = cSocket;

		//queue<int> RemainThreadSlot �� �����Ͽ� �����ִ� Thread index�� �޾ƿ´�.
		//���� �����忡�� ���ÿ� �����ϸ� ���̴ϱ� ����ȭ ���ѵд�
		EnterCriticalSection(&server->hCS_ProcAccept);


		if (server->RemainThreadSlot.empty())
		{
			LeaveCriticalSection(&server->hCS_ProcAccept);
			continue;
		}

		//client�� �÷��ش�. 
		idx_t = server->addClient();

		Data->idx_Thread = idx_t;


		LeaveCriticalSection(&server->hCS_ProcAccept);
		//thread�� �Ҵ��ؼ� ����� ��Ų��
		server->CreateCommunicationRoom(Data, idx_t);

	}


	return 0;
}

unsigned int __stdcall  UMultiThreadServer::procCommunication(LPVOID lpParam)
{
	
	auto Data = (ConnectionData*)lpParam;

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

	
	//CommunicationData* CD= new CommunicationData();
	//memset(CD->buf_Message,0,BUFSIZE+1);
	//memset(CD->buf_IP, 0, BUFSIZE + 1);
	//CD->Share = Share;
	//��ſ� ��Ŷ �ʱ�ȭ
	//MyPacket* packet = new MyPacket();
	FStaticPacket* sPacket = new FStaticPacket();
	FDynamicPacket*dPacket = new FDynamicPacket();

	//������ �ۼ��� ����
	while (1)
	{
		//������ ����� �ۼ��� �ߴ�
		if (!bResult)break;

		//����. static packet
		
		if (server->receiveData(socket,sPacket,dPacket) == false)
		{
				bResult = false;
				
		}
		//������ ����� �ۼ��� �ߴ�
		if (!bResult)break;

		//�۽�
		
		if (server->sendData(socket,sPacket, dPacket) == false)
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
		//delete CD;
		//delete packet;
		delete sPacket;
		delete dPacket;
	
	return 0;
}




bool UMultiThreadServer::CreateCommunicationRoom(void* inputParam, int idx_t)
{
	auto Data = (ConnectionData*)inputParam;


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
ClientSocket* UMultiThreadServer::acceptSocket(SOCKET * sock)
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

	//�ܼ�â ��� �۾� �� log ���Ͽ� ��� �۾�
	char logString[BUFSIZE] = "0,";
	//logString�� ���ڿ� ����
	sprintf(logString,"\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	writeLog(logString);
	printf("%s", logString);


	//Ŭ���̾�Ʈ ���� ���� ����
	ClientSocket* clientSocket = new ClientSocket();
	clientSocket->sock = client_sock;
	clientSocket->addr = clientaddr;

	ClientSockets.push_back(clientSocket);

	//LeaveCriticalSection(&hCriticalSection);
	LeaveCriticalSection(&hCS_AcceptSocket);
	
	return clientSocket;
}
//������
bool UMultiThreadServer::receiveData(ClientSocket* cs,CommunicationData* cd)
{

	
	// ������ �ޱ�
	cs->retval = recv(cs->sock,(char*) cd, sizeof(CommunicationData), 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (cs->retval == 0)
		return false;

	//Critical Section ���� :��� �α׸� ������� ��½�Ű�� ���ؼ�
	EnterCriticalSection(&hcs_ReceiveData);

	

	// ���� �޽��� ������ ���
	cd->buf_Message[strlen(cd->buf_Message)] = '\0';
	printf("[TCP/%s:%d] %s\n", inet_ntoa(cs->addr.sin_addr),
		ntohs(cs->addr.sin_port), cd->buf_Message);

	//printf("[TCP/%s:%d] Ŭ���̾�Ʈ IP : %s\n", inet_ntoa(cs->addr.sin_addr),
	//	ntohs(cs->addr.sin_port));

	


	LeaveCriticalSection(&hcs_ReceiveData);
	
	


	return true;
}


//������
bool UMultiThreadServer::sendData(ClientSocket * cs, CommunicationData* cd)
{
	//EnterCriticalSection(&hCriticalSection);


	//���� �߰�
	addAditionalText(cd->buf_Message, " from Server", cs->retval);
	//printf("\ncd->share :%d \t share : %d\n", cd->Share, Share);
	////Share�� �߰�
	//cd->Share = Share;
	
	// ������ ������
	cs->retval = send(cs->sock, (char*)cd, sizeof(CommunicationData), 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("send()");
		//LeaveCriticalSection(&hCriticalSection);
	
		return false;
	}

	
	return true;
}

bool UMultiThreadServer::receiveData(ClientSocket * cs, FStaticPacket* sPacket, FDynamicPacket* dPacket)
{

	// Static Packet �ޱ�
	cs->retval = recv(cs->sock, (char*)sPacket, sizeof(FStaticPacket), 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (cs->retval == 0)
		return false;

	//Critical Section ���� :��� �α׸� ������� ��½�Ű�� ���ؼ�
	EnterCriticalSection(&hcs_ReceiveData);
	switch (sPacket->Header)
	{
	case EPacketHeader::Null:
		cout << "Header is NULL" << endl;
		break;
	case EPacketHeader::req_read_log_CtoS:
		cout << "Header is req_read_log_CtoS" << endl;
	
		break;
	case EPacketHeader::req_read_log_StoC:
		cout << "[����] ������ req_read_log_StoC�� ���� �� �����ϴ�" << endl;
		break;
		//Client�κ��� Dynamic Packet�� �޾ƿɴϴ�
		//���� �� Ŭ���̾�Ʈ���� Ȯ�� �޽����� ������ ���� Static Packet�� �ش���
		//send_msg_StoC�� �ٲߴϴ�
	case EPacketHeader::send_msg_CtoS:
		
		receiveData(cs, dPacket, sPacket->Length);
		sPacket->Header = EPacketHeader::send_msg_StoC;
		break;
	case EPacketHeader::send_msg_StoC:
		cout << "[����] ������ send_msg_StoC�� ���� �� �����ϴ�" << endl;
		break;

 
	}
	LeaveCriticalSection(&hcs_ReceiveData);

	return true;
}
//
bool UMultiThreadServer::receiveData(ClientSocket * cs, FDynamicPacket * packet, const int Length)
{
	
	cs->retval = recv(cs->sock, (char*)packet, sizeof(FDynamicPacket)+Length, 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (cs->retval == 0)
		return false;
	cout << "[TCP / "<< inet_ntoa(cs->addr.sin_addr)<<":"<< ntohs(cs->addr.sin_port)<<"] " << packet->CString << endl;
	
	
	return true;
}



bool UMultiThreadServer::sendData(ClientSocket * cs, FStaticPacket* sPacket, FDynamicPacket* dPacket)
{

	switch (sPacket->Header)
	{
	case EPacketHeader::Null:
		cout << "Header is NULL" << endl;
		
		break;
	case EPacketHeader::req_read_log_CtoS:
		cout << "[����] ������ req_read_log_CtoS�� ���� �� �����ϴ�" << endl;

		break;
	case EPacketHeader::req_read_log_StoC:
		cout << "Header is req_read_log_StoC" << endl;
		break;
	 
	case EPacketHeader::send_msg_CtoS:
		cout << "[����] ������ send_msg_CtoS�� ���� �� �����ϴ�" << endl;
	 
		break;

		//Client�� Ȯ�� �޽����� �����ϴ�
	case EPacketHeader::send_msg_StoC:

		//Ŭ��� ���� ���� �߰�
		addAditionalText(dPacket->CString, " from Server", sPacket->Length);
		//Static packet ����
		cs->retval = send(cs->sock, (char*)sPacket, sizeof(FStaticPacket), 0);
		if (cs->retval == SOCKET_ERROR) {
			err_display("send()");
			//LeaveCriticalSection(&hCriticalSection);

			return false;
		}
		//Dynamic packet ����
		cs->retval = send(cs->sock, (char*)dPacket, sPacket->Length, 0);
		if (cs->retval == SOCKET_ERROR) {
			err_display("send()");
			//LeaveCriticalSection(&hCriticalSection);

			return false;
		}
		//�޽����� ������ �� dPacket string�� �ʱ�ȭ �����ش�
		dPacket->ResetCString();

		break;
	}



	return true;
}

bool UMultiThreadServer::sendData(ClientSocket * cs, FDynamicPacket* packet, const int Length)
{
	return true;
}

void UMultiThreadServer::addAditionalText(char * inputBuf, const char * text, int & retval)
{
	if (!inputBuf) { printf("nullptr\n"); return; }

	

	strcat_s(inputBuf, BUFSIZE, text);
	
	
	retval = strlen(inputBuf);


}
void UMultiThreadServer::syncShareValue()
{
	//char buf[255];
	//memset(buf, 0, 255);
	//_itoa_s(Share, buf,10);
	////printf("in sync :%s", buf);
	//for (auto it : ClientSockets)
	//{
	//	send(it->sock, buf, strlen(buf),0);
	//}
	 
	
}

void UMultiThreadServer::initFileStreamer()
{

	//�б� ����
	ReadFile.open(FileAddress, ios_base::in);
	//Append 
	WriteFile.open(FileAddress, ios_base::app);
	WriteFile  << getCurrentTime_ToString()<<endl;


}
void UMultiThreadServer::closeFileStreamer()
{
	ReadFile.close();
	WriteFile.close();
}


int UMultiThreadServer::addClient()
{
	++n_Client;
	int retval = RemainThreadSlot.front();
	RemainThreadSlot.pop();
	return retval;
}

int UMultiThreadServer::removeClient(int num)
{
	if (num < Idx_thread || num >= NUM_OF_THREAD)
		return -1;
	--n_Client;
	RemainThreadSlot.push(num);

	return 0;
}

void UMultiThreadServer::writeLog(const char * Input)
{
	EnterCriticalSection(&hCS_FileAccess);
	WriteFile << Input << endl;

	LeaveCriticalSection(&hCS_FileAccess);
}
 

string UMultiThreadServer::getCurrentTime_ToString()
{
	string retval;
	auto stime = std::chrono::system_clock::now();
	auto mill = std::chrono::duration_cast<std::chrono::milliseconds>(stime.time_since_epoch());

	long long currentTimeMillis = mill.count();
	int msc = currentTimeMillis % 1000;

	struct tm t;
	time_t timer; // �ð�����
	timer = time(NULL); // ���� �ð��� �� ������ ���
	localtime_s(&t, &timer); // �� ������ �ð��� �и��Ͽ� ����ü�� �ֱ� 
	//printf("���� �ð��� "); printf("%d�� %d�� %d�� %d�� %d�� %d�� %d milsec�Դϴ�.\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, msc);
	stringstream s_buffer;
	s_buffer<<" ���� �ð� " << t.tm_year + 1900<<"�� " << t.tm_mon + 1<<"�� " << t.tm_mday<<"�� " << t.tm_hour<<"�� " << t.tm_min<<"�� " << t.tm_sec<<"�� "<<msc<<" milsec�Դϴ�.\n";

	retval = s_buffer.str();
	return retval;
}
