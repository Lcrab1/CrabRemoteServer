// FileTransferModeDlg.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "FileTransferModeDlg.h"
#include "afxdialogex.h"


// CFileTransferModeDlg 对话框

IMPLEMENT_DYNAMIC(CFileTransferModeDlg, CDialogEx)

CFileTransferModeDlg::CFileTransferModeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILE_TRANSFER_MODE_DIALOG, pParent)
{

}

CFileTransferModeDlg::~CFileTransferModeDlg()
{
}

void CFileTransferModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CFileTransferModeDlg::OnEndDialog(UINT id)
{
	EndDialog(id);
}


BEGIN_MESSAGE_MAP(CFileTransferModeDlg, CDialogEx)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_COVER_BUTTON, IDC_CANCEL_BUTTON, OnEndDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CFileTransferModeDlg 消息处理程序


BOOL CFileTransferModeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString	Tips;
	Tips.Format("衰了 咋办.... \" %s \" ", m_FileFullPath);


	//
	for (int i = 0; i < Tips.GetLength(); i += 120)
	{
		Tips.Insert(i, "\n");
		i += 1;
	}

	//sdk
	SetDlgItemText(IDC_TIPS_STATIC, Tips);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CFileTransferModeDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
}
