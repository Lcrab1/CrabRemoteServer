#pragma once
#include"MemoryEditorDlg.h"

// CValueEditor 对话框

class CValueEditor : public CDialogEx
{
	DECLARE_DYNAMIC(CValueEditor)

public:
	CValueEditor(CWnd* pParent, CString* Address, CIocpServer*
		IocpServer, CONTEXT_OBJECT* ContextObject);   // 标准构造函数
	virtual ~CValueEditor();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHANGE_VALUE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CString* m_StrAdddress;
	CMemoryEditorDlg* m_Parent = nullptr;

	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;
	int GetInputValue();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedNewValueButton();
	CEdit m_CurrentAddressEdit;
	CEdit m_NewValueEdit;
};
