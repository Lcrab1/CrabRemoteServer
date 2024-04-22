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
}


BEGIN_MESSAGE_MAP(CProcessVMMapDlg, CDialogEx)
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