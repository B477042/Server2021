#include "SingleTCPServer.h"


//
//void hi(char* p,int& a, FClientSocket* cs)
//{
//	char*i = new char;
//	char hello[] = "hello";
//	i = hello;
//	p = i;
//	a = 12;
//	printf("%s\n", p);
//	cs->retval = 3;
//	
//
//}



int main()
{
	USingleTCPServer SingleTCPServer;
	//SingleTCPServer.RunServer();
	SingleTCPServer.RunServer();

	/*char* p=nullptr;
	int a = 0;
	FClientSocket cs;
	cs.retval = 0;
	cs.buf = p;
	hi(p,a,&cs);
	printf("%s\t%d\t%d", p,a,cs.retval);*/

	return 0;
}

