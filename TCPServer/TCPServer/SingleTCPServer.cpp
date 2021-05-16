
#include "SingleTCPServer.h"



USingleTCPServer::USingleTCPServer()
{
	
}


int USingleTCPServer::RunServer()
{// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	
	createServerSocket();
	
	

	//접속 해제용
	//vector<FClientSocket*>::iterator eraseIndex;
	FClientSocket* eraseIndex=nullptr;
	bool bResult = true;
	int n_Disconnect = 0;

	while (1)
	{
		
		n_Disconnect = 0;
		bResult = true;
		//항상 2개를 연결 시킬 것
		int reconnect = n_Client - ClientSockets.size();
		
		if(reconnect!=0)
		for (int i = 0; i < reconnect; ++i)
		{
			printf("연결 대기중........%d / %d\n", ClientSockets.size(), n_Client);
			acceptSocket(&ServerSockets[0]);
		}
		printf("연결 완료\n");
		

		//데이터 송수신 상태
		while (1)
		{
			//문제가 생기면 송수신 중단
			if (!bResult)break;

			//수신
			for (auto it : ClientSockets)
				if (receiveData(it) == false)
				{
					bResult = false;
					eraseIndex = it;
					++n_Disconnect;
					
				}

		
			//송신
			for (auto it : ClientSockets)
				if (sendData(it) == false) 
				{ 
					bResult = false;
					eraseIndex = it;
					++n_Disconnect;
				}

		}
		//한개가 해제 될 경우
		if (n_Disconnect == 1)
		{
		// closesocket()
			closesocket(eraseIndex->sock);
			printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(eraseIndex->addr.sin_addr), ntohs(eraseIndex->addr.sin_port));
		
	
			for (unsigned int i=0; i < ClientSockets.size(); ++i)
			{
				if (ClientSockets[i] == eraseIndex)
				{
					delete(ClientSockets[i]);
					eraseIndex = nullptr;
					ClientSockets.erase(ClientSockets.begin()+i);
					//printf("Erase ClientSocket\n"); //출력 확인
				}
			}
			printf("dis 1\n");
			
		}
		//2개 인 경우 다 지운다
		if (n_Disconnect == 2)
		{
			for (auto it : ClientSockets)
			{
				closesocket(it->sock);
				printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
				inet_ntoa(it->addr.sin_addr), ntohs(it->addr.sin_port));
				delete(it);
				it = nullptr;
			}
			ClientSockets.clear();
			printf("dis 2\n");
		}

		
		
	}
	
	// closesocket()
	closesocket(ServerSockets[0]);

	// 윈속 종료
	WSACleanup();
	return 0;
}




void USingleTCPServer::createServerSocket()
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
	FClientSocket* clientSocket=new FClientSocket();
	clientSocket->sock = client_sock;
	clientSocket->addr = clientaddr;

	ClientSockets.push_back(clientSocket);
}

bool USingleTCPServer::receiveData(FClientSocket* cs)
{
	// 데이터 받기
	cs->retval = recv(cs->sock, cs->buf_Message, BUFSIZE, 0);
	if (cs->retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (cs->retval == 0)
		return false;


	// 받은 데이터 출력
	cs->buf_Message[cs->retval] = '\0';
	printf("[TCP/%s:%d] %s\n", inet_ntoa(cs->addr.sin_addr),
		ntohs(cs->addr.sin_port), cs->buf_Message);
	

	return true;
}

bool USingleTCPServer::sendData(FClientSocket * cs)
{
	

	//문구 추가
	addAditionalText(cs->buf_Message, " from Server", cs->retval);

	// 데이터 보내기
	cs->retval = send(cs->sock, cs->buf_Message, cs->retval, 0);
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


void USingleTCPServer::printCurrentTime()
{
	auto stime = std::chrono::system_clock::now();
	auto mill = std::chrono::duration_cast<std::chrono::milliseconds>(stime.time_since_epoch());

	long long currentTimeMillis = mill.count();
	int msc = currentTimeMillis % 1000;

	struct tm t;
	time_t timer; // 시간측정
	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	localtime_s(&t, &timer); // 초 단위의 시간을 분리하여 구조체에 넣기 
	printf("현재 시간은 "); printf("%d년 %d월 %d일 %d시 %d분 %d초 %d milsec입니다.\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,msc);



}




