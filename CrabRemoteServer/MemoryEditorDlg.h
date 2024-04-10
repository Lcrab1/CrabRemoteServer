#pragma once
#include"IocpServer.h"
#include"Common.h"
#include"ProcessManagerDlg.h"

// CMemoryEditorDlg 对话框

class CMemoryEditorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMemoryEditorDlg)

public:
	CMemoryEditorDlg(CWnd* pParent, CIocpServer*
		IocpServer, CONTEXT_OBJECT* ContextObject);   // 标准构造函数
	virtual ~CMemoryEditorDlg();



// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEMORY_EDITOR_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ProcessID;
	CEdit m_InputValue;
	CListCtrl m_AddressList;
	afx_msg void OnBnClickedFirstScanButton();
	afx_msg void OnBnClickedSecondScanButton();
	afx_msg void OnBnClickedUndoScanButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkMemoryEditorList(NMHDR* pNMHDR, LRESULT* pResult);

public:
	int GetInputValue();
	void ShowAddressInfoList();
	void OnInitAddressList();
	void UndoScan();
	//LRESULT OnOpenValueEditorDialog(WPARAM Parameter1, LPARAM Parameter2);
public:
	HANDLE m_ProcessIDValue;
	int m_CurrentValue;
	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;

	
};
