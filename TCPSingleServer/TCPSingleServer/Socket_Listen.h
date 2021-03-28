#ifndef SOCKET_LISTEN_H_
#define SOCKET_LISTEN_H_

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include<time.h>
#include<chrono>



class Socket_Listen
{
public:
	Socket_Listen();
public:

	const int GetRetval() { return retval; }

protected:

	int retval;
	SOCKET listen_sock;


};



#endif


