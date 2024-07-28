#pragma once
#include "IocpServer.h"
#include"Common.h"
// CRegisterManagerDlg 对话框
struct PACKET_HEADER {
	int   nameCount;          //名字个数
	DWORD nameSize;          //名字大小
	DWORD nameValue;          //值大小

};
class CRegisterManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegisterManagerDlg)

public:
	CRegisterManagerDlg(CWnd* pParent = NULL, CIocpServer* IocpServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // 标准构造函数
	virtual ~CRegisterManagerDlg();
	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;
	HTREEITEM	m_Root;
	HTREEITEM	m_CurrentUser;
	HTREEITEM	m_LocalMachine;
	HTREEITEM	m_Users;
	HTREEITEM	m_CurrentConfig;
	HTREEITEM	m_ClassRoot;
	HTREEITEM   m_Selected;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISTER_MANAGER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_RegisterShowTree;
	CListCtrl m_RegisterShowList;
	HICON   m_IconHwnd;
	CImageList	m_ImageList1;   //List控件上的图标
	CImageList	m_ImageList2;   //List控件上的图标
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
//	afx_msg void OnTvnItemChangedRegisterShowTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnSelchangedRegisterShowTree(NMHDR* pNMHDR, LRESULT* pResult);

	BOOL m_IsEnable = FALSE;
	void OnReceiveComplete(void);
	CString GetFullPath(HTREEITEM Selected);
	char GetFatherPath(CString& FullPath);
	void AddPath(char* BufferData);
	void AddKey(char* BufferData);
};
