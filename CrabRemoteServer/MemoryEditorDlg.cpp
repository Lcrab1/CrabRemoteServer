// MemoryEditorDlg.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "MemoryEditorDlg.h"
#include "afxdialogex.h"
#include"ValueEditor.h"
/*****定义我的列表数据结构体*****/
typedef struct
{
	char* TitleData;             //列表的名称
	int		TitleWidth;            //列表的宽度
}COLUMN_DATA;

/*****Client表头*****/
COLUMN_DATA __AddressInfoList[] =
{
	{ "Address",	    240 },
	{ "Value",			200 },
};
// CMemoryEditorDlg 对话框

IMPLEMENT_DYNAMIC(CMemoryEditorDlg, CDialogEx)

CMemoryEditorDlg::CMemoryEditorDlg(CWnd* pParent, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	: CDialogEx(IDD_MEMORY_EDITOR_DIALOG, pParent)
{
	m_IocpServer = IocpServer;
	m_ContextObject = ContextObject;
	m_ProcessIDValue = INVALID_HANDLE_VALUE;
	m_CurrentValue = 0;
}

CMemoryEditorDlg::~CMemoryEditorDlg()
{
}

void CMemoryEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCESS_ID_IN_EDITOR_EDIT, m_ProcessID);
	DDX_Control(pDX, IDC_VALUE_CHANGE_EDIT, m_InputValue);
	DDX_Control(pDX, IDC_MEMORY_EDITOR_LIST, m_AddressList);
}


BEGIN_MESSAGE_MAP(CMemoryEditorDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FIRST_SCAN_BUTTON, &CMemoryEditorDlg::OnBnClickedFirstScanButton)
	ON_BN_CLICKED(IDC_SECOND_SCAN_BUTTON, &CMemoryEditorDlg::OnBnClickedSecondScanButton)
	ON_BN_CLICKED(IDC_UNDO_SCAN_BUTTON, &CMemoryEditorDlg::OnBnClickedUndoScanButton)
	ON_NOTIFY(NM_DBLCLK, IDC_MEMORY_EDITOR_LIST, &CMemoryEditorDlg::OnDblclkMemoryEditorList)
//	ON_MESSAGE(UM_OPEN_VALUE_EDITOR_DIALOG, &CMemoryEditorDlg::OnOpenValueEditorDialog)

END_MESSAGE_MAP()


// CMemoryEditorDlg 消息处理程序


void CMemoryEditorDlg::OnBnClickedFirstScanButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int bufferLength = sizeof(BYTE)+sizeof(int);
	LPBYTE bufferData = new BYTE[bufferLength];
	bufferData[0] = CLIENT_MEMORY_EDITOR_FIRST_SCAN_REQUIRE;
	int inputValue = GetInputValue();
	memcpy(bufferData + sizeof(BYTE), &inputValue, sizeof(int));
	m_IocpServer->OnPrepareSending(m_ContextObject, bufferData, bufferLength);
}


void CMemoryEditorDlg::OnBnClickedSecondScanButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int bufferLength = sizeof(BYTE) + sizeof(int);
	LPBYTE bufferData = new BYTE[bufferLength];
	bufferData[0] = CLIENT_MEMORY_EDITOR_NEXT_SCAN_REQUIRE;
	int inputValue = GetInputValue();
	memcpy(bufferData + sizeof(BYTE), &inputValue, sizeof(int));
	m_IocpServer->OnPrepareSending(m_ContextObject, bufferData, bufferLength);
}


void CMemoryEditorDlg::OnBnClickedUndoScanButton()
{
	// TODO: 在此添加控件通知处理程序代码
	UndoScan();
}


BOOL CMemoryEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	OnInitAddressList();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

int CMemoryEditorDlg::GetInputValue()
{
	CString strValue;
	m_InputValue.GetWindowText(strValue);
	int dataValue = _ttoi(strValue);
	m_CurrentValue = dataValue;
	return dataValue;
}

void CMemoryEditorDlg::ShowAddressInfoList()
{
	BYTE* BufferData = (BYTE*)(m_ContextObject->m_ReceivedBufferDataDecompressed.GetArray(1));
	
	DWORD	Offset = 0;
	size_t v1;
	m_AddressList.DeleteAllItems();
	//遍历发送来的每一个字符别忘了他的数据结构啊 Id+进程名+0+完整名+0
	int i;
	for (i = 0; Offset < m_ContextObject->m_ReceivedBufferDataDecompressed.GetArrayLength() - 1; i++)
	{
		size_t Address;
		memcpy(&Address, BufferData + Offset,sizeof(size_t));

		CString addressText;
		addressText.Format("%#x", Address);
		m_AddressList.InsertItem(i, addressText);       //将得到的数据加入到列表当中
		CString valueText;
		valueText.Format("%d", m_CurrentValue);
		m_AddressList.SetItemText(i, 1, valueText);

		//跳过这个数据结构进入下一个循环
		Offset += sizeof(DWORD);

	}

}

void CMemoryEditorDlg::OnInitAddressList()
{
	for (int i = 0; i < sizeof(__AddressInfoList) / sizeof(COLUMN_DATA); i++)
	{
		//设置ClientInfoList的标题栏
		m_AddressList.InsertColumn(i, __AddressInfoList[i].TitleData,
			LVCFMT_CENTER, __AddressInfoList[i].TitleWidth);
		//LVCFMT_CENTER是文字居中
	}
	m_AddressList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}

void CMemoryEditorDlg::UndoScan()
{
	BYTE Token = CLIENT_MEMORY_EDITOR_UNDO_SCAN_REQUIRE;
	m_IocpServer->OnPrepareSending(m_ContextObject, &Token, 1);
	m_AddressList.DeleteAllItems();
}


void CMemoryEditorDlg::OnDblclkMemoryEditorList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//PostMessage(UM_OPEN_VALUE_EDITOR_DIALOG, (WPARAM)0, (LPARAM)0);
	CListCtrl* listCtrl = &m_AddressList;
	POSITION position = listCtrl->GetFirstSelectedItemPosition();
	int	item = listCtrl->GetNextSelectedItem(position);
	CString itemText = listCtrl->GetItemText(item, 0);  // 获取第一列的文本

	CValueEditor* Dialog = new CValueEditor(this, &itemText, m_IocpServer, m_ContextObject);
	// 设置父窗口为卓面
	Dialog->Create(IDD_CHANGE_VALUE_DIALOG, GetDesktopWindow());    //创建非阻塞的Dlg
	Dialog->ShowWindow(SW_SHOW);

}
