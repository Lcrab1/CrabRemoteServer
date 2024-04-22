#pragma once
#include"IocpServer.h"
#include"Common.h"
#include"CreateProcessDlg.h"
#include"CrabRemoteServerDlg.h"
#include"MemoryEditorDlg.h"
#include"ProcessVMMapDlg.h"
// CProcessManagerDlg 对话框

class CProcessManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessManagerDlg)

public:
	CProcessManagerDlg(CWnd* pParent = NULL, CIocpServer* IocpServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // 标准构造函数
	virtual ~CProcessManagerDlg();

	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS_MANAGER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	HICON   	m_IconHwnd;   //用户对象  内核对象   HWND  HANDLE  都是对象 
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	CListCtrl m_ProcessInfoList;
	void GetProcessInfoList();
	void ShowProcessInfoList(void);
	void OnReceiveComplete(void);
	//CCreateProcessDlg* m_CreateProcessDlg;

	afx_msg void OnRefreshProcess();
	afx_msg void OnKillProcess();
	afx_msg void OnSuspendProcess();
	afx_msg void OnResumeProcess();
	afx_msg void OnProcessModules();
	afx_msg void OnProcessThreads();
	afx_msg void OnProcessMemory();
	afx_msg void OnProcessHandles();
	afx_msg void OnRclickProcessInfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawProcessInfoList(NMHDR* pNMHDR, LRESULT* pResult);

	void InitializeSolidMenu();
	afx_msg void OnProcessCreate();

	LRESULT OnOpenProcessCreateDialog(WPARAM ParameterData1, LPARAM ParameterData2);
	LRESULT OnOpenMemoryEditorDialog(WPARAM ParameterData1, LPARAM ParameterData2);
	afx_msg void OnProcessVMMap();
	LRESULT OnOpenVMMapDialog(WPARAM processID, LPARAM contextObject);
};

extern CProcessManagerDlg* __CProcessManagerDlg;