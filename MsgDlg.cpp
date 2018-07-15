// MsgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "MsgDlg.h"
#include "resource.h"


// CMsgDlg �Ի���

IMPLEMENT_DYNAMIC(CMsgDlg, CDialog)
bool onCloseMsgWindowFlag;
CMsgDlg::CMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgDlg::IDD, pParent)
{

}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_msgList);
}


BEGIN_MESSAGE_MAP(CMsgDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CMsgDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMsgDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_MSG_CALLBACK, &CMsgDlg::OnRecieveMsg)
	ON_BN_CLICKED(IDC_BUTTON_MSG_CLEAR, &CMsgDlg::OnBnClickedButtonMsgClear)
//	ON_BN_CLICKED(IDC_BUTTON1, &CMsgDlg::OnBnClickedCloseMsgDlg)
END_MESSAGE_MAP()


// CMsgDlg ��Ϣ�������

void CMsgDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CMsgDlg::OnBnClickedCancel()
{
	//change close to open
	onCloseMsgWindowFlag = true;
	SetDlgItemText(IDC_BUTTON_MSG_CALLBACK, "Open Window");
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		OnCancel();
}

LRESULT CMsgDlg::OnRecieveMsg( WPARAM wparam, LPARAM lparam )
{

	int n = m_msgList.GetItemCount();
	m_msgList.InsertItem(n, (LPCTSTR)lparam);
	
	return 0;
}

void CMsgDlg::OnBnClickedButtonMsgClear()
{
	m_msgList.DeleteAllItems();
}

