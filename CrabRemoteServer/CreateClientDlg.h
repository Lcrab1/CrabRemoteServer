#pragma once


// CCreateClientDlg 对话框

class CCreateClientDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCreateClientDlg)

public:
	CCreateClientDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCreateClientDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_CLIENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_IpEdit;
	CString m_PortEdit;
	virtual BOOL OnInitDialog();
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOkButton();
	int MemoryFind(const char* BufferData, const char* KeyValue, int BufferLength, int KeyLength);

};
