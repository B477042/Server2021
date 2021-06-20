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
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//서버 소켓 초기화
	createServerSocket();
	//크리티컬 섹션 초기화
	InitializeCriticalSection(&hCS_ProcAccept);
	InitializeCriticalSection(&hCS_AcceptSocket);
	InitializeCriticalSection(&hcs_ReceiveData);
	InitializeCriticalSection(&hCS_DeleteCS);
	InitializeCriticalSection(&hCS_FileAccess);
	//파일 시스템 초기화
	initFileStreamer();

	return 0;
}

int UMultiThreadServer::RunMultiThreadServer()
{
	//Wait Thread 생성
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

	//Accept Thread 생성
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
	// 윈속 종료
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
	어떤 클라이언트와도 연결이 되지 않았다면 타이머를 작동시킨다.
	클라이언트와의 대기 시간은 const float M_waitTimer를 기준으로 한다.(기본 10초)

	- 클라이언트와 연결된 쓰레드가 없다면 타이머를 돌린다. 
	- 연결된 상태라면 쓰레드를 굴린다. 
*/
unsigned int __stdcall UMultiThreadServer::procWait(LPVOID lpParam)
{
	


//	cout << "\n====procWait Thread id : " << this_thread::get_id() << "========" << endl;
	auto server = (UMultiThreadServer*)lpParam;
	if (!server) {
		cout << " Casting Failed : procWait" << endl;
	}

	DWORD sleepTime = 100;
	
	//연결된 클라이언트가 없다면 0.1초마다 타이머를 한번씩 올린다. 
	while (1)
	{
		//power check
		if (!server ->bPower)break;
		// timer timeout		
		if (server->WaitTimer >= server->Max_WaitTime)break;

		//연결이 됐다면
		if (server->n_Client > 0) { 
			//타이머 초기화
			server->WaitTimer = 0;


			continue; 
		}
		
		Sleep(sleepTime);
		

		server->WaitTimer += sleepTime;

		DWORD dur = server->Max_WaitTime - server->WaitTimer;

		if (dur == server->Max_WaitTime / 2)
		{
			cout << server->Max_WaitTime / 2000 << "초 후 프로그렘 종료" << endl;
			continue;
		}

		if (dur == server->Max_WaitTime / 4)
		{
			cout << server->Max_WaitTime / 4000 << "초 후 프로그렘 종료" << endl;
			continue;
		}
	}

	
	//power 종료
	server->bPower = false;
	

	return 0;
}


//Power 내려가면 꺼진다
unsigned int __stdcall  UMultiThreadServer::procAccept(LPVOID lpParam)
{
	//cout << "\n====proc Accept Thread id : " << this_thread::get_id() << "========" << endl;
	auto server = (UMultiThreadServer*)lpParam;
	if (!server) {
		cout << " Casting Failed : procAccept" << endl;
	}

	while (server->bPower)
	{

		


		//accept로 받아온다. return value clientsocket
		auto cSocket = server->acceptSocket(&server->ServerSockets[0]);

		if (!cSocket) { /*cout << "cSocket is nullptr" << endl;*/ continue; }


		ConnectionData* Data = new ConnectionData();
		int idx_t=-1;
		Data->Server = server;
		Data->idx_Sockets = cSocket;

		//queue<int> RemainThreadSlot 에 접근하여 남아있는 Thread index를 받아온다.
		//여러 쓰레드에서 동시에 접근하면 꼬이니까 동기화 시켜둔다
		EnterCriticalSection(&server->hCS_ProcAccept);


		if (server->RemainThreadSlot.empty())
		{
			LeaveCriticalSection(&server->hCS_ProcAccept);
			continue;
		}

		//client를 늘려준다. 
		idx_t = server->addClient();

		Data->idx_Thread = idx_t;


		LeaveCriticalSection(&server->hCS_ProcAccept);
		//thread를 할당해서 통신을 시킨다
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




	//송수신 중 문제가 발생되면 중단한다
	bool bResult = true;

	auto server = Data->Server;
	auto socket = Data->idx_Sockets;

	
	//CommunicationData* CD= new CommunicationData();
	//memset(CD->buf_Message,0,BUFSIZE+1);
	//memset(CD->buf_IP, 0, BUFSIZE + 1);
	//CD->Share = Share;
	//통신용 패킷 초기화
	//MyPacket* packet = new MyPacket();
	FStaticPacket* sPacket = new FStaticPacket();
	FDynamicPacket*dPacket = new FDynamicPacket();

	//데이터 송수신 상태
	while (1)
	{
		//문제가 생기면 송수신 중단
		if (!bResult)break;

		//수신. static packet
		
		if (server->receiveData(socket,sPacket,dPacket) == false)
		{
				bResult = false;
				
		}
		//문제가 생기면 송수신 중단
		if (!bResult)break;

		//송신
		
		if (server->sendData(socket,sPacket, dPacket) == false)
		{
			bResult = false;
			
		}

		


		//server->sendShare(socket);


	}


		//client sockets에서 현재 이 클라이언트와의 정보를 지운다. 
	EnterCriticalSection(&server->hCS_DeleteCS);
	
		// closesocket()
		closesocket(socket->sock);
		::printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(socket->addr.sin_addr), ntohs(socket->addr.sin_port));

		server->removeClient(Data->idx_Thread);
		
		int idx_remove = -1;
		//ClinetsSockets에서 찾아서 지울걸 찾는다. 
		for (unsigned int i = 0; i < server->ClientSockets.size(); ++i)
		{
			if (server->ClientSockets[i] == Data->idx_Sockets)
				idx_remove = i;
		}


		
		delete(server->ClientSockets[idx_remove]);
		server->ClientSockets[idx_remove] = nullptr;
		server->ClientSockets.erase(server->ClientSockets.begin() + idx_remove);
		//printf("Erase ClientSocket\n"); //출력 확인

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

	//Idx 체크
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
	//예외처리 - 수용인원보다 더 많은 클라이언트가 접속 시도시
	//		   - power가 꺼졌을 때



	if (!bPower) { cout << "\n===== power off =====\n"; return nullptr; }

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;


	//먼저 여기까지 실행한 쓰레드를 우선으로 한다
	
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

	//NUM_OF_THREAD - Idx_thread 개까지만 받는다
	if (n_Client > NUM_OF_THREAD - Idx_thread-1)
	{
		//꽉 찼으면 1초 재운다
		cout << "Sleep" << endl;
		Sleep(1000);
		return nullptr;
	}

	//콘솔창 출력 작업 및 log 파일에 기록 작업
	char logString[BUFSIZE] = "0,";
	//logString에 문자열 저장
	sprintf(logString,"\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	writeLog(logString);
	printf("%s", logString);


	//클라이언트 소켓 정보 저장
	ClientSocket* clientSocket = new ClientSocket();
	clientSocket->sock = client_sock;
	clientSocket->addr = clientaddr;

	ClientSockets.push_back(clientSocket);

	//LeaveCriticalSection(&hCriticalSection);
	LeaveCriticalSection(&hCS_AcceptSocket);
	
	return clientSocket;
}
//구버전
bool UMultiThreadServer::receiveData(ClientSocket* cs,CommunicationData* cd)
{

	
	// 데이터 받기
	cs->retval = recv(cs->sock,(char*) cd, sizeof(CommunicationData), 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (cs->retval == 0)
		return false;

	//Critical Section 진입 :출력 로그를 순서대로 출력시키기 위해서
	EnterCriticalSection(&hcs_ReceiveData);

	

	// 받은 메시지 데이터 출력
	cd->buf_Message[strlen(cd->buf_Message)] = '\0';
	printf("[TCP/%s:%d] %s\n", inet_ntoa(cs->addr.sin_addr),
		ntohs(cs->addr.sin_port), cd->buf_Message);

	//printf("[TCP/%s:%d] 클라이언트 IP : %s\n", inet_ntoa(cs->addr.sin_addr),
	//	ntohs(cs->addr.sin_port));

	


	LeaveCriticalSection(&hcs_ReceiveData);
	
	


	return true;
}


//구버전
bool UMultiThreadServer::sendData(ClientSocket * cs, CommunicationData* cd)
{
	//EnterCriticalSection(&hCriticalSection);


	//문구 추가
	addAditionalText(cd->buf_Message, " from Server", cs->retval);
	//printf("\ncd->share :%d \t share : %d\n", cd->Share, Share);
	////Share값 추가
	//cd->Share = Share;
	
	// 데이터 보내기
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

	// Static Packet 받기
	cs->retval = recv(cs->sock, (char*)sPacket, sizeof(FStaticPacket), 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (cs->retval == 0)
		return false;

	//Critical Section 진입 :출력 로그를 순서대로 출력시키기 위해서
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
		cout << "[오류] 서버는 req_read_log_StoC를 받을 수 없습니다" << endl;
		break;
		//Client로부터 Dynamic Packet을 받아옵니다
		//받은 후 클라이언트에게 확인 메시지를 보내기 위해 Static Packet의 해더를
		//send_msg_StoC로 바꿉니다
	case EPacketHeader::send_msg_CtoS:
		
		receiveData(cs, dPacket, sPacket->Length);
		sPacket->Header = EPacketHeader::send_msg_StoC;
		break;
	case EPacketHeader::send_msg_StoC:
		cout << "[오류] 서버는 send_msg_StoC를 받을 수 없습니다" << endl;
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
		cout << "[오류] 서버는 req_read_log_CtoS를 보낼 수 없습니다" << endl;

		break;
	case EPacketHeader::req_read_log_StoC:
		cout << "Header is req_read_log_StoC" << endl;
		break;
	 
	case EPacketHeader::send_msg_CtoS:
		cout << "[오류] 서버는 send_msg_CtoS를 보낼 수 없습니다" << endl;
	 
		break;

		//Client로 확인 메시지를 보냅니다
	case EPacketHeader::send_msg_StoC:

		//클라로 보낼 문구 추가
		addAditionalText(dPacket->CString, " from Server", sPacket->Length);
		//Static packet 전송
		cs->retval = send(cs->sock, (char*)sPacket, sizeof(FStaticPacket), 0);
		if (cs->retval == SOCKET_ERROR) {
			err_display("send()");
			//LeaveCriticalSection(&hCriticalSection);

			return false;
		}
		//Dynamic packet 전송
		cs->retval = send(cs->sock, (char*)dPacket, sPacket->Length, 0);
		if (cs->retval == SOCKET_ERROR) {
			err_display("send()");
			//LeaveCriticalSection(&hCriticalSection);

			return false;
		}
		//메시지를 전송한 후 dPacket string을 초기화 시켜준다
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

	//읽기 전용
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
	time_t timer; // 시간측정
	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	localtime_s(&t, &timer); // 초 단위의 시간을 분리하여 구조체에 넣기 
	//printf("현재 시간은 "); printf("%d년 %d월 %d일 %d시 %d분 %d초 %d milsec입니다.\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, msc);
	stringstream s_buffer;
	s_buffer<<" 시작 시간 " << t.tm_year + 1900<<"년 " << t.tm_mon + 1<<"월 " << t.tm_mday<<"일 " << t.tm_hour<<"시 " << t.tm_min<<"분 " << t.tm_sec<<"초 "<<msc<<" milsec입니다.\n";

	retval = s_buffer.str();
	return retval;
}
