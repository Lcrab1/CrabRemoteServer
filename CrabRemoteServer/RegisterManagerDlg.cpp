// RegisterManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "RegisterManagerDlg.h"
#include "afxdialogex.h"


// CRegisterManagerDlg 对话框
enum MY_KEY {
	MHKEY_CLASSES_ROOT,
	MHKEY_CURRENT_USER,
	MHKEY_LOCAL_MACHINE,
	MHKEY_USERS,
	MHKEY_CURRENT_CONFIG
};
enum KEY_VALUE {
	MREG_SZ,
	MREG_DWORD,
	MREG_BINARY,
	MREG_EXPAND_SZ
};
IMPLEMENT_DYNAMIC(CRegisterManagerDlg, CDialogEx)

CRegisterManagerDlg::CRegisterManagerDlg(CWnd* pParent, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	: CDialogEx(IDD_REGISTER_MANAGER_DIALOG, pParent)
{

	m_IocpServer = IocpServer;
	m_ContextObject = ContextObject;
}

CRegisterManagerDlg::~CRegisterManagerDlg()
{
}

void CRegisterManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REGISTER_SHOW_TREE, m_RegisterShowTree);
	DDX_Control(pDX, IDC_REGISTER_SHOW_LIST, m_RegisterShowList);
}


BEGIN_MESSAGE_MAP(CRegisterManagerDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_REGISTER_SHOW_TREE, &CRegisterManagerDlg::OnTvnSelchangedRegisterShowTree)
END_MESSAGE_MAP()


// CRegisterManagerDlg 消息处理程序


BOOL CRegisterManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->clientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
	v1.Format(_T("\\\\%s - 远程注册表管理"), IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//设置对话框标题
	SetWindowText(v1);


	// TODO:  在此添加额外的初始化
	 //制作树控件上的图标
	m_ImageList1.Create(18, 18, ILC_COLOR16, 10, 0);
	m_IconHwnd = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FATHER_ICON), IMAGE_ICON, 18, 18, 0);
	m_ImageList1.Add(m_IconHwnd);
	m_IconHwnd = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FILE_ICON), IMAGE_ICON, 18, 18, 0);
	m_ImageList1.Add(m_IconHwnd);


	m_RegisterShowTree.SetImageList(&m_ImageList1, TVSIL_NORMAL);


	m_Root = m_RegisterShowTree.InsertItem(_T("注册表管理"), 0, 0, 0, 0);      //0
	m_CurrentUser = m_RegisterShowTree.InsertItem(_T("HKEY_CURRENT_USER"), 1, 1, m_Root, 0); //1
	m_LocalMachine = m_RegisterShowTree.InsertItem(_T("HKEY_LOCAL_MACHINE"), 1, 1, m_Root, 0);
	m_Users = m_RegisterShowTree.InsertItem(_T("HKEY_USERS"), 1, 1, m_Root, 0);
	m_CurrentConfig = m_RegisterShowTree.InsertItem(_T("HKEY_CURRENT_CONFIG"), 1, 1, m_Root, 0);
	m_ClassRoot = m_RegisterShowTree.InsertItem(_T("HKEY_CLASSES_ROOT"), 1, 1, m_Root, 0);

	m_RegisterShowTree.Expand(m_Root, TVE_EXPAND);


	m_RegisterShowList.InsertColumn(0, _T("名称"), LVCFMT_LEFT, 150, -1);
	m_RegisterShowList.InsertColumn(1, _T("类型"), LVCFMT_LEFT, 60, -1);
	m_RegisterShowList.InsertColumn(2, _T("数据"), LVCFMT_LEFT, 300, -1);
	m_RegisterShowList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	//////添加图标//////
	m_ImageList2.Create(16, 16, TRUE, 2, 2);
	m_ImageList2.Add(AfxGetApp()->LoadIcon(IDI_STRING_ICON));
	m_ImageList2.Add(AfxGetApp()->LoadIcon(IDI_DWORD_ICON));
	m_RegisterShowList.SetImageList(&m_ImageList2, LVSIL_SMALL);

	m_IsEnable = TRUE;   //该值是为了解决频繁 向被控端请求  
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CRegisterManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_ContextObject != NULL)
	{
		m_ContextObject->RegisterDlg = NULL;
		m_ContextObject->DlgIdentity = 0;
		CancelIo((HANDLE)m_ContextObject->clientSocket);
		closesocket(m_ContextObject->clientSocket);
	}
	CDialogEx::OnClose();
}



void CRegisterManagerDlg::OnTvnSelchangedRegisterShowTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (!m_IsEnable)
	{
		return;
	}
	m_IsEnable = FALSE;;
	//获得你点击的Item
	TVITEM Item = pNMTreeView->itemNew;
	if (Item.hItem == m_Root)
	{
		m_IsEnable = TRUE;
		return;
	}
	m_Selected = Item.hItem;			//保存用户打开的子树节点句柄   
	m_RegisterShowList.DeleteAllItems();
	CString BufferData = GetFullPath(m_Selected);    //获得键值路径  
	char IsToken = GetFatherPath(BufferData);       //[2] \1\2\3

	//这个while循环是解决反复点击一个选项
	while (m_RegisterShowTree.GetChildItem(Item.hItem) != NULL)
	{
		m_RegisterShowTree.
			DeleteItem(m_RegisterShowTree.GetChildItem(Item.hItem));
	}
	//预加一个键
	int v1 = m_RegisterShowList.InsertItem(0, _T("(默认)"), 0);
	m_RegisterShowList.SetItemText(v1, 1, _T("REG_SZ"));
	m_RegisterShowList.SetItemText(v1, 2, _T("(数据未设置值)"));
	BufferData.Insert(0, IsToken);//插入  那个根键
	IsToken = CLIENT_REGISTER_MANAGER_DATA_CONTINUE;
	BufferData.Insert(0, IsToken);      //插入查询命令  [COMMAND_REG_FIND][1]
	m_IocpServer->OnPrepareSending(m_ContextObject, (LPBYTE)(BufferData.GetBuffer(0)),
		BufferData.GetLength() + 1);
	m_IsEnable = TRUE;
	*pResult = 0;
}

void CRegisterManagerDlg::OnReceiveComplete(void)
{
	switch (m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(0)[0])
	{
	case CLIENT_REGISTER_MANAGER_PATH_DATA_REPLY:
	{

		AddPath((char*)(m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1)));
		break;
	}

	case CLIENT_REGISTER_MANAGER_KEY_DATA_REPLY:
	{

		AddKey((char*)(m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1)));
		break;
	}
	}
	return;
}

CString CRegisterManagerDlg::GetFullPath(HTREEITEM Selected)
{
	CString v1;
	CString v2 = "";
	while (1)
	{
		if (Selected == m_Root)
		{
			return v2;
		}
		v1 = m_RegisterShowTree.GetItemText(Selected);
		if (v1.Right(1) != "\\")
			v1 += "\\";
		v2 = v1 + v2;
		Selected = m_RegisterShowTree.GetParentItem(Selected);   //得到父的
	}
	return v2;
}

char CRegisterManagerDlg::GetFatherPath(CString& FullPath)
{
	char IsToken;
	if (!FullPath.Find("HKEY_CLASSES_ROOT"))	//判断主键
	{
		IsToken = MHKEY_CLASSES_ROOT;
		FullPath.Delete(0, sizeof("HKEY_CLASSES_ROOT"));
	}
	else if (!FullPath.Find("HKEY_CURRENT_USER"))
	{
		IsToken = MHKEY_CURRENT_USER;
		FullPath.Delete(0, sizeof("HKEY_CURRENT_USER"));
	}
	else if (!FullPath.Find("HKEY_LOCAL_MACHINE"))
	{
		IsToken = MHKEY_LOCAL_MACHINE;
		FullPath.Delete(0, sizeof("HKEY_LOCAL_MACHINE"));
	}
	else if (!FullPath.Find("HKEY_USERS"))
	{
		IsToken = MHKEY_USERS;
		FullPath.Delete(0, sizeof("HKEY_USERS"));
	}
	else if (!FullPath.Find("HKEY_CURRENT_CONFIG"))
	{
		IsToken = MHKEY_CURRENT_CONFIG;
		FullPath.Delete(0, sizeof("HKEY_CURRENT_CONFIG"));
	}
	return IsToken;
}

void CRegisterManagerDlg::AddPath(char* BufferData)
{
	if (BufferData == NULL) return;
	int PacketHeaderLength = sizeof(PACKET_HEADER);
	PACKET_HEADER PacketHeader;
	memcpy((void*)&PacketHeader, BufferData, PacketHeaderLength);
	DWORD v1 = PacketHeader.nameCount;
	int v2 = PacketHeader.nameSize;

	if (v2 > 0 && v1 > 0) {                   //一点保护措施
		for (int i = 0; i < v1; i++) {
			char* PathName = BufferData + v2 * i + PacketHeaderLength;
			m_RegisterShowTree.InsertItem(PathName, 1, 1, m_Selected, 0);//插入子键名称
			m_RegisterShowTree.Expand(m_Selected, TVE_EXPAND);
		}
	}
}

void CRegisterManagerDlg::AddKey(char* BufferData)
{
	m_RegisterShowList.DeleteAllItems();
	int Item = m_RegisterShowList.InsertItem(0, _T("(Data)"), 0);
	m_RegisterShowList.SetItemText(Item, 1, _T("REG_SZ"));
	m_RegisterShowList.SetItemText(Item, 2, _T("(NULL)"));

	if (BufferData == NULL) return;

	PACKET_HEADER PacketHeader;
	memcpy((void*)&PacketHeader, BufferData, sizeof(PACKET_HEADER));
	char* Offset = BufferData + sizeof(PACKET_HEADER);
	for (int i = 0; i < PacketHeader.nameCount; i++)
	{
		BYTE Type = Offset[0];   //类型
		Offset += sizeof(BYTE);
		char* ValueName = Offset;   //取出名字
		Offset += PacketHeader.nameSize;
		BYTE* ValueDate = (BYTE*)Offset;      //取出值
		Offset += PacketHeader.nameValue;
		if (Type == MREG_SZ)
		{
			int Item = m_RegisterShowList.InsertItem(0, ValueName, 0);
			m_RegisterShowList.SetItemText(Item, 1, _T("REG_SZ"));
			m_RegisterShowList.SetItemText(Item, 2, (char*)ValueDate);
		}
		if (Type == MREG_DWORD)
		{

			char v1[256];
			DWORD v2 = 0;
			memcpy((void*)&v2, ValueDate, sizeof(DWORD));
			CString v3;
			v3.Format("0x%x", v2);

			int Item = m_RegisterShowList.InsertItem(0, ValueName, 1);
			m_RegisterShowList.SetItemText(Item, 1, _T("REG_DWORD"));
			m_RegisterShowList.SetItemText(Item, 2, v3);

		}
		if (Type == MREG_BINARY)
		{
			char v1[256];
			sprintf(v1, "%d", ValueDate);

			int Item = m_RegisterShowList.InsertItem(0, ValueName, 1);
			m_RegisterShowList.SetItemText(Item, 1, _T("REG_BINARY"));
			m_RegisterShowList.SetItemText(Item, 2, v1);
		}
		if (Type == MREG_EXPAND_SZ)
		{
			int Item = m_RegisterShowList.InsertItem(0, ValueName, 0);
			m_RegisterShowList.SetItemText(Item, 1, _T("REG_EXPAND_SZ"));
			m_RegisterShowList.SetItemText(Item, 2, (char*)ValueDate);
		}
	}
}
