
// CrabRemoteServerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CrabRemoteServer.h"
#include "CrabRemoteServerDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//自定义的消息必须是WM_USER+x
#define UM_NOTIFY_ICON_DATA  WM_USER + 1  

/*****定义我的列表数据结构体*****/
typedef struct
{
	char*	TitleData;             //列表的名称
	int		TitleWidth;            //列表的宽度
}COLUMN_DATA;

/*****Client表头*****/
COLUMN_DATA __ClientInfoList[] =
{
	{ "IP",			    120 },
	{ "Region",			60 },
	{ "Computer name / Note",	200 },
	{ "OS",		128 },
	{ "CPU",			    180 },
	{ "Camera",		    81 },
	{ "Ping",			151 }
};

/*****Server表头*****/
COLUMN_DATA __ServerInfoList[] =
{
	{ "Type",		200 },
	{ "Time",			200 },
	{ "Content",	    550 }
};

CCrabRemoteServerDlg* __ServerProjectDlg = NULL;

//静态全局变量  不能让当前的全局变量在其他的cpp文件中被使用
static UINT __Indicators[] =
{
	IDR_STATUSBAR_SERVER_STRING
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSuspendProcess();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCrabRemoteServerDlg 对话框



CCrabRemoteServerDlg::CCrabRemoteServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CRABREMOTESERVER_DIALOG, pParent)
{
	m_listenPort = 0;
	m_maxConnections = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//
void CCrabRemoteServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, CRAB_CLIENT_INFORMATION_LIST, m_ClientInfoList);
	DDX_Control(pDX, CRAB_SERVER_INFORMATION_LIST, m_ServerInfoList);
}

BEGIN_MESSAGE_MAP(CCrabRemoteServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_SET_LOCAL, &CCrabRemoteServerDlg::OnMenuSetLocal)
	ON_COMMAND(ID_MENU_ADD_INFORMATION, &CCrabRemoteServerDlg::OnMenuAddInformation)
	ON_COMMAND(ID_MENU_EXIT, &CCrabRemoteServerDlg::OnMenuExit)
	ON_WM_CLOSE()

	//真彩棒的消息关联
	ON_COMMAND(ID_CMD_MANAGER, &CCrabRemoteServerDlg::OnButtonCmdManager)
	ON_COMMAND(ID_PROCESS_MANAGER, &CCrabRemoteServerDlg::OnButtonProcessManager)
	ON_COMMAND(ID_WINDOW_MANAGER, &CCrabRemoteServerDlg::OnButtonWindowManager)
	ON_COMMAND(ID_REMOTE_CONTROL, &CCrabRemoteServerDlg::OnButtonRemoteControl)
	ON_COMMAND(ID_FILE_MANAGER, &CCrabRemoteServerDlg::OnButtonFileManager)
	ON_COMMAND(ID_AUDIO_MANAGER, &CCrabRemoteServerDlg::OnButtonAudioManager)
	ON_COMMAND(ID_CLEAN_MANAGER, &CCrabRemoteServerDlg::OnButtonCleanManager)
	ON_COMMAND(ID_VIDEO_MANAGER, &CCrabRemoteServerDlg::OnButtonVideoManager)
	ON_COMMAND(ID_SERVICE_MANAGER, &CCrabRemoteServerDlg::OnButtonServiceManager)
	ON_COMMAND(ID_REGISTER_MANAGER, &CCrabRemoteServerDlg::OnButtonRegisterManager)
	ON_COMMAND(ID_SERVER_MANAGER, &CCrabRemoteServerDlg::OnButtonServerManager)
	ON_COMMAND(ID_CLIENT_MANAGER, &CCrabRemoteServerDlg::OnButtonClientManager)
	ON_COMMAND(ID_SERVER_ABOUT, &CCrabRemoteServerDlg::OnButtonServerAbout)
	ON_MESSAGE(UM_NOTIFY_ICON_DATA, (LRESULT(__thiscall CWnd::*)(WPARAM, LPARAM))OnNotifyIconData)
	ON_COMMAND(ID_SHOW_MAIN_DIALOG, &CCrabRemoteServerDlg::OnShowMainDialog)
	ON_COMMAND(ID_HIDE_MAIN_DIALOG, &CCrabRemoteServerDlg::OnHideMainDialog)
	ON_MESSAGE(UM_CLIENT_LOGIN, OnClientLogin)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, CRAB_CLIENT_INFORMATION_LIST, &CCrabRemoteServerDlg::OnNMRClickClientInformationList)
	ON_COMMAND(ID_DELETE_CONNECTION, &CCrabRemoteServerDlg::OnDeleteConnection)
	ON_COMMAND(ID_INSTANT_MESSAGE, &CCrabRemoteServerDlg::OnInstantMessage)
	ON_COMMAND(ID_REMOTE_SHUTDOWN, &CCrabRemoteServerDlg::OnRemoteShutdown)
	ON_MESSAGE(UM_OPEN_CMD_MANAGER_DIALOG, OnOpenCmdManagerDialog)
	ON_MESSAGE(UM_OPEN_REMOTE_MESSAGE_DIALOG, OnOpenInstantMessageDialog)
	ON_MESSAGE(UM_OPEN_PROCESS_MANAGER_DIALOG, OnOpenProcessManagerDialog)
END_MESSAGE_MAP()


// CCrabRemoteServerDlg 消息处理程序

BOOL CCrabRemoteServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	__ServerProjectDlg = this;       //构造函数中也可以

	/*****调用时钟事件*****/
	SetTimer(0, 1000, NULL);		//1000从现在开始每隔1秒 NULL默认调用OnTimer()函数
	getLocalTime();

	/*****初始化主窗口*****/ 
	initListCtrl();				//先调用时钟，再初始化主窗口，可以让时间瞬时显示

	/*****初始化主菜单*****/
	intiSolidMenu();

	/*****初始化真彩棒*****/
	TrueColorToolBarInit();

	/*****初始化托盘*****/
	NotifyIconDataInit();

	/*****初始化ini文件*****/
	m_configFile.ConfigFileInit();

	m_configFile.GetCongfigFileData(_T("Settings"), _T("ListenPort"), m_listenPort);
	m_configFile.GetCongfigFileData(_T("Settings"), _T("MaxConnections"), m_maxConnections);

	/*****初始化状态栏*****/
	StatusBarInit();
	ServerStart();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCrabRemoteServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCrabRemoteServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCrabRemoteServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCrabRemoteServerDlg::initListCtrl()
{
	//sizeof(__ClientInfoList) / sizeof(COLUMN_DATA) 计算ClientInfoList的列数
	for (int i = 0; i < sizeof(__ClientInfoList) / sizeof(COLUMN_DATA); i++)
	{
		//设置ClientInfoList的标题栏
		m_ClientInfoList.InsertColumn(i, __ClientInfoList[i].TitleData,
			LVCFMT_CENTER, __ClientInfoList[i].TitleWidth);
		//LVCFMT_CENTER是文字居中
	}
	m_ClientInfoList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	//sizeof(__ServerInfoList) / sizeof(COLUMN_DATA) 计算ServerInfoList的列数
	for (int i = 0; i < sizeof(__ServerInfoList) / sizeof(COLUMN_DATA); i++)
	{
		//设置ServerInfoList的标题栏
		m_ServerInfoList.InsertColumn(i, __ServerInfoList[i].TitleData, LVCFMT_CENTER,
			__ServerInfoList[i].TitleWidth);
	}
	m_ServerInfoList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}



void CCrabRemoteServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);

	getLocalTime();
}

void CCrabRemoteServerDlg::getLocalTime()
{
	auto  Object = time(NULL);   //自动变量(Time的对象)   
	tm v1;						//tm是系统定义的结构体
	char  v2[MAX_PATH];
	localtime_s(&v1, &Object);		//获取本地时间，放入v1中
	strftime(v2, _countof(v2), "%Y-%m-%d %H:%M:%S", &v1);   //获得的时间v1以该格式进行字符串格式化 放入v2中
	SetWindowText(v2);   //设置到主窗口的标题栏上
}

void CCrabRemoteServerDlg::intiSolidMenu()
{
	HMENU  Object;    //定义一个菜单对象    将数据型数据转换成字符型数据
	Object = LoadMenu(NULL, MAKEINTRESOURCE(CRAB_DIALOG_MAIN_MENU));        //对象载入菜单资源
	::SetMenu(this->GetSafeHwnd(), Object);      //将带有资源的菜单对象设置到主Dlg句柄上
	::DrawMenuBar(this->GetSafeHwnd());
}


void CCrabRemoteServerDlg::OnMenuSetLocal()
{
	// TODO: 在此添加命令处理程序代码
}


void CCrabRemoteServerDlg::OnMenuAddInformation()
{
	// TODO: 在此添加命令处理程序代码
	int i = m_ServerInfoList.InsertItem(m_ServerInfoList.GetItemCount(), "张飞");  //把张飞插入到第几排
	m_ServerInfoList.SetItemText(i, 1, "23");
	m_ServerInfoList.SetItemText(i, 2, "车骑将军");
	m_ServerInfoList.SetItemText(i, 3, "蜀");
}


void CCrabRemoteServerDlg::OnMenuExit()
{
	// TODO: 在此添加命令处理程序代码

	SendMessage(WM_CLOSE);    //调用消息函数的方法
}


void CCrabRemoteServerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_IocpServer != NULL) {
		delete m_IocpServer;
		m_IocpServer = NULL;
	}

	//关闭时钟资源
	KillTimer(0);
	//MessageBox("OnClose");
	//关闭托盘
	Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);
	CDialogEx::OnClose();
}

void CCrabRemoteServerDlg::TrueColorToolBarInit()
{
	if (!m_TrueColorToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))  //创建一个工具条  加载资源
	{

		return;
	}

	if (!m_TrueColorToolBar.LoadToolBar(IDR_SERVER_DIALOG_MAIN_TOOL_BAR))
	{
		return;
	}

	m_TrueColorToolBar.LoadTrueColorToolBar
	(
		48,    //加载真彩工具条
		IDB_SERVER_DIALOG_MAIN_BITMAP,
		IDB_SERVER_DIALOG_MAIN_BITMAP,
		IDB_SERVER_DIALOG_MAIN_BITMAP
	);  //和我们的位图资源相关联

	RECT v1, v2;   //设置矩形对象
	GetWindowRect(&v2);   //得到整个窗口的大小
	v1.left = 0;
	v1.top = 0;
	v1.bottom = 80;
	v1.right = v2.right - v2.left + 10;
	m_TrueColorToolBar.MoveWindow(&v1, TRUE);    //将真彩Bar设置到对象v1范围内

	m_TrueColorToolBar.SetButtonText(0, "终端管理");     //在位图的下面添加文件
	m_TrueColorToolBar.SetButtonText(1, "进程管理");
	m_TrueColorToolBar.SetButtonText(2, "窗口管理");
	m_TrueColorToolBar.SetButtonText(3, "桌面管理");
	m_TrueColorToolBar.SetButtonText(4, "文件管理");
	m_TrueColorToolBar.SetButtonText(5, "语音管理");
	m_TrueColorToolBar.SetButtonText(6, "系统清理");
	m_TrueColorToolBar.SetButtonText(7, "视频管理");
	m_TrueColorToolBar.SetButtonText(8, "服务管理");
	m_TrueColorToolBar.SetButtonText(9, "注册表管理");
	m_TrueColorToolBar.SetButtonText(10, "服务端设置");
	m_TrueColorToolBar.SetButtonText(11, "客户端设置");
	m_TrueColorToolBar.SetButtonText(12, "帮助");
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);  //显示

}

VOID CCrabRemoteServerDlg::StatusBarInit()
{
	//成员函数  
	//消息函数(声明  Mapping  实现)  Static函数(声明 实现)  普通函数(声明 实现 自定义消息函数)
	//MFC 成员变量  
	//

	if (!m_StatusBar.Create(this) ||
		!m_StatusBar.SetIndicators(__Indicators,
			sizeof(__Indicators) / sizeof(UINT)))                    //创建状态条并设置字符资源的ID
	{
		return;
	}
	CRect v1;    //矩形类
	GetWindowRect(&v1); //Top Left Bottom Rigth   
	v1.bottom += 1;     //没有任何意义  触发OnSize立即执行
	MoveWindow(v1);
}

VOID CCrabRemoteServerDlg::OnButtonCmdManager()
{
	BYTE	IsToken = CLIENT_CMD_MANAGER_REQUIRE;
	SendingSelectedCommand(&IsToken, sizeof(BYTE));
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonProcessManager()
{
	BYTE	IsToken = CLIENT_PROCESS_MANAGER_REQUIRE;
	SendingSelectedCommand(&IsToken, sizeof(BYTE));
}

VOID CCrabRemoteServerDlg::OnButtonWindowManager()
{
	//MessageBox("OnButtonWindowManager");
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonRemoteControl()
{
	//MessageBox("OnButtonRemoteControl");
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonFileManager()
{
	//MessageBox("OnButtonFileManager");
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonAudioManager()
{
	//MessageBox("OnButtonAudioManager");
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonCleanManager()
{
	//MessageBox("OnButtonCleanManager");
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonVideoManager()
{
	//MessageBox("OnButtonVideoManager");
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonServiceManager()
{
	//MessageBox("OnButtonServiceManager");
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonRegisterManager()
{
	//MessageBox("OnButtonRegisterManager");
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonServerManager()
{
	CServerManagerDlg Object(this);    //构造类对象    子窗口类中对父窗口类中的成员变量进行访问m_ConfigFile  (构造函数中获取类成员) 
	//m_ConfigFile = ((CServerPojectDlg*)pParent)->m_ConfigFile;
	Object.DoModal();        //显示对话框 
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonClientManager()
{
	//MessageBox("OnButtonClienManager");
	return VOID();
}

VOID CCrabRemoteServerDlg::OnButtonServerAbout()
{
	//MessageBox("OnButtonServerAbout");
	return VOID();
}

void CCrabRemoteServerDlg::NotifyIconDataInit()

{	//定义托盘类成员变量
	m_NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	m_NotifyIconData.hWnd = m_hWnd;
	m_NotifyIconData.uID = IDR_MAINFRAME;
	m_NotifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;  //气泡提示
	m_NotifyIconData.uCallbackMessage = UM_NOTIFY_ICON_DATA;   //自定义消息  与该消息要关联消息处理函数       
	m_NotifyIconData.hIcon = m_hIcon;
	CString v1 = "长安大学安全实验室";
	lstrcpyn(m_NotifyIconData.szTip, v1, sizeof(m_NotifyIconData.szTip) / sizeof(m_NotifyIconData.szTip[0]));
	Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData);         //显示托盘
}

void CCrabRemoteServerDlg::OnNotifyIconData(WPARAM wParam, LPARAM lParam)
{
	switch ((UINT)lParam)   //判断动作
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		if (!IsWindowVisible())  //当前主对话框是否是显示状态
		{
			//窗口不显示
			ShowWindow(SW_SHOW);
		}
		else
		{
			ShowWindow(SW_HIDE);
		}
		break;
	}
	case WM_RBUTTONDOWN:
	{

		//动态加载菜单
		CMenu Menu;
		Menu.LoadMenu(IDR_NOTIFY_ICON_DATA_MENU);
		CPoint Point;  //x y
		GetCursorPos(&Point); //获得鼠标位置   
		Menu.GetSubMenu(0)->TrackPopupMenu(
			TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
			Point.x, Point.y, this, NULL);

		break;
	}
	}
}


void CCrabRemoteServerDlg::OnShowMainDialog()
{
	// TODO: 在此添加命令处理程序代码
	if (!IsWindowVisible())  //当前主对话框是否是显示状态
	{
		//窗口不显示
		ShowWindow(SW_SHOW);
	}
}


void CCrabRemoteServerDlg::OnHideMainDialog()
{
	// TODO: 在此添加命令处理程序代码
	if (IsWindowVisible())  //当前主对话框是否是显示状态
	{
		//窗口不显示
		ShowWindow(SW_HIDE);
	}
}

void CCrabRemoteServerDlg::ShowMainDlgInfo(BOOL ok,CString& message)
{
	CTime Object = CTime::GetCurrentTime();        //强制调用CTime类中该函数    
	CString v1;
	CString v2 = Object.Format("%H:%M:%S");        //将获得的时间以该格式进行字符串格式化'

	if (ok)
	{
		v1 = _T("执行成功");
	}
	else
	{
		v1 = _T("执行失败");
	}
	m_ServerInfoList.InsertItem(0, v1);    //向控件中设置数据
	m_ServerInfoList.SetItemText(0, 1, v2);
	m_ServerInfoList.SetItemText(0, 2, message);	///v2需要修改
	UpdateData(FALSE);

	if (message.Find("上线") > 0)         //处理上线还是下线消息
	{
		m_ConnectionCount++;
	}
	else if (message.Find("下线") > 0)
	{
		m_ConnectionCount--;
	}
	else if (message.Find("断开") > 0)
	{
		m_ConnectionCount--;
	}

	CString v3;
	m_ConnectionCount = (m_ConnectionCount <= 0 ? 0 : m_ConnectionCount);         //防止iCount 有-1的情况
	v3.Format("有%d台主机在线", m_ConnectionCount);
	m_StatusBar.SetPaneText(0, v3);   //在状态条上显示文字
}

void CCrabRemoteServerDlg::ServerStart()
{

	//启动通信
	m_IocpServer = new CIocpServer;
	if (m_IocpServer == NULL) {
		return;
	}
	if (m_IocpServer->ServerRun(m_listenPort, WndCallback)==TRUE) {

	}


	CString v1;
	v1.Format(_T("监听端口: %d 最大连接数:%d"), m_listenPort, m_maxConnections);
	ShowMainDlgInfo(TRUE, v1);

}

VOID CCrabRemoteServerDlg::WndCallback(PCONTEXT_OBJECT ContextObject)
{
	//IocpSever类中与窗口交互的函数   

//静态成员函数
//线程回调
//窗口回调

//不能够直接调用普通的类成员函数 通过传参this窗口类句柄 

	WndHandleIo(ContextObject);   //学习点点
}

VOID CCrabRemoteServerDlg::WndHandleIo(CONTEXT_OBJECT* ContextObject)
{
	if (ContextObject == NULL)
	{
		return;
	}

	if (ContextObject->DlgIdentity > 0
		&& ContextObject->m_ReceivedBufferDataDecompressed.GetArray(0)[0]
		== CLIENT_PROCESS_MANAGER_REPLY)
	{
		switch (ContextObject->DlgIdentity)
		{

			case PROCESS_MANAGER_DIALOG:
			{
				CProcessManagerDlg* Dialog = (CProcessManagerDlg*)ContextObject->DlgHandle;
				Dialog->OnReceiveComplete();

				break;
			}


		}

		return;
	}

	if (ContextObject->DlgIdentity > 0
		&& ContextObject->DlgIdentity== CMD_MANAGER_DIALOG)
	{
			CCmdManagerDlg* Dialog = (CCmdManagerDlg*)ContextObject->DlgHandle;
			Dialog->OnReceiveComplete();
	}


	switch (ContextObject->m_ReceivedBufferDataDecompressed.GetArray(0)[0])   //[13][]
	{
	case CLIENT_LOGIN:   //用户登录请求
	{

		//调用类窗口中的非静态成员函数


		//消息函数  来实现窗口中的函数调用
		__ServerProjectDlg->PostMessageA(UM_CLIENT_LOGIN,
			NULL, (LPARAM)ContextObject);   //使用自定义消息

		break;
	}
	case CLIENT_GET_OUT_REPLY:
	{
		CancelIo((HANDLE)ContextObject->clientSocket);  //回收在当前对象上的异步请求
		closesocket(ContextObject->clientSocket);
		ContextObject->clientSocket = NULL;
		Sleep(10);
		break;
	}

	//即时消息
	case CLIENT_REMOTE_MESSAGE_REPLY:
	{

		__ServerProjectDlg->PostMessage(UM_OPEN_REMOTE_MESSAGE_DIALOG, 0, (LPARAM)ContextObject);
		break;
	}
	case CLIENT_SHUT_DOWN_REPLY:
	{

		CancelIo((HANDLE)ContextObject->clientSocket);  //回收在当前对象上的异步请求
		closesocket(ContextObject->clientSocket);
		ContextObject->clientSocket = NULL;
		Sleep(10);

		break;
	}

	case CLIENT_CMD_MANAGER_REPLY:
	{

		__ServerProjectDlg->PostMessage(UM_OPEN_CMD_MANAGER_DIALOG, 0, (LPARAM)ContextObject);

		break;    //自定义消息调用函数  
	}

	case CLIENT_PROCESS_MANAGER_REPLY:
	{
		__ServerProjectDlg->PostMessage(UM_OPEN_PROCESS_MANAGER_DIALOG, 0, (LPARAM)ContextObject);
		break;
	}



	case CLIENT_MEMORY_EDITOR_FIRST_SCAN_REPLY:
	{
		CMemoryEditorDlg* Dialog = (CMemoryEditorDlg*)ContextObject->EditorDlg;
		Dialog->ShowAddressInfoList();

		break;
	}
	case CLIENT_MEMORY_EDITOR_NEXT_SCAN_REPLY:
	{
		CMemoryEditorDlg* Dialog = (CMemoryEditorDlg*)ContextObject->EditorDlg;
		Dialog->ShowAddressInfoList();

		break;
	}
	case CLIENT_VMMAP_SYSTEM_INFO_REPLY:
	{
		//Sleep(100);
		CProcessVMMapDlg* Dialog = (CProcessVMMapDlg*)ContextObject->VMMapDlg;
		Dialog->VMShowAddressList();
		break;
	}
	case CLIENT_VMMAP_SYSTEM_INFO_UPDATE_REPLY:
	{
		CProcessVMMapDlg* Dialog = (CProcessVMMapDlg*)ContextObject->VMMapDlg;
		//Dialog->UpdateSystemInfo(Dialog);
		Dialog->UpdateSystemInfo();
		break;
	}
	}
}

LRESULT CCrabRemoteServerDlg::OnClientLogin(WPARAM wParam, LPARAM lParam)
{
	CString ClientAddress, ClientPosition, HostName, ProcessorName, IsWebCameraExist, WebSpeed, OsName;
	CONTEXT_OBJECT* ContextObject = (CONTEXT_OBJECT*)lParam;         //注意这里的  ClientContext  正是发送数据时从列表里取出的数据
	if (ContextObject == NULL)
	{
		return -1;
	}
	CString	v1;
	try
	{
		int v20 = ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength();
		int v21 = sizeof(LOGIN_INFORMAITON);
		if (ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() != sizeof(LOGIN_INFORMAITON))
		{
			return -1;
		}
		//LoginInfo指向上下文对象指向的数据的首地址
		LOGIN_INFORMAITON* LoginInfo =
			(LOGIN_INFORMAITON*)ContextObject->m_ReceivedBufferDataDecompressed.GetArray();
		sockaddr_in     v2;
		memset(&v2, 0, sizeof(v2));

		//分析客户端的IP地址
		int v3 = sizeof(sockaddr_in);
		getpeername(ContextObject->clientSocket, (SOCKADDR*)&v2, &v3);
		ClientAddress = inet_ntoa(v2.sin_addr);

		//主机名称
		HostName = LoginInfo->HostName;

		switch (LoginInfo->OsVersionInfoEx.dwPlatformId)
		{

		case VER_PLATFORM_WIN32_NT:
			if (LoginInfo->OsVersionInfoEx.dwMajorVersion <= 4)
			{
				OsName = "WindowsNT";
			}

			if (LoginInfo->OsVersionInfoEx.dwMajorVersion == 5 && LoginInfo->OsVersionInfoEx.dwMinorVersion == 0)
			{
				OsName = "Windows2000";
			}

			if (LoginInfo->OsVersionInfoEx.dwMajorVersion == 5 && LoginInfo->OsVersionInfoEx.dwMinorVersion == 1)
			{
				OsName = "WindowsXP";
			}

			if (LoginInfo->OsVersionInfoEx.dwMajorVersion == 5 && LoginInfo->OsVersionInfoEx.dwMinorVersion == 2)
			{
				OsName = "Windows2003";
			}

			if (LoginInfo->OsVersionInfoEx.dwMajorVersion == 6 && LoginInfo->OsVersionInfoEx.dwMinorVersion == 0)
			{
				OsName = "WindowsVista";
			}
			if (LoginInfo->OsVersionInfoEx.dwMajorVersion == 6 && LoginInfo->OsVersionInfoEx.dwMinorVersion == 1)
			{
				OsName = "Windows7";
			}

			if (LoginInfo->OsVersionInfoEx.dwMajorVersion == 6 && LoginInfo->OsVersionInfoEx.dwMinorVersion == 2)
			{
				OsName = "Windows10";
			}

		}
		//CPU
		ProcessorName = LoginInfo->ProcessorName;
		//网速
		WebSpeed.Format("%d", LoginInfo->WebSpeed);
		//有无摄像头
		IsWebCameraExist = LoginInfo->IsWebCameraExist ? "有" : "无";


		//数据分析

		//向控件添加数据
		AddClientInfo(ClientAddress, ClientPosition,
			HostName, OsName, ProcessorName,
			IsWebCameraExist, WebSpeed, ContextObject);    //最后一个参数 不显示 为了Socket
		//ContextObject 是放在控件中的隐藏项中
	}
	catch (...) {}

}

VOID CCrabRemoteServerDlg::AddClientInfo(CString ClientAddress, CString ClientPosition,
	CString HostName,
	CString OsName, CString ProcessorNameString, CString IsWebCameraExist,
	CString WebSpeed, CONTEXT_OBJECT* ContextObject)
{
	//默认为0行  这样所有插入的新列都在最上面
	int i = m_ClientInfoList.InsertItem(m_ClientInfoList.GetItemCount(),
		ClientAddress);
	m_ClientInfoList.SetItemText(i, 2, HostName);
	m_ClientInfoList.SetItemText(i, 3, OsName);
	m_ClientInfoList.SetItemText(i, 4, ProcessorNameString);
	m_ClientInfoList.SetItemText(i, 5, IsWebCameraExist);
	m_ClientInfoList.SetItemText(i, 6, WebSpeed);
	m_ClientInfoList.SetItemData(i, (ULONG_PTR)ContextObject);  //插入到该排的隐藏区  删除

	ShowMainDlgInfo(TRUE, ClientAddress + "主机上线");
}

void CCrabRemoteServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_StatusBar.m_hWnd != NULL)                         //状态栏
	{
		CRect Rect;
		Rect.top = cy - 20;
		Rect.left = 0;
		Rect.right = cx;
		Rect.bottom = cy;
		m_StatusBar.MoveWindow(Rect);
		m_StatusBar.SetPaneInfo(0, m_StatusBar.GetItemID(0), SBPS_POPOUT, cx);
	}
}


void CCrabRemoteServerDlg::OnNMRClickClientInformationList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CMenu Menu;
	Menu.LoadMenu(IDR_CLIENT_INFO_LIST_MENU);
	CPoint Point;  //x y
	GetCursorPos(&Point); //获得鼠标位置   
	Menu.GetSubMenu(0)->TrackPopupMenu(
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		Point.x, Point.y, this, NULL);


	*pResult = 0;
}


void CCrabRemoteServerDlg::OnDeleteConnection()
{
	// TODO: 在此添加命令处理程序代码
		//发送数据到客户端
	BYTE IsToken = CLIENT_GET_OUT_REQUIRE;
	SendingSelectedCommand(&IsToken, sizeof(BYTE));   //构建客户端数据包

	//清除ListControl列表
	CString  ClientAddress;
	int SelectedCount = m_ClientInfoList.GetSelectedCount();
	int i = 0;
	for (i = 0; i < SelectedCount; i++)
	{
		POSITION Position = m_ClientInfoList.GetFirstSelectedItemPosition();
		int Item = m_ClientInfoList.GetNextSelectedItem(Position);
		ClientAddress = m_ClientInfoList.GetItemText(Item, 0);   //第几排  第0列
		//销毁列表项
		m_ClientInfoList.DeleteItem(Item);
		ClientAddress += "强制断开";
		ShowMainDlgInfo(TRUE, ClientAddress);
	}

}


void CCrabRemoteServerDlg::OnInstantMessage()
{
	// TODO: 在此添加命令处理程序代码
	BYTE IsToken = CLIENT_REMOTE_MESSAGE_REQUIRE;
	SendingSelectedCommand(&IsToken, sizeof(BYTE));
}


void CCrabRemoteServerDlg::OnRemoteShutdown()
{
	// TODO: 在此添加命令处理程序代码
	BYTE IsToken = CLIENT_SHUT_DOWN_REQUIRE;
	SendingSelectedCommand(&IsToken, sizeof(BYTE));


	CString  ClientAddress;
	int SelectedCount = m_ClientInfoList.GetSelectedCount();
	int i = 0;
	for (i = 0; i < SelectedCount; i++)
	{
		POSITION Position = m_ClientInfoList.GetFirstSelectedItemPosition();
		int Item = m_ClientInfoList.GetNextSelectedItem(Position);
		ClientAddress = m_ClientInfoList.GetItemText(Item, 0);
		m_ClientInfoList.DeleteItem(Item);
		ClientAddress += "强制断开";
		ShowMainDlgInfo(TRUE, ClientAddress);
	}
}



//将数据包发送至客户端
VOID CCrabRemoteServerDlg::SendingSelectedCommand(PBYTE BufferData, ULONG BufferLength)
{

	//从ListControl上的隐藏项中选取中Context
	POSITION Position = m_ClientInfoList.GetFirstSelectedItemPosition();
	//该代码支持多项选择
	while (Position)
	{
		//获取当前position对应的item，并更新position到下一个选中项
		int	Item = m_ClientInfoList.GetNextSelectedItem(Position);
		//获得该排的隐藏数据项得到Context
		CONTEXT_OBJECT* ContextObject = (CONTEXT_OBJECT*)m_ClientInfoList.GetItemData(Item);   //上线显示的函数中插入一个Context隐藏数据
		//通信类负责发送数据
		m_IocpServer->OnPrepareSending(ContextObject, BufferData, BufferLength);

	}
}

LRESULT CCrabRemoteServerDlg::OnOpenInstantMessageDialog(WPARAM ParameterData1, LPARAM ParameterData2)
{
	//创建一个远程消息对话框
	PCONTEXT_OBJECT ContextObject = (CONTEXT_OBJECT*)ParameterData2;

	//动态窗口
	CInstantMessageDlg* Dialog = new CInstantMessageDlg(this, m_IocpServer, ContextObject);
	// 设置父窗口为卓面
	Dialog->Create(IDD_INSTANT_MESSAGE_DIALOG, GetDesktopWindow());    //创建非阻塞的Dlg
	Dialog->ShowWindow(SW_SHOW);

	return 0;
}

LRESULT CCrabRemoteServerDlg::OnOpenCmdManagerDialog(WPARAM ParameterData1, LPARAM ParameterData2)
{
	//创建一个远程消息对话框
	PCONTEXT_OBJECT ContextObject = (CONTEXT_OBJECT*)ParameterData2;

	//非阻塞对话框
	//阻塞对话框

	CCmdManagerDlg* Dialog = new CCmdManagerDlg(this, m_IocpServer, ContextObject);
	// 设置父窗口为卓面
	Dialog->Create(IDD_CMD_MANAGER_DIALOG, GetDesktopWindow());    //创建非阻塞的Dlg
	Dialog->ShowWindow(SW_SHOW);


	if (ContextObject != NULL)
	{
		ContextObject->DlgIdentity = CMD_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dialog;
	}

	return LRESULT();
}

LRESULT CCrabRemoteServerDlg::OnOpenProcessManagerDialog(WPARAM ParameterData1, LPARAM ParameterData2)
{
	//创建一个远程消息对话框
	PCONTEXT_OBJECT ContextObject = (CONTEXT_OBJECT*)ParameterData2;

	//非阻塞对话框
	CProcessManagerDlg* Dialog = new CProcessManagerDlg(this, m_IocpServer, ContextObject);
	// 设置父窗口为卓面
	Dialog->Create(IDD_PROCESS_MANAGER_DIALOG, GetDesktopWindow());    //创建非阻塞的Dlg
	Dialog->ShowWindow(SW_SHOW);



	if (ContextObject != NULL)
	{
		ContextObject->DlgIdentity = PROCESS_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dialog;
	}

	return 0;
}