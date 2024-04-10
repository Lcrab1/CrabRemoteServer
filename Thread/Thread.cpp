#include <iostream>
#include <Windows.h>
#include <tchar.h>
using namespace std;

//北邮 北航    【【】】【【】】  中科院 

DWORD WINAPI ThreadProcedure(LPVOID ParameterData);
DWORD WINAPI ThreadProcedure1(LPVOID ParameterData);
int _tmain()//主thread
{
	//主线程消亡 进程Over
	//默认线程Stack 1M
	int i = 0;

	//int x = 10000;
	//DWORD ThreadIdentity = 0;
	//NULL
	//0   1M
	//NULL  主线程通过该参数可以向子线程栈区传递数据
	//HANDLE ThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcedure, &x, 0, &ThreadIdentity);
	HANDLE ThreadHandle[5] = { NULL };
	DWORD  ThreadIdentity[5] = { 0 };
	for (i = 0; i < 5; i++)
	{
		ThreadHandle[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcedure1, NULL, 0, &ThreadIdentity[i]);

		if (ThreadHandle[i] == NULL)
		{
			goto Error;
		}
	}



	/*
	for (i=0;i<100;i++)
	{
		_tprintf(_T("i = %d\r\n"), i);
	}*/

	//等待所有子线程执行完毕
	//WaitForSingleObject(ThreadHandle, INFINITE);  //直到线程句柄授信才能向下执行 否则一直等待
	WaitForMultipleObjects(5, ThreadHandle, false, INFINITE);   //等待所有线程完成之后
Error:

	for (i = 0; i < 5; i++)
	{
		if (ThreadHandle[i] != NULL)
		{
			CloseHandle(ThreadHandle[i]);
			ThreadHandle[i] = NULL;
		}
	}




	_tprintf(_T("Good Bye\r\n"));

	_tprintf(_T("CurrentThreadId:%d\r\n"), GetCurrentThreadId());
}

DWORD WINAPI ThreadProcedure(LPVOID ParameterData)
{

	int x = *((int*)ParameterData);
	int j = 0;
	for (;;)
	{
		_tprintf(_T("x = %d\r\n"), x);
	}


	return 0;
}



DWORD WINAPI ThreadProcedure1(LPVOID ParameterData)
{


	_tprintf(_T("CurrentThreadId:%d\r\n"), GetCurrentThreadId());


	return 0;
}
