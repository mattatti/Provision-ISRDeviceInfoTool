// BackupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "BackupDlg.h"
#include "strdef.h"


// CBackupDlg 对话框

IMPLEMENT_DYNAMIC(CBackupDlg, CDialog)

CBackupDlg::CBackupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBackupDlg::IDD, pParent)
{

}

CBackupDlg::~CBackupDlg()
{
}

void CBackupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
}


BEGIN_MESSAGE_MAP(CBackupDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CBackupDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBackupDlg::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBackupDlg 消息处理程序

void CBackupDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CBackupDlg::OnBnClickedCancel()
{
	NET_SDK_StopGetFile(m_fileHanle);
	OnCancel();
}

BOOL CBackupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	
	CString fname;
	SYSTEMTIME time = {0};
	::GetLocalTime(&time);
	if (m_backupPath.Right(1) != "\\")
		m_backupPath += "\\";
	fname.Format("%sbackup_%d%02d%02d%02d%02d%02d.avi", m_backupPath.GetBuffer(0), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	LONG handle = NET_SDK_GetFileByTime(m_userID, m_backupFiles[0].dwChannel, &m_backupFiles[0].startTime, 
		&m_backupFiles[0].stopTime, fname.GetBuffer());
	if (handle > 0)
	{
		m_fileHanle = handle;
		SetTimer(1010, 1000, NULL);
	}
	else
	{
		AfxMessageBox(STR_BACKUP_FAILED);
		EndDialog(IDCANCEL);
		return TRUE;
	}
	m_doneNum = 1;
	refreshTip();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CBackupDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1010)
	{
		int pos = NET_SDK_GetDownloadPos(m_fileHanle);
		if (pos >= 100)
		{
			if (m_doneNum == m_fileNum)
			{
				delete[] m_backupFiles;
				EndDialog(IDOK);
				KillTimer(1010);
				NET_SDK_StopGetFile(m_fileHanle);
				return;
			}
			NET_SDK_StopGetFile(m_fileHanle);
			CString fname;
			fname.Format(_T("%s\\backup_%d.avi"), m_backupPath, m_doneNum + 1);
			LONG handle = NET_SDK_GetFileByTime(m_userID, m_backupFiles[m_doneNum].dwChannel, &m_backupFiles[m_doneNum].startTime, 
				&m_backupFiles[m_doneNum].stopTime, fname.GetBuffer());
			if (handle > 0)
			{
				m_fileHanle = handle;
			}
			else
			{
				DWORD error = NET_SDK_GetLastError();
				AfxMessageBox(STR_BACKUP_FAILED);
				EndDialog(IDCANCEL);
				KillTimer(1010);
				return;
			}
			m_doneNum++;
			refreshTip();
		}
		else
		{
			m_progress.SetPos(pos);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CBackupDlg::refreshTip()
{
	CString temp;
	temp.Format(_T("%d/%d"), m_doneNum, m_fileNum);
	SetDlgItemText(IDC_STATIC_TIP, temp);
}
