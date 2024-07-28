// ServiceManager.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "ServiceManagerDlg.h"
#include "afxdialogex.h"


// CServiceManager 对话框

IMPLEMENT_DYNAMIC(CServiceManagerDlg, CDialogEx)

CServiceManagerDlg::CServiceManagerDlg(CWnd* pParent, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	: CDialogEx(IDD_SERVICE_MANAGER_DIALOG, pParent)
{
	m_IocpServer = IocpServer;
	m_ContextObject = ContextObject;
}

CServiceManagerDlg::~CServiceManagerDlg()
{
}

void CServiceManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVICE_INFO_LIST, m_ServiceInfoList);
}


BEGIN_MESSAGE_MAP(CServiceManagerDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_RCLICK, IDC_SERVICE_INFO_LIST, &CServiceManagerDlg::OnNMRClickServiceInfoList)
	ON_COMMAND(ID_REFRESH_SERVICE, &CServiceManagerDlg::OnRefreshService)
	ON_COMMAND(ID_START_SERVICE, &CServiceManagerDlg::OnStartService)
	ON_COMMAND(ID_STOP_SERVICE, &CServiceManagerDlg::OnStopService)
	ON_COMMAND(ID_AUTO_START_SERVICE, &CServiceManagerDlg::OnAutoStartService)
	ON_COMMAND(ID_MANUAL_START_SERVICE, &CServiceManagerDlg::OnManualStartService)
END_MESSAGE_MAP()


// CServiceManager 消息处理程序


BOOL CServiceManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->clientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
	v1.Format(_T("\\\\%s - 远程服务管理"), IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//设置对话框标题
	SetWindowText(v1);


	m_ServiceInfoList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ServiceInfoList.InsertColumn(0, _T("真实名称"), LVCFMT_LEFT, 150);
	m_ServiceInfoList.InsertColumn(1, _T("显示名称"), LVCFMT_LEFT, 260);
	m_ServiceInfoList.InsertColumn(2, _T("启动类型"), LVCFMT_LEFT, 80);
	m_ServiceInfoList.InsertColumn(3, _T("运行状态"), LVCFMT_LEFT, 80);
	m_ServiceInfoList.InsertColumn(4, _T("可执行文件路径"), LVCFMT_LEFT, 380);

	ShowClientServiceList();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CServiceManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_ContextObject != NULL)
	{
		m_ContextObject->ServiceDlg = NULL;
		m_ContextObject->DlgIdentity = 0;
		CancelIo((HANDLE)m_ContextObject->clientSocket);
		closesocket(m_ContextObject->clientSocket);
	}
	CDialogEx::OnClose();
}

void CServiceManagerDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(0)[0])
	{
	case CLIENT_SERVICE_MANAGER_REPLY:
	{
		ShowClientServiceList();
		break;
	}
	default:
		// 传输发生异常数据
		break;
	}
}

int CServiceManagerDlg::ShowClientServiceList(void)
{
	char* BufferData = (char*)(m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1));
	char* DisplayName;
	char* ServiceName;
	char* StartWay;
	char* AutoStart;
	char* FileFullPath;
	DWORD	Offset = 0;
	m_ServiceInfoList.DeleteAllItems();

	int i = 0;
	for (i = 0; Offset < m_ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() - 1; i++)
	{
		DisplayName = BufferData + Offset;
		ServiceName = DisplayName + lstrlen(DisplayName) + 1;
		FileFullPath = ServiceName + lstrlen(ServiceName) + 1;
		StartWay = FileFullPath + lstrlen(FileFullPath) + 1;
		AutoStart = StartWay + lstrlen(StartWay) + 1;

		m_ServiceInfoList.InsertItem(i, ServiceName);
		m_ServiceInfoList.SetItemText(i, 1, DisplayName);
		m_ServiceInfoList.SetItemText(i, 2, AutoStart);
		m_ServiceInfoList.SetItemText(i, 3, StartWay);
		m_ServiceInfoList.SetItemText(i, 4, FileFullPath);

		Offset += lstrlen(DisplayName) + lstrlen(ServiceName) + lstrlen(FileFullPath) + lstrlen(StartWay)
			+ lstrlen(AutoStart) + 5;
	}
	CString v1;
	v1.Format(_T("程序个数 / %d"), i);
	LVCOLUMN v3;
	v3.mask = LVCF_TEXT;
	v3.pszText = v1.GetBuffer(0);
	v3.cchTextMax = v1.GetLength();
	m_ServiceInfoList.SetColumn(4, &v3); //在列表中显示有多少个进程

	return 0;
}


void CServiceManagerDlg::OnNMRClickServiceInfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu Menu;
	Menu.LoadMenu(IDR_SERVICE_MANAGER_MAIN_MENU);
	CMenu* SubMenu = Menu.GetSubMenu(0);
	CPoint	Point;
	GetCursorPos(&Point);
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, Point.x, Point.y, this);
	*pResult = 0;
}


void CServiceManagerDlg::OnRefreshService()
{
	// TODO: 在此添加命令处理程序代码
	BYTE IsToken = CLIENT_SERVICE_MANAGER_REQUIRE;   //刷新
	m_IocpServer->OnPrepareSending(m_ContextObject, &IsToken, 1);
}


void CServiceManagerDlg::OnStartService()
{
	// TODO: 在此添加命令处理程序代码
	ConfigClientService(1);
}


void CServiceManagerDlg::OnStopService()
{
	// TODO: 在此添加命令处理程序代码
	ConfigClientService(2);
}


void CServiceManagerDlg::OnAutoStartService()
{
	// TODO: 在此添加命令处理程序代码
	ConfigClientService(3);
}


void CServiceManagerDlg::OnManualStartService()
{
	// TODO: 在此添加命令处理程序代码
	ConfigClientService(4);
}

void CServiceManagerDlg::ConfigClientService(BYTE IsMethod)
{
	DWORD	Offset = 2;
	POSITION Position = m_ServiceInfoList.GetFirstSelectedItemPosition();
	int	Item = m_ServiceInfoList.GetNextSelectedItem(Position);
	CString v1 = m_ServiceInfoList.GetItemText(Item, 0);

	char* ServiceName = v1.GetBuffer(0);

	LPBYTE  BufferData = (LPBYTE)LocalAlloc(LPTR, 3 + lstrlen(ServiceName));  //[][][]\0
	BufferData[0] = CLIENT_SERVICE_MANAGER_CONFIG_REQUIRE;
	BufferData[1] = IsMethod;
	memcpy(BufferData + Offset, ServiceName, lstrlen(ServiceName) + 1);
	m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, LocalSize(BufferData));
	LocalFree(BufferData);
}
