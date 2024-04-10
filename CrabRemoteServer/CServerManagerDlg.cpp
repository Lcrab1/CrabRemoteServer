// CServerManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "CServerManagerDlg.h"
#include "afxdialogex.h"


// CServerManagerDlg 对话框

IMPLEMENT_DYNAMIC(CServerManagerDlg, CDialogEx)

CServerManagerDlg::CServerManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_MANAGER_DIALOG, pParent)

	, m_setListenPort(0)
	, m_setMaxConnects(0)
{
	m_configFile=((CCrabRemoteServerDlg*)pParent)->m_configFile;
	m_iconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICO_SEVER_MANAGER));
}

CServerManagerDlg::~CServerManagerDlg()
{
}

void CServerManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_APPLY_BUTTON, m_applyButton);
	DDX_Text(pDX, IDC_SET_LISTEN_PORT, m_setListenPort);
	DDX_Text(pDX, IDC_SET_MAX_CONNECTS, m_setMaxConnects);
}


BEGIN_MESSAGE_MAP(CServerManagerDlg, CDialogEx)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, &CServerManagerDlg::OnBnClickedApplyButton)
END_MESSAGE_MAP()


// CServerManagerDlg 消息处理程序


void CServerManagerDlg::OnBnClickedApplyButton()
{
	UpdateData(TRUE);  //更新修改的数据
	m_configFile.SetConfigFileData(_T("Settings"), _T("ListenPort"), m_setListenPort);
	//向ini文件中写入值
	m_configFile.SetConfigFileData(_T("Settings"), _T("MaxConnections"), m_setMaxConnects);
	SendMessage(WM_CLOSE);
}


BOOL CServerManagerDlg::OnInitDialog()
{

	CDialogEx::OnInitDialog();
	SetIcon(m_iconHwnd, FALSE);
	// TODO:  在此添加额外的初始化
	int ListenPort = 0;
	int MaxConnections = 0;

	m_applyButton.ShowWindow(SW_HIDE);
	m_applyButton.EnableWindow(FALSE);
	 
	m_configFile.GetCongfigFileData(_T("Settings"), _T("ListenPort"), m_setListenPort);
	m_configFile.GetCongfigFileData(_T("Settings"), _T("MaxConnections"), m_setMaxConnects);


	//刷新窗口
	UpdateData(FALSE);



	m_applyButton.ShowWindow(SW_NORMAL);
	m_applyButton.EnableWindow(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
