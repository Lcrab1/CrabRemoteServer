#pragma once
#include"IocpServer.h"
#include"Common.h"
// CServiceManager 对话框

class CServiceManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServiceManagerDlg)

public:
	CServiceManagerDlg(CWnd* pParent = NULL, CIocpServer* IocpServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // 标准构造函数
	virtual ~CServiceManagerDlg();
	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVICE_MANAGER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ServiceInfoList;
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	void OnReceiveComplete(void);
	int ShowClientServiceList(void);
	afx_msg void OnNMRClickServiceInfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRefreshService();
	afx_msg void OnStartService();
	afx_msg void OnStopService();
	afx_msg void OnAutoStartService();
	afx_msg void OnManualStartService();
	void ConfigClientService(BYTE IsMethod);
};
