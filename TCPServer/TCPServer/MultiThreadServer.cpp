#include "MultiThreadServer.h"


 int  MultiThreadServer::share = 0;

MultiThreadServer::MultiThreadServer()
{
	waitTimer = 0;
	bPower = true;
	n_Client = 0;

	n_Client = 0;
	for (int i = Idx_thread; i < NUM_OF_THREAD; ++i)
		remainThreadSlot.push(i);

	
}



int MultiThreadServer::initServer()
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;


	createServerSocket();

	InitializeCriticalSection(&hCS_ProcAccept);
	InitializeCriticalSection(&hCS_AcceptSocket);
	InitializeCriticalSection(&hCS_ReceiveData);
	InitializeCriticalSection(&hCS_SendData);
	InitializeCriticalSection(&hCS_DeleteCS);


	return 0;
}

int MultiThreadServer::RunMultiThreadServer()
{
	//Wait Thread 생성
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

	//Accept Thread 생성
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

	cout << "ByeBye" << endl;
	//cout << "GJ" << endl;
	return 0;
}

int MultiThreadServer::closeServer()
{

	// closesocket()
	closesocket(ServerSockets[0]);

	DeleteCriticalSection(&hCS_ProcAccept);
	DeleteCriticalSection(&hCS_AcceptSocket);
	DeleteCriticalSection(&hCS_ReceiveData);
	DeleteCriticalSection(&hCS_SendData);
	DeleteCriticalSection(&hCS_DeleteCS);
	// 윈속 종료
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

	//ServerSockets.push_back(sock);

	
}

/*
	어떤 클라이언트와도 연결이 되지 않았다면 타이머를 작동시킨다.
	클라이언트와의 대기 시간은 const float M_waitTimer를 기준으로 한다.(기본 10초)

	- 클라이언트와 연결된 쓰레드가 없다면 타이머를 돌린다. 
	- 연결된 상태라면 쓰레드를 굴린다. 
*/
unsigned int __stdcall MultiThreadServer::procWait(LPVOID lpParam)
{
	


//	cout << "\n====procWait Thread id : " << this_thread::get_id() << "========" << endl;
	auto server = (MultiThreadServer*)lpParam;
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
		if (server->waitTimer >= server->M_waitTime)break;

		//연결이 됐다면
		if (server->n_Client > 0) { 
			//타이머 초기화
			server->waitTimer = 0;


			continue; 
		}
		
		Sleep(sleepTime);
		

		server->waitTimer += sleepTime;

		DWORD dur = server->M_waitTime - server->waitTimer;

		if (dur == server->M_waitTime / 2)
		{
			cout << server->M_waitTime / 2000 << "초 후 프로그렘 종료" << endl;
			continue;
		}

		if (dur == server->M_waitTime / 4)
		{
			cout << server->M_waitTime / 4000 << "초 후 프로그렘 종료" << endl;
			continue;
		}
	}

	
	//power 종료
	server->bPower = false;
	

	return 0;
}


//Power 내려가면 꺼진다
unsigned int __stdcall  MultiThreadServer::procAccept(LPVOID lpParam)
{
	//cout << "\n====proc Accept Thread id : " << this_thread::get_id() << "========" << endl;
	auto server = (MultiThreadServer*)lpParam;
	if (!server) {
		cout << " Casting Failed : procAccept" << endl;
	}

	while (server->bPower)
	{

		


		//accept로 받아온다. return value clientsocket
		auto cSocket = server->acceptSocket(&server->ServerSockets[0]);

		if (!cSocket) { /*cout << "cSocket is nullptr" << endl;*/ continue; }


		FCommunicationData* Data = new FCommunicationData();
		int idx_t=-1;
		Data->Server = server;
		Data->idx_Sockets = cSocket;

		//queue<int> remainThreadSlot 에 접근하여 남아있는 Thread index를 받아온다.
		//여러 쓰레드에서 동시에 접근하면 꼬이니까 동기화 시켜둔다
		EnterCriticalSection(&server->hCS_ProcAccept);


		if (server->remainThreadSlot.empty())
		{
			LeaveCriticalSection(&server->hCS_ProcAccept);
			continue;
		}

		//client를 늘려준다. 
		idx_t = server->addClient();

		Data->idx_Thread = idx_t;


		LeaveCriticalSection(&server->hCS_ProcAccept);
		//thread를 할당해서 통신을 시킨다
		server->createCommunicationRoom(Data, idx_t);

	}


	return 0;
}

unsigned int __stdcall  MultiThreadServer::procCommunication(LPVOID lpParam)
{
	
	auto Data = (FCommunicationData*)lpParam;

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

	//데이터 송수신 상태
	while (1)
	{
		//문제가 생기면 송수신 중단
		if (!bResult)break;

		//수신
		
		if (server->receiveData(socket) == false)
		{
				bResult = false;
				
		}
		//문제가 생기면 송수신 중단
		if (!bResult)break;

		//송신
		
		if (server->sendData(socket) == false)
		{
			bResult = false;
			
		}

		server->sendShare(socket);


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
		

	
	
	return 0;
}




bool MultiThreadServer::createCommunicationRoom(void* inputParam, int idx_t)
{
	auto Data = (FCommunicationData*)inputParam;


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
			MultiThreadServer::procCommunication,
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
FClientSocket* MultiThreadServer::acceptSocket(SOCKET * sock)
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

	// 접속한 클라이언트 정보 출력
	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	
	FClientSocket* clientSocket = new FClientSocket();
	clientSocket->sock = client_sock;
	clientSocket->addr = clientaddr;

	ClientSockets.push_back(clientSocket);

	//LeaveCriticalSection(&hCriticalSection);
	LeaveCriticalSection(&hCS_AcceptSocket);
	
	return clientSocket;
}

bool MultiThreadServer::receiveData(FClientSocket* cs)
{
	//EnterCriticalSection(&hCriticalSection);
	//EnterCriticalSection(&hCS_ReceiveData);
	// 데이터 받기
	cs->retval = recv(cs->sock, cs->buf, BUFSIZE, 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (cs->retval == 0)
		return false;


	// 받은 데이터 출력
	cs->buf[cs->retval] = '\0';
	printf("[TCP/%s:%d] %s\n", inet_ntoa(cs->addr.sin_addr),
		ntohs(cs->addr.sin_port), cs->buf);

	//LeaveCriticalSection(&hCriticalSection);
	//LeaveCriticalSection(&hCS_ReceiveData);
	return true;
}

bool MultiThreadServer::sendData(FClientSocket * cs)
{
	//EnterCriticalSection(&hCriticalSection);
//	EnterCriticalSection(&hCS_SendData);

	//문구 추가
	addAditionalText(cs->buf, " from Server", cs->retval);
	// 메시지 보내기
	cs->retval = send(cs->sock, cs->buf, cs->retval, 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("send()");
		//LeaveCriticalSection(&hCriticalSection);
		LeaveCriticalSection(&hCS_SendData);
		return false;
	}
	//LeaveCriticalSection(&hCriticalSection);
	//LeaveCriticalSection(&hCS_SendData);
	return true;
}

void MultiThreadServer::addAditionalText(char * inputBuf, const char * text, int & retval)
{
	if (!inputBuf) { printf("nullptr\n"); return; }

	

	strcat_s(inputBuf, BUFSIZE, text);
	
	
	retval = strlen(inputBuf);


}

void MultiThreadServer::sendShare(FClientSocket * cs)
{
	//buf값 초기화
	memset(cs->buf, 0, sizeof cs->buf);
	////Client ip주소 써주기
	//addAditionalText(cs->buf, inet_ntoa(cs->addr.sin_addr), cs->retval);
	////share text 써주기
	//addAditionalText(cs->buf, " share=", cs->retval);

	////Test value 10
	//addAditionalText(cs->buf, "10", cs->retval);

	//보낼 메시지 작성
	char message[BUFSIZE];
	sprintf(message, "%s share=10", inet_ntoa(cs->addr.sin_addr));
	memcpy(cs->buf, message, sizeof(message));

	printf("\nSend Message : %s\n", message);
	cs->retval = send(cs->sock, cs->buf, cs->retval, 0);
	if (cs->retval == SOCKET_ERROR)
	{
		err_display("send()");

	}


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

//// num_of_thread보다 더 많이 들어가는걸 방지한다
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
	time_t timer; // 시간측정
	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	localtime_s(&t, &timer); // 초 단위의 시간을 분리하여 구조체에 넣기 
	printf("현재 시간은 "); printf("%d년 %d월 %d일 %d시 %d분 %d초 %d milsec입니다.\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, msc);

}
