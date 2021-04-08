/*
	ThreadAdderTwo.cpp
	프로그램 설명: 전역변수를 이용한 쓰레드 기반 Adder.
*/

#include <iostream>
#include <windows.h>
#include <tchar.h>

using namespace std;
static int total = 0;

DWORD WINAPI ThreadProc( LPVOID lpParam ) 
{ 
	DWORD * nPtr = (DWORD *) lpParam;

	DWORD numOne = *nPtr;
	DWORD numTwo = *(nPtr+3);

	/*for(DWORD i=numOne; i<=numTwo; i++)
	{
		total += i;
	}*/

	for (DWORD i = 0; i < 4; ++i)
	{
		//cout << "its = " << *(nPtr + i) << endl;
		total += *(nPtr + i);
	}

    return 0; // 정상적 종료.
}






/*
	12개의 숫자를 입력 받아서 3개의 쓰레드를 이용하여 합산하라
*/

int _tmain(int argc, TCHAR* argv[])
{
    DWORD dwThreadID[3];
    HANDLE hThread[3];

	DWORD paramThread[] = {1, 3, 4, 7, 8, 10};

	DWORD *paramArray = new DWORD[12];

	printf("intput 12 nums : ");
	int num=0;
	for (int i = 0; i < 12; ++i)
	{
		
		cin>>num;
		paramArray[i] = num;
	}
	printf("\n");

    hThread[0] = 
		CreateThread ( 
				NULL, 0,
				ThreadProc,
				(LPVOID)(&paramArray[0]),
				0, &dwThreadID[0]
		    );			                
 
    hThread[1] = 
		CreateThread ( 
				NULL, 0,
				ThreadProc,
				(LPVOID)(&paramArray[4]),
				0, &dwThreadID[1]
		    );			                
 
    hThread[2] = 
		CreateThread ( 
				NULL, 0,
				ThreadProc,
				(LPVOID)(&paramArray[8]),
				0, &dwThreadID[2]
		    );
 

	if(hThread[0] == NULL || hThread[1] == NULL || hThread[2] == NULL)
	{
		_tprintf(_T("Thread creation fault! \n"));
		return -1;
	}

	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	_tprintf(_T("total : %d \n"), total);

    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);
    CloseHandle(hThread[2]);

	delete paramArray;

	return 0;
}

