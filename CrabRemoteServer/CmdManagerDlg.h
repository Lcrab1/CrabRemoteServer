#pragma once
#include"IocpServer.h"
#include"Common.h"
// CCmdManagerDlg 对话框

class CCmdManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCmdManagerDlg)

public:
	CCmdManagerDlg(CWnd* pParent = NULL, CIocpServer* IocpServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // 标准构造函数	virtual ~CCmdManagerDlg();
	virtual ~CCmdManagerDlg();
	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CMD_MANAGER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_CmdManagerEdit;
	ULONG           m_CurDataPosition = 0;   //获得当前数据所在位置;
	ULONG           m_ShowDataLength = 0;



	VOID ShowCmdData(void);
	void OnReceiveComplete(void);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
};
