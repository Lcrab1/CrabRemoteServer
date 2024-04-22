// CreateProcessDlg.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "CreateProcessDlg.h"
#include "afxdialogex.h"


// CCreateProcessDlg 对话框

IMPLEMENT_DYNAMIC(CCreateProcessDlg, CDialogEx)

CCreateProcessDlg::CCreateProcessDlg(CWnd* pParent, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	: CDialogEx(IDD_CREATE_PROCESS_DIALOG, pParent)
{
	m_IocpServer = IocpServer;
	m_ContextObject = ContextObject;
}

CCreateProcessDlg::~CCreateProcessDlg()
{
}

void CCreateProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCESS_CREATE_EDIT, m_ProcessName);
}


BEGIN_MESSAGE_MAP(CCreateProcessDlg, CDialogEx)
	ON_BN_CLICKED(IDC_PROCESS_CREATE_BUTTON, &CCreateProcessDlg::OnBnClickedProcessCreateButton)
//	ON_WM_CLOSE()
ON_WM_CLOSE()
//ON_COMMAND(ID_32823, &CCreateProcessDlg::OnVMMap)
END_MESSAGE_MAP()


// CCreateProcessDlg 消息处理程序


BOOL CCreateProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CCreateProcessDlg::OnBnClickedProcessCreateButton()
{
	// TODO: 在此添加控件通知处理程序代码
	

	CString textContent;
	m_ProcessName.GetWindowTextA(textContent);
	int bufferLength = textContent.GetLength() + sizeof(BYTE);

	LPBYTE bufferData = new BYTE[bufferLength];
	//char* processName = new char[textContent.GetLength() + 1];
	//memcpy(processName, textContent, textContent.GetLength());
	//加入结束进程的数据头
	//memcpy source需要是c字符串
	bufferData[0] = CLIENT_PROCESS_MANAGER_CREATE_REQUIRE;
	memcpy(bufferData + sizeof(BYTE), textContent, textContent.GetLength());
	//memcpy(bufferData + sizeof(BYTE), processName, textContent.GetLength());

	m_IocpServer->OnPrepareSending(m_ContextObject, bufferData, bufferLength);
	
}


//void CCreateProcessDlg::OnClose()
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	__CProcessManagerDlg->PostMessage(WM_COMMAND, MAKEWPARAM(ID_REFRESH_PROCESS, BN_CLICKED));
//	CDialogEx::OnClose();
//}


void CCreateProcessDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	__CProcessManagerDlg->PostMessage(WM_COMMAND, MAKEWPARAM(ID_REFRESH_PROCESS, BN_CLICKED));
	CDialogEx::OnClose();
}


//void CCreateProcessDlg::OnVMMap()
//{
//	// TODO: 在此添加命令处理程序代码
//}
