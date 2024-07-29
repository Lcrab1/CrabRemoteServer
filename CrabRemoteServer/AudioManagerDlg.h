#pragma once
#include"IocpServer.h"
#include"Audio.h"
#include"Common.h"
// CAudioManager 对话框

class CAudioManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAudioManagerDlg)

public:
	CAudioManagerDlg(CWnd* pParent = NULL, CIocpServer* IocpServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);  // 标准构造函数
	virtual ~CAudioManagerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUDIO_MANAGER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton m_AudioManagerCheck;
	CIocpServer* m_IocpServer;
	HANDLE m_ThreadHandle;
	PCONTEXT_OBJECT m_ContextObject;
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	CAudio      m_Audio;
	BOOL        m_IsLoop;
	static DWORD WINAPI ThreadProcedure(LPVOID ParameterData);
	void OnReceiveComplete(void);
};
