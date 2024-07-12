#pragma once
#include"IocpServer.h"

// CWindowManager 对话框

class CWindowManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWindowManagerDlg)

public:
	CWindowManagerDlg(CWnd* pParent = NULL, CIocpServer* IocpServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);
	virtual ~CWindowManagerDlg();
	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINDOW_MANAGER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	CListCtrl m_WindowManagerList;
	void ShowWindowInfoList(void);
	void RefreshWindowInfoList(void);

	CListCtrl m_WindowInfoList;
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnRefreshWindow();
	afx_msg void OnCloseWindow();
	afx_msg void OnHideWindow();
	afx_msg void OnRecoverWindow();
	afx_msg void OnMaxWindow();
	afx_msg void OnMinWindow();
	afx_msg void OnRclickWindowManagerList(NMHDR* pNMHDR, LRESULT* pResult);
};
