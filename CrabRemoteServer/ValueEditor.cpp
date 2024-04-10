// ValueEditor.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "ValueEditor.h"
#include "afxdialogex.h"


// CValueEditor 对话框

IMPLEMENT_DYNAMIC(CValueEditor, CDialogEx)

CValueEditor::CValueEditor(CWnd* pParent, CString* Address, CIocpServer*
	IocpServer, CONTEXT_OBJECT* ContextObject)
	: CDialogEx(IDD_CHANGE_VALUE_DIALOG, pParent)
{
	m_Parent = (CMemoryEditorDlg*)pParent;
	m_StrAdddress = Address;
	m_IocpServer = IocpServer;
	m_ContextObject = ContextObject;
}

CValueEditor::~CValueEditor()
{
}

void CValueEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_CURRENT_ADDRESS_EDIT, m_CurrentEdit);
	//  DDX_Control(pDX, IDC_NEW_VALUE_EDIT, m_NewValueEdit);
	DDX_Control(pDX, IDC_CURRENT_ADDRESS_EDIT, m_CurrentAddressEdit);
	DDX_Control(pDX, IDC_NEW_VALUE_EDIT, m_NewValueEdit);
}


BEGIN_MESSAGE_MAP(CValueEditor, CDialogEx)
	ON_BN_CLICKED(IDC_NEW_VALUE_BUTTON, &CValueEditor::OnBnClickedNewValueButton)
END_MESSAGE_MAP()


// CValueEditor 消息处理程序


int CValueEditor::GetInputValue()
{
	CString strValue;
	m_NewValueEdit.GetWindowText(strValue);
	int dataValue = _ttoi(strValue);
	return dataValue;
}

BOOL CValueEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_CurrentAddressEdit.SetWindowTextA(*m_StrAdddress);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CValueEditor::OnBnClickedNewValueButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Judege;
	m_CurrentAddressEdit.GetWindowText(Judege);
	if (Judege == "") {
		return;
	}

	int bufferLength = sizeof(BYTE) + sizeof(int)+sizeof(size_t);
	LPBYTE bufferData = new BYTE[bufferLength];
	bufferData[0] = CLIENT_MEMORY_EDITOR_CHANGE_VALUE_REQUIRE;

	CString strAddress;
	m_CurrentAddressEdit.GetWindowText(strAddress);	//得到原地址
	size_t targetAddress = _tcstoul(strAddress, NULL, 0);	//将字符串转换为数字

	//得到要修改的数字
	int inputValue = GetInputValue();

	memcpy(bufferData + sizeof(BYTE) , &targetAddress, sizeof(size_t));
	memcpy(bufferData + sizeof(BYTE) + sizeof(size_t), &inputValue, sizeof(int));

	
	m_IocpServer->OnPrepareSending(m_ContextObject, bufferData, bufferLength);

}
