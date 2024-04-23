#pragma once
#include"IocpServer.h"
#include"Common.h"
#include<list>
// CProcessVMMapDlg 对话框

class CProcessVMMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessVMMapDlg)

public:
	CProcessVMMapDlg(CWnd* pParent, CIocpServer*
		IocpServer, CONTEXT_OBJECT* ContextObject);   // 标准构造函数
	virtual ~CProcessVMMapDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS_VMMAP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_MemoryPageSizeEdit;
//	CEdit m_TotalPhysicalMemory;
	CEdit m_AccessPhysicalMemoryEdit;
	CEdit m_TotalPhysicalMemoryEdit;
	CEdit m_MinimalAddressEdit;
	CEdit m_TotalPageFileEdit;
	CEdit m_AccessPageFileEdit;
	CEdit m_MaximumAddressEdit;
	CEdit m_TotalVirtualMemoryEdit;
	CEdit m_AccessVirtualMemory;
	CEdit m_ProcessIDEdit;
	CEdit m_ProcessPathEdit;
	CListCtrl m_MemoryList;

	CButton m_CommitCheckBox;
	CButton m_ReserveCheckBox;
	CButton m_FreeCheckBox;

public:
	void OnInitControlList();
	void VMShowAddressList();
	void VMShowSystemInfo(SYSTEM_INFO& systemInfo,MEMORYSTATUS& memoryStatus);
	void VMShowAddressInfo(PBYTE BufferData, DWORD& Offset);
	void UpdateSystemInfoRequire();
	void UpdateSystemInfo();
	//static DWORD WINAPI WorkThreadProcedure(LPVOID ParameterData);
public:
	CIocpServer*			m_IocpServer;
	PCONTEXT_OBJECT			m_ContextObject;
	static HANDLE			m_ProcessID;
	static CString          m_ProcessPath;
	list<MEMORY_BASIC_INFORMATION> m_MemoryBasicInfoList;
public:
	afx_msg void OnBnClickedVmmapRefreshButton();
	virtual BOOL OnInitDialog();
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

