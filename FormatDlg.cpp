// FormatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "FormatDlg.h"
#include "DVR_NET_SDK.h"
#include "strdef.h"

// CFormatDlg 对话框

IMPLEMENT_DYNAMIC(CFormatDlg, CDialog)

CFormatDlg::CFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFormatDlg::IDD, pParent)
	, m_formatHandle(-1)
{

}

CFormatDlg::~CFormatDlg()
{
}

void CFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_FORMAT, m_progressCtrl);
}


BEGIN_MESSAGE_MAP(CFormatDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFormatDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFormatDlg::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CFormatDlg 消息处理程序

void CFormatDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CFormatDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

BOOL CFormatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SetTimer(4040, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFormatDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 4040)
	{
		LONG currentDisk = -1;
		LONG percent = 0;
		LONG status = -1;
		if (NET_SDK_GetFormatProgress(m_formatHandle, &currentDisk, &percent, &status))
		{
			if (status == 2)
			{
				KillTimer(4040);
				AfxMessageBox(STR_FORMAT_FAILED);
				EndDialog(IDCANCEL);
			}
			else if(status == 1)
			{
				
				KillTimer(4040);
				AfxMessageBox(STR_FORMAT_FINISHED);
				EndDialog(IDOK);
			}
			else if (status == 0)
			{
				m_progressCtrl.SetPos(percent);
			}
		}
		else
		{
			KillTimer(4040);
			AfxMessageBox(STR_GET_PROCESS_FAILED);
			EndDialog(IDCANCEL);
		}
	}

	CDialog::OnTimer(nIDEvent);
}
