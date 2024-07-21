#pragma once


// CFileTransferModeDlg 对话框

class CFileTransferModeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileTransferModeDlg)

public:
	CFileTransferModeDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFileTransferModeDlg();
	CString m_FileFullPath;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_TRANSFER_MODE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg	void OnEndDialog(UINT id);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};
