#pragma once
#include <Windows.h>
#include <iostream>
#include <winsock2.h>    //通信Socket2  套接字  
#include"CArray.h"
#include"CriticalSection.h"
#include <mstcpip.h>
#include "zconf.h"
#include "zlib.h"
using namespace std;
#pragma comment(lib,"Ws2_32.lib")   //kernel32.dll User32.dll  Ws2_32.dll 加载这个库为使用通信接口
#pragma comment(lib,"zlib.lib") 
#define WORK_THREAD_MAX 2
#define PACKET_LENGTH 0x2000


enum PACKET_TYPE
{
	IO_INITIALIZE,	//初始化数据包
	IO_RECEIVE,		//接收数据包
	IO_SEND,		//发送数据包
	IO_IDLE			//没有数据包正在被使用，网络处于空闲状态
};

class COVERLAPPEDEX
{
public:

	OVERLAPPED			m_Overlapped;   //真正的重叠结构  当前异步IO请求得到完成我们的代码能够响应
	PACKET_TYPE			m_PackType;     //枚举  4  4 

	COVERLAPPEDEX(PACKET_TYPE PackType)
	{
		ZeroMemory(this, sizeof(COVERLAPPEDEX));	//将整个COVERLAPPEDEX的值清零
		m_PackType = PackType;						//设置 m_PackType的状态
	}
};


#define PACKET_LENGTH 0x2000
#define PACKET_FLAG_LENGTH     5       //Shine
#define PACKET_HEADER_LENGTH   13      //Shine[数据包总长(4)][原始数据总长(4)]

typedef struct _CONTEXT_OBJECT_
{
	SOCKET clientSocket;

	WSABUF bufferReceive;
	WSABUF bufferSend;

	char bufferData[PACKET_LENGTH];


	//数据的传送使用压缩算法
	CArray_ m_ReceivedBufferDataCompressed;
	CArray_ m_ReceivedBufferDataDecompressed;
	CArray_ m_SendBufferDataCompressed;

	int DialogID;
	HANDLE DialogHandle;

	VOID MemberInit()
	{
		clientSocket = INVALID_SOCKET;
		memset(bufferData, 0, sizeof(char) * PACKET_LENGTH);
		memset(&bufferReceive, 0, sizeof(WSABUF));
		memset(&bufferSend, 0, sizeof(WSABUF));
		DialogHandle = NULL;
		DialogID = 0;

	}

	//判读如果下面的两个成员有数据说明是二级窗口
	int				  DlgIdentity;
	HANDLE            DlgHandle;   //HWND(用户对象) HANDLE(内核对象) 
	HANDLE            EditorDlg;   //MemoryEditor窗口
	HANDLE		      VMMapDlg;

}CONTEXT_OBJECT,*PCONTEXT_OBJECT;


typedef CList<PCONTEXT_OBJECT> 	CONTEXT_LIST;      //MFC的官方的类模板

typedef void (CALLBACK* LPFN_WNDCALLBACK)(PCONTEXT_OBJECT ContextObject);  //函数指针定义 

//线程回调函数的创建法1
DWORD WINAPI WorkThreadProcedure(LPVOID ParameterData);
class CIocpServer   //IoCompletePort
{
public:
	CIocpServer();
	~CIocpServer();
	BOOL ServerRun(USHORT ListenPort, LPFN_WNDCALLBACK WndCallback);
	BOOL IocpInit();
	//线程回调函数的创建法2
	static DWORD WINAPI ListenThreadProcedure(LPVOID ParameterData);
	void OnAccept();

	PCONTEXT_OBJECT AllocateContextObject();
	PCONTEXT_OBJECT RemoveContextObject(PCONTEXT_OBJECT contextObject, LPOVERLAPPED Overlapped);

	VOID PostReceive(PCONTEXT_OBJECT);
	BOOL HandleIo(PACKET_TYPE PacketType, PCONTEXT_OBJECT ContextObject,
		DWORD NumberOfBytesTransferred, LPOVERLAPPED Overlapped);
	VOID MoveContextObjectToFreePool(CONTEXT_OBJECT* ContextObject);
	BOOL OnReceiving(PCONTEXT_OBJECT  ContextObject, DWORD BufferLength, LPOVERLAPPED Overlapped);
	VOID OnPrepareSending(CONTEXT_OBJECT* ContextObject, PBYTE BufferData, ULONG BufferLength);
	BOOL OnSending(CONTEXT_OBJECT* ContextObject, ULONG BufferLength, LPOVERLAPPED Overlapped);
public:
	//监听套接字
	SOCKET m_listenSocket;



	//线程池
	ULONG m_ThreadsPoolMin;	
	ULONG m_ThreadsPoolMax;

	//工作线程
	HANDLE m_WorkThreadHandle[WORK_THREAD_MAX];
	ULONG m_WorkThreadsCount;

	//监听线程
	HANDLE m_ListenThreadHandle;   
	HANDLE m_ListenEventHandle;

	BOOL m_Working;
	HANDLE m_KillEventHandle;    //当该事件对象授信 销毁资源

	CRITICAL_SECTION  m_CriticalSection;    //构造函数 析构函数中进行 初始化销毁
	CONTEXT_LIST  m_FreeContextList;        //内存池模板
	CONTEXT_LIST  m_ConnectContextList;     //上线用户上下背景文列表

	HANDLE m_CompletionPortHandle;          //完成端口句柄

	ULONG m_KeepAliveTime;

	volatile long m_CurrentThreadsCount;
	volatile long m_BusyThreadsCount;

	char   m_PacketHeaderFlag[PACKET_FLAG_LENGTH];
	LPFN_WNDCALLBACK m_WndCallback;
};
