#pragma once


// CProcessVMMapDlg 对话框

class CProcessVMMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessVMMapDlg)

public:
	CProcessVMMapDlg(CWnd* pParent = nullptr);   // 标准构造函数
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
};
