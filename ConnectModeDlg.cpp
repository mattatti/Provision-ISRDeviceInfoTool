// ConnectModeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "ConnectModeDlg.h"


// CConnectModeDlg 对话框

IMPLEMENT_DYNAMIC(CConnectModeDlg, CDialog)

CConnectModeDlg::CConnectModeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectModeDlg::IDD, pParent)
	, m_registerPort(6000)
	, m_connectMode(0)
{

}

CConnectModeDlg::~CConnectModeDlg()
{
}

void CConnectModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PORT, m_registerPort);
	DDV_MinMaxUInt(pDX, m_registerPort, 1, 65535);
	DDX_Radio(pDX, IDC_RADIO_LOGIN, m_connectMode);
}


BEGIN_MESSAGE_MAP(CConnectModeDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_LOGIN, &CConnectModeDlg::OnBnClickedRadioLogin)
	ON_BN_CLICKED(IDC_RADIO_AUTO, &CConnectModeDlg::OnBnClickedRadioAuto)
END_MESSAGE_MAP()


// CConnectModeDlg 消息处理程序

BOOL CConnectModeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetDlgItem(IDC_STC_PORT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PORT)->ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CConnectModeDlg::OnBnClickedRadioLogin()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STC_PORT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PORT)->ShowWindow(SW_HIDE);
}

void CConnectModeDlg::OnBnClickedRadioAuto()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STC_PORT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_PORT)->ShowWindow(SW_SHOW);
}
