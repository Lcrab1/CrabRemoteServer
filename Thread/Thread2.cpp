#include <iostream>
#include <Windows.h>
#include <tchar.h>
using namespace std;



DWORD WINAPI ThreadProcedure1(LPVOID ParameterData);

int __g1 = 0;   //execulsive
int __g2 = 0;
CRITICAL_SECTION  __CriticalSection;   //线程同步
int _tmain()//主thread
{

	int i = 0;
	HANDLE ThreadHandle[2] = { NULL };
	DWORD  ThreadIdentity[2] = { 0 };





	InitializeCriticalSection(&__CriticalSection);  //初始化
	for (i = 0; i < 2; i++)
	{
		ThreadHandle[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcedure1, NULL, 0, &ThreadIdentity[i]);

		if (ThreadHandle[i] == NULL)
		{
			goto Error;
		}
	}
	WaitForMultipleObjects(2, ThreadHandle, true, INFINITE);   //等待所有线程完成之后


	_tprintf(_T("%d   %d\r\n"), __g1, __g2);
Error:

	for (i = 0; i < 2; i++)
	{
		if (ThreadHandle[i] != NULL)
		{
			CloseHandle(ThreadHandle[i]);
			ThreadHandle[i] = NULL;
		}
	}
	_tprintf(_T("Good Bye\r\n"));


	DeleteCriticalSection(&__CriticalSection);

}

DWORD WINAPI ThreadProcedure1(LPVOID ParameterData)
{


	int i = 0;
	EnterCriticalSection(&__CriticalSection);
	for (i = 0; i < 100000; i++)
	{

		__g1++;
		__g2++;
	}
	LeaveCriticalSection(&__CriticalSection);


	return 0;
}
