/*
	CriticalSectionSync.cpp
	프로그램 설명: 생성 가능한 쓰레드의 개수 측정.
*/

#include <iostream>
#include <windows.h>
#include <process.h>
#include <thread>
#include <tchar.h>
using namespace std;

#define NUM_OF_GATE		3
#define NUM_OF_INPUT 12

LONG gTotalCount = 0;

CRITICAL_SECTION   hCriticalSection;

typedef struct FMultiParamStruct {
public:
	int ThreadIndex;
	DWORD* InputArrayPointer;
}MParamStruct;


void IncreaseCount(int num)
{
	EnterCriticalSection (&hCriticalSection);
	cout << "num : " << num << endl;
	gTotalCount += num;

	LeaveCriticalSection (&hCriticalSection);
}

//대충 천번 실행
unsigned int WINAPI ThreadProc( LPVOID lpParam ) 
{ 
	
	auto tempStruct = (MParamStruct *)lpParam;
	
	if (!tempStruct)
	{
		cout << "faile" << endl;
		return 0;
	}

	int sector = (NUM_OF_INPUT / NUM_OF_GATE);
	int idx_thread = tempStruct->ThreadIndex;
	cout << this_thread::get_id() << "<-- id\n";
	cout << "\n====Activate Thread Index ====" << idx_thread << endl;

	

	for(int i=sector*idx_thread; i< sector*(idx_thread+1); i++)
	{
		IncreaseCount(*(tempStruct->InputArrayPointer +i));
	}

	return 0;
} 


int _tmain(int argc, TCHAR* argv[])
{
    DWORD dwThreadId[NUM_OF_GATE];
    HANDLE hThread[NUM_OF_GATE];

	
	DWORD* InputArray = new DWORD[NUM_OF_INPUT];


	for (int i = 0; i < NUM_OF_INPUT; ++i)
	{
		int num = 0;
		cout << i + 1 << " : ";
		cin >> num;
		InputArray[i] = num;
		cout << endl;
	}
		MParamStruct* newStruct=new MParamStruct[NUM_OF_GATE];
	//	newStruct.ThreadIndex = 0;
		for (int i = 0; i < NUM_OF_GATE; ++i)
		{
			(newStruct + i)->ThreadIndex = i;
			(newStruct+i)->InputArrayPointer = InputArray;
			cout << "init Stuct index " << (newStruct + i)->ThreadIndex << endl;
		}
		

	InitializeCriticalSection(&hCriticalSection);

	for(DWORD i=0; i<NUM_OF_GATE; i++)
	{
		

		cout << "init Stuct index " << (newStruct+i)->ThreadIndex << endl;
        hThread[i] = (HANDLE)
		    _beginthreadex ( 
			    NULL,
			    0,				        
			    ThreadProc,				  
				(LPVOID)(&newStruct[i]),
			    CREATE_SUSPENDED,		   
			    (unsigned *)&dwThreadId[i]   
	        );

	    if(hThread[i] == NULL)
	    {
	        _tprintf(_T("Thread creation fault! \n"));
	        return -1;
	    }
	}

	for(DWORD i=0; i<NUM_OF_GATE; i++)
	{
		ResumeThread(hThread[i]);
	}


	WaitForMultipleObjects(NUM_OF_GATE, hThread, TRUE, INFINITE);

	_tprintf(_T("total count: %d \n"), gTotalCount);

	for(DWORD i=0; i<NUM_OF_GATE; i++)
	{
		CloseHandle(hThread[i]);
	}
  
	DeleteCriticalSection(&hCriticalSection);

	delete[] InputArray;
	delete[] newStruct;
	return 0;
}
