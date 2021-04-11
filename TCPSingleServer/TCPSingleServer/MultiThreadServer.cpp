#include "MultiThreadServer.h"




MultiThreadServer::MultiThreadServer()
{
	waitTimer = 0;
	bPower = true;
}

//int MultiThreadServer::RunServer()
//{
//
//	
//
//	//메인 루프 
//	while (1)
//	{
//		//클라이언트 소켓이 비었다면 대기 상태로 들어가서 
//		if(ClientSockets.empty())
//		while (waitTimer >= M_waitTime)
//		{
//			
//			
//
//
//			//단위 ms, 1 s = 1000 ms
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
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;


	createServerSocket();

	InitializeCriticalSection(&hCriticalSection);

	

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
			NULL,
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
			NULL,
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

	ServerSockets.push_back(sock);

}


unsigned int __stdcall MultiThreadServer::procWait(LPVOID lpParam)
{
	cout << "\n====procWait Thread id : " << this_thread::get_id() << "========" << endl;



	return 0;
}

unsigned int __stdcall  MultiThreadServer::procAccept(LPVOID lpParam)
{
	cout << "\n====proc Accept Thread id : " << this_thread::get_id() << "========" << endl;



	return 0;
}

unsigned int __stdcall  MultiThreadServer::procCommunication(LPVOID lpParam)
{
	cout << "\n====proc Communication Thread id : " << this_thread::get_id() << "========" << endl;


	return 0;
}




bool MultiThreadServer::createCommunicationRoom(void* inputParam, int idx)
{
	//Idx 체크
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
	//예외처리 - 수용인원보다 더 많은 클라이언트가 접속 시도시


	// 데이터 통신에 사용할 변수
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

	// 접속한 클라이언트 정보 출력
	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	FClientSocket* clientSocket = new FClientSocket();
	clientSocket->sock = client_sock;
	clientSocket->addr = clientaddr;

	ClientSockets.push_back(clientSocket);
}

bool MultiThreadServer::receiveData(FClientSocket * cs)
{
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


	return true;
}

bool MultiThreadServer::sendData(FClientSocket * cs)
{
	
	//문구 추가
	addAditionalText(cs->buf, " from Server", cs->retval);

	// 데이터 보내기
	cs->retval = send(cs->sock, cs->buf, cs->retval, 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("send()");
		return false;
	}
	return true;
}

void MultiThreadServer::addAditionalText(char * inputBuf, const char * text, int & retval)
{
	if (!inputBuf) { printf("nullptr\n"); return; }

	

	strcat_s(inputBuf, BUFSIZE, text);

	
	retval = strlen(inputBuf);


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
