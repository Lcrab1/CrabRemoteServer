#pragma once
#include"IocpServer.h"
#include"Common.h"
// CInstantMessageDlg 对话框

class CInstantMessageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInstantMessageDlg)

public:
	CInstantMessageDlg(CWnd* pParent = NULL, CIocpServer* IocpServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);;   // 标准构造函数
	virtual ~CInstantMessageDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INSTANT_MESSAGE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_InstantMessageEdit;
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnSending();

private:
	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;
};
