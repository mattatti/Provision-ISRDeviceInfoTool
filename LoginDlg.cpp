// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "LoginDlg.h"
#include <WinSock2.h>
#include "resource.h"

// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
	, m_username(_T(""))
	, m_password(_T(""))
	, m_port(0)
	, m_eConnectType(NET_SDK_CONNECT_TCP)
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_username);
	DDV_MaxChars(pDX, m_username, 65);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_password);
	DDV_MaxChars(pDX, m_password, 65);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_IP, &CLoginDlg::OnBnClickedRadioIp)
	ON_BN_CLICKED(IDC_RADIO_DOMAIN, &CLoginDlg::OnBnClickedRadioDomain)
	ON_BN_CLICKED(IDC_RADIO_NAT, &CLoginDlg::OnBnClickedRadioNAT)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序

void CLoginDlg::OnBnClickedOk()
{
	IsDlgButtonChecked(IDC_RADIO_IP) ?
		GetDlgItemText(IDC_IPADDRESS, m_ip) : GetDlgItemText(IDC_EDIT_DOMAIN, m_ip);

	m_port = GetDlgItemInt(IDC_EDIT_PORT);
	if (IsDlgButtonChecked(IDC_RADIO_NAT))
	{
		GetDlgItemText(IDC_EDIT_DOMAIN, m_ip);
		GetDlgItemText(IDC_EDIT_DEVICE_SN, m_strDeviceSN);
		if (m_strDeviceSN.IsEmpty())
		{
			AfxMessageBox(_T("Device SN error!"));
			return;
		}
	}

	OnOK();
}



BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_IPADDRESS, m_ip);
	SetDlgItemInt(IDC_EDIT_PORT, m_port, FALSE);

	m_username = _T("matan");
	m_password = _T("atias");

	SetDlgItemText(IDC_EDIT_DOMAIN, _T("provisionisr-nat.com"));
	SetDlgItemText(IDC_EDIT_DEVICE_SN, _T(""));
	GetDlgItem(IDC_EDIT_DEVICE_SN)->ShowWindow(SW_HIDE);
	SetDlgItemText(IDC_EDIT_PORT, _T("6004"));
	GetDlgItem(IDC_STATIC_DevSN)->ShowWindow(SW_HIDE);
	UpdateData(FALSE);

	CheckDlgButton(IDC_RADIO_IP, BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CLoginDlg::OnBnClickedRadioIp()
{
	
		
	GetDlgItem(IDC_STATIC_DevSN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_IPADDRESS)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_DOMAIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PORT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_DEVICE_SN)->ShowWindow(SW_HIDE);
	SetDlgItemText(IDC_EDIT_PORT, _T("6036"));
	m_eConnectType = NET_SDK_CONNECT_TCP;
	GetDlgItem(IDC_STATIC_POR)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_ADD)->ShowWindow(SW_SHOW);
}

void CLoginDlg::OnBnClickedRadioDomain()
{
	GetDlgItem(IDC_STATIC_DevSN)->ShowWindow(SW_HIDE);
	SetDlgItemText(IDC_EDIT_DOMAIN, _T(""));
	GetDlgItem(IDC_IPADDRESS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_DOMAIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_DEVICE_SN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PORT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_POR)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_ADD)->ShowWindow(SW_SHOW);

	SetDlgItemText(IDC_EDIT_PORT, _T("6036"));
	m_eConnectType = NET_SDK_CONNECT_TCP;
}

void CLoginDlg::OnBnClickedRadioNAT()
{
	GetDlgItem(IDC_STATIC_POR)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_DevSN)->ShowWindow(SW_SHOW);
	SetDlgItemText(IDC_EDIT_DOMAIN, _T("provisionisr-nat.com"));
	GetDlgItem(IDC_IPADDRESS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_DOMAIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_DEVICE_SN)->ShowWindow(TRUE);
	SetDlgItemText(IDC_EDIT_PORT, _T("8989"));
	GetDlgItem(IDC_EDIT_PORT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_ADD)->ShowWindow(SW_HIDE);

	m_eConnectType = NET_SDK_CONNECT_NAT;
}