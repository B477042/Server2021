
#include "SingleTCPServer.h"
// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

	// 윈속 초기화
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

	// 데이터 통신에 사용할 변수
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

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 클라이언트와 데이터 통신
		while (1) {
			// 데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;
			

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), buf);
			//printf("Retval : %d\n", retval);

			//문구 추가
			addAditionalText(buf," from Server",retval);
			
			//printf("before send : %s\n", buf);
			// 데이터 보내기
			

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
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

int USingleTCPServer::run1()
{// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	
	createSocket();
	
	

	//접속 해제용
	//vector<FClientSocket*>::iterator eraseIndex;
	FClientSocket* eraseIndex=nullptr;
	bool bResult = true;
	while (1)
	{
		//항상 2개를 연결 시킬 것
		int reconnect = 2 - ClientSockets.size();
		if(reconnect!=0)
		for (int i = 0; i < reconnect; ++i)
		{
			printf("연결 대기중........%d / %d\n", ClientSockets.size(), 2);
			acceptSocket(&ServerSockets[0]);
		}
		printf("연결 완료\n");

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
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
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

	// 윈속 종료
	WSACleanup();
	return 0;
}


int USingleTCPServer::run2()
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	// closesocket()
	closesocket(ServerSockets[0]);

	// 윈속 종료
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
	//문구 추가
	addAditionalText(cs->buf, " from Server", cs->retval);

	return true;
}

bool USingleTCPServer::sendData(FClientSocket * cs)
{
	// 데이터 보내기


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

//서버와 클라이언트가 주고 받는 시간을 정확히 알고 싶어서
//인터넷에서 검색해서 출력하는 형태를 복사했습니다
//https://jhnyang.tistory.com/305
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




