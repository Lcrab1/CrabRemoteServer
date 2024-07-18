#pragma once
#include"IocpServer.h"
#include"Common.h"
// CRemoteController 对话框

class CRemoteController : public CDialogEx
{
	DECLARE_DYNAMIC(CRemoteController)

public:
	CRemoteController(CWnd* pParent = nullptr, CIocpServer* IocpServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // 标准构造函数
	virtual ~CRemoteController();
	CIocpServer* m_IocpServer;
	PCONTEXT_OBJECT m_ContextObject;

	HDC              m_WindowDCHandle;         //工人
	HDC              m_WindowMemoryDCHandle;   //工人的工具箱
	LPBITMAPINFO     m_BitmapInfo;             //工具
	HBITMAP	         m_BitmapHandle;           //工具
	PVOID            m_BitmapData;
	POINT            m_CursorPosition;         //存储鼠标位置
	ULONG   m_HorizontalScrollPosition;
	ULONG   m_VerticalScrollPosition;
	HICON   	     m_IconHwnd;
	BOOL    m_IsTraceCursor = FALSE;           //跟踪光标轨迹
	BOOL 	m_IsControl = FALSE;               //远程控制
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTE_CONTROL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void OnReceiveComplete(void);
	VOID DrawFirstData(void);
	VOID DrawNextData(void);
	VOID UpdateClipboardData(char* BufferData, ULONG BufferLength);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	BOOL SaveSnapshotData(void);
	VOID SendClipboardData(void);
	VOID OnSending(MSG* Msg);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};



enum
{
	ID_CONTROL = 0x1010,
	ID_SEND_CTRL_ALT_DEL,
	ID_TRACE_CURSOR,
	ID_BLOCK_INPUT,	// 锁定远程计算机输入
	ID_SAVE_DIB,		// 保存图片
	ID_GET_CLIPBOARD,	// 获取剪贴板
	ID_SET_CLIPBOARD,	// 设置剪贴板

};