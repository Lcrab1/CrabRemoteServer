#pragma once
#include"IocpServer.h"
#include"Common.h"
#include"ProcessManagerDlg.h"
// CCreateProcessDlg 对话框

class CCreateProcessDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCreateProcessDlg)

public:
	CCreateProcessDlg(CWnd* pParent, CIocpServer*
		IocpServer, CONTEXT_OBJECT* ContextObject);   // 标准构造函数
	virtual ~CCreateProcessDlg();

	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_PROCESS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_ProcessName;
	afx_msg void OnBnClickedProcessCreateButton();
//	afx_msg void OnClose();
	afx_msg void OnClose();
};
