/*
	ThreadAdderTwo.cpp
	���α׷� ����: ���������� �̿��� ������ ��� Adder.
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

    return 0; // ������ ����.
}






/*
	12���� ���ڸ� �Է� �޾Ƽ� 3���� �����带 �̿��Ͽ� �ջ��϶�
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

