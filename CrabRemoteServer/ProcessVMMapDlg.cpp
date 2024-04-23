// ProcessVMMapDlg.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "ProcessVMMapDlg.h"
#include "afxdialogex.h"

HANDLE CProcessVMMapDlg::m_ProcessID = INVALID_HANDLE_VALUE;
CString CProcessVMMapDlg::m_ProcessPath = NULL;
// CProcessVMMapDlg 对话框

typedef struct
{
	TCHAR* TitleData;          //列表的名称
	int		TitleWidth;            //列表的宽度
}COLUMN_DATA;

COLUMN_DATA __Column_Data_Online[] =
{
	{ _T("ID"),			50 },
	{ _T("地址"),	70 },
	{ _T("大小"),		70 },
	{ _T("状态"),			70 },
	{ _T("类型"),	70 },
	{ _T("初始保护"),		70 },
	{ _T("访问保护"),			70 }

};

IMPLEMENT_DYNAMIC(CProcessVMMapDlg, CDialogEx)

CProcessVMMapDlg::CProcessVMMapDlg(CWnd* pParent, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	: CDialogEx(IDD_PROCESS_VMMAP_DIALOG, pParent),m_IocpServer(IocpServer),m_ContextObject(ContextObject)
{
	
}

CProcessVMMapDlg::~CProcessVMMapDlg()
{
}

void CProcessVMMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMORY_PAGE_SIZE_EDIT, m_MemoryPageSizeEdit);
	//  DDX_Control(pDX, IDC_TOTAL_PHYSICAL_MEMORY_EDIT, m_TotalPhysicalMemory);
	DDX_Control(pDX, IDC_ACCESS_PHYSICAL_MEMORY_EDIT, m_AccessPhysicalMemoryEdit);
	DDX_Control(pDX, IDC_TOTAL_PHYSICAL_MEMORY_EDIT, m_TotalPhysicalMemoryEdit);
	DDX_Control(pDX, IDC_MINIMAL_ADDRESS_EDIT, m_MinimalAddressEdit);
	DDX_Control(pDX, IDC_TOTAL_PAGE_FILE_EDIT, m_TotalPageFileEdit);
	DDX_Control(pDX, IDC_ACCESS_PAGE_FILE_EDIT, m_AccessPageFileEdit);
	DDX_Control(pDX, IDC_MAXIMUM_ADDRESS_EDIT, m_MaximumAddressEdit);
	DDX_Control(pDX, IDC_TOTAL_VIRTUAL_MEMORY_EDIT, m_TotalVirtualMemoryEdit);
	DDX_Control(pDX, IDC_ACCESS_VIRTUAL_MEMORY_EDIT, m_AccessVirtualMemory);
	DDX_Control(pDX, IDC_VMMAP_PROCESS_ID_EDIT, m_ProcessIDEdit);
	DDX_Control(pDX, IDC_VMMAP_PROCESS_PATH_EDIT, m_ProcessPathEdit);
	DDX_Control(pDX, IDC_VMMAP_MEMORY_LIST_DIALOG, m_MemoryList);
	DDX_Control(pDX, IDC_VMMAP_COMMIT_CHECK, m_CommitCheckBox);
	DDX_Control(pDX, IDC_VMMAP_RESERVE_CHECK, m_ReserveCheckBox);
	DDX_Control(pDX, IDC_VMMAP_FREE_CHECK, m_FreeCheckBox);
}


BEGIN_MESSAGE_MAP(CProcessVMMapDlg, CDialogEx)
	ON_BN_CLICKED(IDC_VMMAP_REFRESH_BUTTON, &CProcessVMMapDlg::OnBnClickedVmmapRefreshButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CProcessVMMapDlg 消息处理程序


BOOL CProcessVMMapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//将进程ID和进程完整路径放在dlg中
	//放入进程ID
	CString strValue;
	strValue.Format("%d", m_ProcessID);
	m_ProcessIDEdit.SetWindowTextA(strValue);

	//放入进程完整路径
	m_ProcessPathEdit.SetWindowTextA(m_ProcessPath);
	OnInitControlList();
	this->m_CommitCheckBox.SetCheck(BST_CHECKED);
	this->m_ReserveCheckBox.SetCheck(BST_CHECKED);
	this->m_FreeCheckBox.SetCheck(BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CProcessVMMapDlg::OnInitControlList()
{
	for (int i = 0; i <
		sizeof(__Column_Data_Online) / sizeof(COLUMN_DATA); i++)
	{
		m_MemoryList.InsertColumn(i, __Column_Data_Online[i].TitleData,
			LVCFMT_CENTER, __Column_Data_Online[i].TitleWidth);
	}
	m_MemoryList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
}

void CProcessVMMapDlg::VMShowAddressList()
{

	DWORD	Offset = 0;
	PBYTE BufferData = (PBYTE)(m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1));
	SYSTEM_INFO systemInfo; 
	MEMORYSTATUS memoryStatus;
	memcpy(&systemInfo, BufferData+Offset, sizeof(SYSTEM_INFO));
	Offset += sizeof(SYSTEM_INFO);
	memcpy(&memoryStatus, BufferData+ Offset, sizeof(MEMORYSTATUS));
	Offset += sizeof(MEMORYSTATUS);

	VMShowSystemInfo(systemInfo, memoryStatus);

	VMShowAddressInfo(BufferData,Offset);
	//遍历发送来的每一个字符别忘了他的数据结构啊 Id+进程名+0+完整名+0
}

void CProcessVMMapDlg::VMShowSystemInfo(SYSTEM_INFO& systemInfo, MEMORYSTATUS& memoryStatus)
{
	CString v1;
	//内存页大小
	v1.Format(_T("%p"), systemInfo.dwPageSize);
	this->SetDlgItemText(IDC_MEMORY_PAGE_SIZE_EDIT, v1);
	//最小地址
	v1.Format(_T("%p"), systemInfo.lpMinimumApplicationAddress);
	this->SetDlgItemText(IDC_MINIMAL_ADDRESS_EDIT, v1);
	//最大地址
	v1.Format(_T("%p"), systemInfo.lpMaximumApplicationAddress);
	this->SetDlgItemText(IDC_MAXIMUM_ADDRESS_EDIT, v1);
	//物理内存
	v1.Format(_T("%p"), memoryStatus.dwTotalPhys);
	this->SetDlgItemText(IDC_TOTAL_PHYSICAL_MEMORY_EDIT, v1);
	//可用物理内存
	v1.Format(_T("%p"), memoryStatus.dwAvailPhys);
	this->SetDlgItemText(IDC_ACCESS_PHYSICAL_MEMORY_EDIT, v1);
	//总页文件
	v1.Format(_T("%p"), memoryStatus.dwTotalPageFile);
	this->SetDlgItemText(IDC_TOTAL_PAGE_FILE_EDIT, v1);
	//可用页文件
	v1.Format(_T("%p"), memoryStatus.dwAvailPageFile);
	this->SetDlgItemText(IDC_ACCESS_PAGE_FILE_EDIT, v1);
	//总虚拟地址
	v1.Format(_T("%p"), memoryStatus.dwTotalVirtual);
	this->SetDlgItemText(IDC_TOTAL_VIRTUAL_MEMORY_EDIT, v1);
	//可用虚拟地址
	v1.Format(_T("%p"), memoryStatus.dwAvailVirtual);
	this->SetDlgItemText(IDC_ACCESS_VIRTUAL_MEMORY_EDIT, v1);
}

void CProcessVMMapDlg::VMShowAddressInfo(PBYTE BufferData, DWORD& Offset)
{
	PBYTE bufferData = BufferData;
	DWORD offset = Offset;
	CString v1;
	UINT index=0;
	for (int i = 0; offset < m_ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() - 1; i++, offset += sizeof(MEMORY_BASIC_INFORMATION))
	{
		int test = m_CommitCheckBox.GetCheck();
		MEMORY_BASIC_INFORMATION* mbi = (MEMORY_BASIC_INFORMATION*)(bufferData+offset);
		if ((mbi->State == MEM_COMMIT && this->m_CommitCheckBox.GetCheck() == BST_UNCHECKED) || \
			(mbi->State == MEM_RESERVE && this->m_ReserveCheckBox.GetCheck() == BST_UNCHECKED) || \
			(mbi->State == MEM_FREE && this->m_FreeCheckBox.GetCheck() == BST_UNCHECKED) || \
			(mbi->State == 0))
			continue;

		v1.Format(_T("%d"), index + 1);
		this->m_MemoryList.InsertItem(index, v1);

		// [1]起始地址
		v1.Format(_T("%p"), mbi->BaseAddress);
		this->m_MemoryList.SetItemText(index, 1, v1);

		// [2]大小
		v1.Format(_T("%p"), mbi->RegionSize);
		this->m_MemoryList.SetItemText(index, 2, v1);

		// [3]状态
		switch (mbi->State)
		{
		case MEM_COMMIT:	v1 = _T("提交");	break;
		case MEM_RESERVE:	v1 = _T("保留");	break;
		case MEM_FREE:		v1 = _T("空闲");	break;
		}
		this->m_MemoryList.SetItemText(index, 3, v1);

		// [4]类型
		switch (mbi->Type)
		{
		case MEM_IMAGE:		v1 = _T("映像");	break;
		case MEM_PRIVATE:	v1 = _T("私有");	break;
		case MEM_MAPPED:	v1 = _T("映射");	break;
		}
		this->m_MemoryList.SetItemText(index, 4, v1);

		// [5]初始保护
		switch (mbi->AllocationProtect)
		{
		case PAGE_READONLY:				v1 = _T("-R--");	break;
		case PAGE_READWRITE:			v1 = _T("-RW-");	break;
		case PAGE_WRITECOPY:			v1 = _T("-RWC");	break;
		case PAGE_EXECUTE:				v1 = _T("E---");	break;
		case PAGE_EXECUTE_READ:			v1 = _T("ER--");	break;
		case PAGE_EXECUTE_READWRITE:	v1 = _T("ERW-");	break;
		case PAGE_EXECUTE_WRITECOPY:	v1 = _T("ERWC");	break;
		case PAGE_NOACCESS:				v1 = _T("----");	break;
		default:						v1 = _T("----");	break;
		}
		this->m_MemoryList.SetItemText(index, 5, v1);


		// [6]访问保护
		switch (mbi->Protect)
		{
		case PAGE_READONLY:				v1 = _T("-R--");	break;
		case PAGE_READWRITE:			v1 = _T("-RW-");	break;
		case PAGE_WRITECOPY:			v1 = _T("-RWC");	break;
		case PAGE_EXECUTE:				v1 = _T("E---");	break;
		case PAGE_EXECUTE_READ:			v1 = _T("ER--");	break;
		case PAGE_EXECUTE_READWRITE:	v1 = _T("ERW-");	break;
		case PAGE_EXECUTE_WRITECOPY:	v1 = _T("ERWC");	break;
		case PAGE_NOACCESS:				v1 = _T("----");	break;
		default:						v1 = _T("----");	break;
		}
		this->m_MemoryList.SetItemText(index, 6, v1);

		index++;

	}

}

void CProcessVMMapDlg::UpdateSystemInfo()
{
	BYTE IsToken = CLIENT_VMMAP_SYSTEM_INFO_UPDATE_REQUIRE;
	HANDLE ProcessID = m_ProcessID;
	int bufferLength = sizeof(HANDLE) + sizeof(BYTE);
	LPBYTE bufferData = new BYTE[bufferLength];
	bufferData[0] = IsToken;
	memcpy(bufferData + sizeof(BYTE), &ProcessID, sizeof(HANDLE));
	m_IocpServer->OnPrepareSending(m_ContextObject, &IsToken, 1);
}

void CProcessVMMapDlg::OnBnClickedVmmapRefreshButton()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MemoryList.DeleteAllItems();
	VMShowAddressList();
}


void CProcessVMMapDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
}
