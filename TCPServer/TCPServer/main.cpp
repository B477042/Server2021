#include "MultiThreadServer.h"


int _tmain(int argc, TCHAR* argv[])
{
	
	cout << "hi" << endl;
	UMultiThreadServer server;
	if (server.initServer() != 0)
	{
		cout << "Fail to Init Server" << endl;
		return 0;
	}
	//서버 프로그램 실행
	server.RunMultiThreadServer();
	
	server.CloseServer();

	return 0;
}

