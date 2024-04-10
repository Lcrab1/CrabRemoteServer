#include <iostream>
#include <Windows.h>
#include <tchar.h>
using namespace std;

//北邮 北航    【【】】【【】】  中科院 

DWORD WINAPI ThreadProcedure(LPVOID ParameterData);
DWORD WINAPI ThreadProcedure1(LPVOID ParameterData);


//匿名  4参数NULL
//命名

//事件:  线程相互通知
int _tmain()//主thread
{



	HANDLE v10 = NULL;
	v10 = CreateEvent(NULL, true, true, NULL);	//创建一个授信的时间
	//第三个参数 若为true，说明初始状态为授信，若为false，说明初始状态为非授信
	DWORD v20 = WaitForSingleObject(v10, INFINITE);
	//若第二个参数为false，那么系统在执行完WaitForSingleObject后，会将授信状态改为非授信状态
	//授信状态被改为非授信状态，那么WaitForSingleObject此时就会永久等待

	//若第二个参数为true，WaitForSingleObject不会修改授信状态，始终处于授信
	//v20 = WaitForSingleObject(v10, INFINITE);
	_tprintf(_T("Hello World\r\n"));

	//若第二个参数为true，那么需要手动改变授信状态,WaitForsingleObject并不会
	//手动改变授信可以用 ResetEvent 将授信 改为非授信
	ResetEvent(v10);
	v20 = WaitForSingleObject(v10, INFINITE);
	_tprintf(_T("Hello World\r\n"));







	int i = 0;

	int x = 10000;

	HANDLE ThreadHandle[2] = { NULL };
	DWORD  ThreadIdentity[2] = { 0 };

	HANDLE EventHandle[2] = { NULL };


	//2  false  自动
	//3  false  初始化状态 不授信
	for (i = 0; i < 2; i++)
	{
		EventHandle[i] = CreateEvent(NULL, false, false, NULL);
		//创建带有事件的线程 
		ThreadHandle[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcedure, &EventHandle[i], 0, &ThreadIdentity[i]);

		if (ThreadHandle[i] == NULL)
		{
			goto Error;
		}
	}


	Sleep(5000);  //主线程停顿5秒

	SetEvent(EventHandle[0]);   //设置为授信状态

	Sleep(5000);

	SetEvent(EventHandle[1]);

	WaitForMultipleObjects(2, ThreadHandle, true, INFINITE);   //等待所有线程完成之后
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

	_tprintf(_T("CurrentThreadId:%d\r\n"), GetCurrentThreadId());
}

DWORD WINAPI ThreadProcedure(LPVOID ParameterData)
{
	int j = 0;
	HANDLE EventHandle = *((HANDLE*)ParameterData);
	BOOL IsWorking = TRUE;
	while (IsWorking == TRUE)
	{
		//之前第一个参数是线程句柄，当线程句柄执行完毕时，继续往下执行
		
		//现在的第一个参数是事件句柄，当事件授信时，继续往下执行
		int v1 = WaitForSingleObject(EventHandle, 1000);   //主线程设置事件状态   毫秒单位

		switch (v1)
		{
		case WAIT_OBJECT_0:
		{
			//事件授信
			IsWorking = FALSE;
			break;
		}

		case WAIT_FAILED:
		{
			//日志记录  
			IsWorking = FALSE;
			break;
		}

		case WAIT_TIMEOUT:
		{

			//超时

		}
		}
		_tprintf(_T("CurrentThreadId:%d\r\n"), GetCurrentThreadId());
	}


	return 0;
}