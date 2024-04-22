// ProcessVMMapDlg.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "ProcessVMMapDlg.h"
#include "afxdialogex.h"


// CProcessVMMapDlg 对话框

IMPLEMENT_DYNAMIC(CProcessVMMapDlg, CDialogEx)

CProcessVMMapDlg::CProcessVMMapDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESS_VMMAP_DIALOG, pParent)
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
