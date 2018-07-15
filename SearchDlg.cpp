// SearchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "SearchDlg.h"
#include "FolderDialog.h"
#include "BackupDlg.h"
#include "PlayBackDlg.h"
#include "strdef.h"

// CSearchDlg 对话框

IMPLEMENT_DYNAMIC(CSearchDlg, CDialog)

CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchDlg::IDD, pParent)
	, m_chnn(0)
	, m_searchDate(0)
	, m_backPath(_T("D:\\"))
{

}

CSearchDlg::~CSearchDlg()
{
	listPlaybackDlgIter it = m_listPlaybackDlg.begin();
	for (; it != m_listPlaybackDlg.end(); it++)
	{
		CPlayBackDlg *pDlg = reinterpret_cast<CPlayBackDlg*>(*it);
		if (pDlg)
		{
			delete pDlg;
			pDlg = NULL;
		}
	}
	m_listPlaybackDlg.clear();
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SEARCHTYPE, m_searchType);
	DDX_Control(pDX, IDC_LIST_RESULT, m_resultList);
	DDX_CBIndex(pDX, IDC_COMBO_CHNN, m_chnn);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_DATE, m_searchDate);
	DDX_Text(pDX, IDC_EDIT_BACKUP_PATH, m_backPath);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSearchDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSearchDlg::OnBnClickedCancel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SEARCHTYPE, &CSearchDlg::OnTcnSelchangeTabSearchtype)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CSearchDlg::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_LOCK, &CSearchDlg::OnBnClickedButtonLock)
	ON_BN_CLICKED(IDC_BUTTON_UNLOCK, &CSearchDlg::OnBnClickedButtonUnlock)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CSearchDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP, &CSearchDlg::OnBnClickedButtonBackup)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CSearchDlg::OnBnClickedButtonBrowser)
	ON_BN_CLICKED(IDC_RADIO_EVENTTYPE_MOTION, &CSearchDlg::OnBnClickedRadioEventtype)
	ON_BN_CLICKED(IDC_RADIO_EVENTTYPE_SENSOR, &CSearchDlg::OnBnClickedRadioEventtype)
	ON_BN_CLICKED(IDC_RADIO_EVENTTYPE_ALL, &CSearchDlg::OnBnClickedRadioEventtype)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RESULT, &CSearchDlg::OnNMDblclkListResult)
END_MESSAGE_MAP()


// CSearchDlg 消息处理程序

void CSearchDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CSearchDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

BOOL CSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_searchType.InsertItem(0, STR_FILE);
	m_searchType.InsertItem(1, STR_EVENT);
	m_searchType.InsertItem(2, STR_TIME);
	m_searchType.InsertItem(3, STR_DATE);
	
	LONG result;
	m_searchDate = CTime::GetCurrentTime();
	UpdateData(FALSE);
	OnTcnSelchangeTabSearchtype(NULL, &result);
	((CButton *)GetDlgItem(IDC_RADIO_EVENTTYPE_MOTION))->SetCheck(true);
	OnBnClickedRadioEventtype();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSearchDlg::SetDeviceInfo( NET_SDK_DEVICEINFO* di, LONG userId )
{
	m_deviceInfo = di;

	CString temp;
	CComboBox * combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_CHNN));
	for (int i = 0; i < m_deviceInfo->videoInputNum; i++)
	{
		temp.Format(STR_CHANNEL_D, i + 1);
		combo->AddString(temp);
	}
	combo->SetCurSel(0);

	if (userId >= 0)
	{
		m_userID = userId;
	}

}

void CSearchDlg::OnTcnSelchangeTabSearchtype(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_resultList.DeleteAllItems();
	m_fileList.RemoveAll();
	m_eventList.RemoveAll();
	int   colCount   =   m_resultList.GetHeaderCtrl()-> GetItemCount();
	for(int i=0;   i <colCount;   i++)
	{
		m_resultList.DeleteColumn(0);
	}

	int sel = m_searchType.GetCurSel();
	if (sel == 0)
	{
		m_resultList.SetExtendedStyle(m_resultList.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES
			| LVS_EX_FULLROWSELECT);
		m_resultList.InsertColumn(0, STR_SELECT);
		m_resultList.InsertColumn(1, STR_CHANNEL);
		m_resultList.InsertColumn(2, STR_START_TIME);
		m_resultList.InsertColumn(3, STR_STOP_TIME);
		m_resultList.InsertColumn(4, STR_STATUS);
		m_resultList.SetColumnWidth(0, 50);
		m_resultList.SetColumnWidth(1, 50);
		m_resultList.SetColumnWidth(2, 140);
		m_resultList.SetColumnWidth(3, 140);
		m_resultList.SetColumnWidth(4, 50);
	}
	else if (sel == 1)
	{
		m_resultList.SetExtendedStyle((m_resultList.GetExtendedStyle() & (~LVS_EX_CHECKBOXES)) | LVS_EX_GRIDLINES
			| LVS_EX_FULLROWSELECT );
		m_resultList.InsertColumn(0, STR_CHANNEL);
		m_resultList.InsertColumn(1, STR_START_TIME);
		m_resultList.InsertColumn(2, STR_STOP_TIME);
		m_resultList.InsertColumn(3, STR_TYPE);
		m_resultList.SetColumnWidth(0, 50);
		m_resultList.SetColumnWidth(1, 140);
		m_resultList.SetColumnWidth(2, 140);
		m_resultList.SetColumnWidth(3, 100);
	}
	else if (sel == 2)
	{
		m_resultList.SetExtendedStyle((m_resultList.GetExtendedStyle() & (~LVS_EX_CHECKBOXES)) | LVS_EX_GRIDLINES
			| LVS_EX_FULLROWSELECT );
		m_resultList.InsertColumn(0, STR_CHANNEL);
		m_resultList.InsertColumn(1, STR_START_TIME);
		m_resultList.InsertColumn(2, STR_STOP_TIME);
		m_resultList.SetColumnWidth(0, 100);
		m_resultList.SetColumnWidth(1, 160);
		m_resultList.SetColumnWidth(2, 160);
	}
	else if (sel == 3)
	{
		m_resultList.SetExtendedStyle((m_resultList.GetExtendedStyle() & (~LVS_EX_CHECKBOXES)) | LVS_EX_GRIDLINES
			| LVS_EX_FULLROWSELECT );
		m_resultList.InsertColumn(0, STR_YEAR);
		m_resultList.InsertColumn(1, STR_MONTH);
		m_resultList.InsertColumn(2, STR_DAY);
		m_resultList.SetColumnWidth(0, 100);
		m_resultList.SetColumnWidth(1, 160);
		m_resultList.SetColumnWidth(2, 160);
	}
	
	CRect rect;
	for (int i = IDC_BUTTON_LOCK, j = IDC_RADIO_EVENTTYPE_MOTION; i <= IDC_BUTTON_DELETE; i++, j++)
	{
		GetDlgItem(i)->ShowWindow(sel == 0);
		GetDlgItem(j)->ShowWindow(sel == 1);
	}

	GetDlgItem(IDC_GROUP_BACKUP)->ShowWindow(sel == 0);
	GetDlgItem(IDC_EDIT_BACKUP_PATH)->ShowWindow(sel == 0);
	GetDlgItem(IDC_BUTTON_BACKUP)->ShowWindow(sel == 0);
	GetDlgItem(IDC_STATIC_BACKUP)->ShowWindow(sel == 0);
	GetDlgItem(IDC_BUTTON_BROWSER)->ShowWindow(sel == 0);
	GetDlgItem(IDC_STATIC_HEHE)->ShowWindow(sel < 2);

	*pResult = 0;
}

void CSearchDlg::OnBnClickedButtonSearch()
{
	UpdateData();
#if 1
	DD_TIME start = {0, 0, 0, 0, m_searchDate.GetDay(), m_searchDate.GetMonth() - 1, m_searchDate.GetYear() - 1900};
	DD_TIME end = {59, 59, 23, 0, m_searchDate.GetDay(), m_searchDate.GetMonth() - 1, m_searchDate.GetYear() - 1900};
	
	int sel = m_searchType.GetCurSel();
	m_resultList.DeleteAllItems();
	m_fileList.RemoveAll();
	m_eventList.RemoveAll();
	m_timeList.RemoveAll();
	m_dateList.RemoveAll();


	if (sel == 0)
	{
		LONG searchHandle =  NET_SDK_FindFile(m_userID, m_chnn, &start, &end);
		LONG result = NET_SDK_FILE_SUCCESS;
		if (searchHandle > 0)
		{
			while (true)
			{
				NET_SDK_REC_FILE file;
				result = NET_SDK_FindNextFile(searchHandle, &file);
				if (result == NET_SDK_NOMOREFILE)
				{
					break;
				}
				m_fileList.AddTail(file);

				int n = m_resultList.GetItemCount();
				m_resultList.InsertItem(n, _T(""));
				CString temp;
				temp.Format(_T("%d"), file.dwChannel + 1);
				m_resultList.SetItemText(n , 1, temp);
				CTime s(file.startTime.year + 1900, file.startTime.month + 1, file.startTime.mday, file.startTime.hour, 
					file.startTime.minute, file.startTime.second);
				CTime e(file.stopTime.year + 1900, file.stopTime.month + 1, file.stopTime.mday, file.stopTime.hour, 
					file.stopTime.minute, file.stopTime.second);

				m_resultList.SetItemText(n, 2, s.Format(_T("%Y-%m-%d %H:%M:%S")));
				m_resultList.SetItemText(n, 3, e.Format(_T("%Y-%m-%d %H:%M:%S")));
				if (file.bFileLocked)
				{
					m_resultList.SetItemText(n, 4, STR_LOCKED);
				}
			}
			NET_SDK_FindClose(searchHandle);
		}
		else
		{
			DWORD erro = NET_SDK_GetLastError();
			AfxMessageBox(STR_FIND_NO_FILE);
		}
		
	}
	else if (sel == 1)
	{
		LONG searchHandle =  NET_SDK_FindEvent(m_userID, m_chnn, m_eventType, &start, &end);
		LONG result = NET_SDK_FILE_SUCCESS;
		if (searchHandle > 0)
		{
			while(true)
			{
				NET_SDK_REC_EVENT event;
				result = NET_SDK_FindNextEvent(searchHandle, &event);
				if (result == NET_SDK_NOMOREFILE)
				{
					break;
				}
				m_eventList.AddTail(event);

				int n = m_resultList.GetItemCount();
				m_resultList.InsertItem(n, _T(""));
				CString temp;
				temp.Format(_T("%d"), event.dwChannel + 1);
				m_resultList.SetItemText(n , 0, temp);
				CTime s(event.startTime.year + 1900, event.startTime.month + 1, event.startTime.mday, event.startTime.hour, 
					event.startTime.minute, event.startTime.second);
				CTime e(event.stopTime.year + 1900, event.stopTime.month + 1, event.stopTime.mday, event.stopTime.hour, 
					event.stopTime.minute, event.stopTime.second);

				m_resultList.SetItemText(n, 1, s.Format(_T("%Y-%m-%d %H:%M:%S")));
				m_resultList.SetItemText(n, 2, e.Format(_T("%Y-%m-%d %H:%M:%S")));
				switch (event.dwRecType)
				{
				case DD_RECORD_TYPE_MANUAL:
					temp = STR_MANUAL_RECORD;
					break;
				case DD_RECORD_TYPE_SCHEDULE:
					temp = STR_SCHEDULE_RECORD;
					break;
				case DD_RECORD_TYPE_MOTION:
					temp = STR_MOTION_RECORD;
					break;
				case DD_RECORD_TYPE_SENSOR:
					temp = STR_SENSOR_RECORD;
					break;
				default:break;
				}
				m_resultList.SetItemText(n, 3, temp);
			}
			NET_SDK_FindEventClose(searchHandle);
		}
		else
		{
			AfxMessageBox(STR_FIND_NO_FILE);
		}
		
	}
	else if (sel == 2)
	{
		LONG searchHandle =  NET_SDK_FindTime(m_userID, m_chnn, &start, &end);
		LONG result = NET_SDK_FILE_SUCCESS;
		if (searchHandle > 0)
		{
			while(true)
			{
				NET_SDK_REC_TIME time;
				ZeroMemory(&time, sizeof(NET_SDK_REC_TIME));
				result = NET_SDK_FindNextTime(searchHandle, &time);		
				if (result == NET_SDK_NOMOREFILE)
				{
					break;
				}
				if (result == NET_SDK_FILE_NOFIND)
				{
					AfxMessageBox(STR_FIND_NO_FILE);
					break;
				}

				m_timeList.AddTail(time);

				int n = m_resultList.GetItemCount();
				m_resultList.InsertItem(n, _T(""));
				CString temp;
				temp.Format(_T("%d"), time.dwChannel + 1);
				m_resultList.SetItemText(n , 0, temp);
				CTime s(time.startTime.year + 1900, time.startTime.month + 1, time.startTime.mday, time.startTime.hour, 
					time.startTime.minute, time.startTime.second);
				CTime e(time.stopTime.year + 1900, time.stopTime.month + 1, time.stopTime.mday, time.stopTime.hour, 
					time.stopTime.minute, time.stopTime.second);

				m_resultList.SetItemText(n, 1, s.Format(_T("%Y-%m-%d %H:%M:%S")));
				m_resultList.SetItemText(n, 2, e.Format(_T("%Y-%m-%d %H:%M:%S")));
			}
			NET_SDK_FindTimeClose(searchHandle);
		}
		else
		{
			AfxMessageBox(STR_FIND_NO_FILE);
		}
	}
	else if (sel == 3)
	{
		LONG handle = NET_SDK_FindRecDate(m_userID);
		if (handle > 0)
		{

			while(true)
			{
				DD_DATE date;
				LONG result = NET_SDK_FindNextRecDate(handle, &date);
				if (result == NET_SDK_NOMOREFILE)
				{
					break;
				}
				m_dateList.AddTail(date);
				int n = m_resultList.GetItemCount();
				m_resultList.InsertItem(n, _T(""));

				CString temp;
				temp.Format(_T("%d"), date.year);
				m_resultList.SetItemText(n, 0, temp);
				temp.Format(_T("%d"), date.month);
				m_resultList.SetItemText(n, 1, temp);
				temp.Format(_T("%d"), date.mday);
				m_resultList.SetItemText(n, 2, temp);
			}
			NET_SDK_FindRecDateClose(handle);
		}
		else
		{
			AfxMessageBox(STR_FIND_NO_FILE);
		}
		
	}
#endif
#if 0
	NET_SDK_IMAGE_SREACH sInSreachImage;
	DD_TIME start = {0, 0, 0, 0, m_searchDate.GetDay(), m_searchDate.GetMonth(), m_searchDate.GetYear()};
	DD_TIME end = {59, 59, 23, 0, m_searchDate.GetDay(), m_searchDate.GetMonth(), m_searchDate.GetYear()};
	memcpy(&sInSreachImage.StartTime, &start, sizeof(DD_TIME));
	memcpy(&sInSreachImage.StopTime, &end, sizeof(DD_TIME));
	
	sInSreachImage.dwChannel = m_chnn;
	sInSreachImage.pageSize = 10;
	sInSreachImage.pageIndex = 1;
	sInSreachImage.sort = IMAGE_SORT_DESC;
	int Index = 0;
	NET_SDK_IMAGE pOutImage[10];
	LONG pOutImageNum = 0;
	NET_SDK_SearchPictures(m_userID, sInSreachImage, 10*sizeof(NET_SDK_IMAGE), pOutImage, &pOutImageNum);
	if(pOutImageNum)
	{
		NET_SDK_IMAGE_INFO pOutImageInfo;
		unsigned int outBufferSize =  2*1024*1024;
		char *pOutBuffer = new char[outBufferSize];
		BOOL ret = NET_SDK_DownLoadPicture(m_userID, pOutImage[0], &pOutImageInfo, pOutBuffer, outBufferSize);	
		if(ret)
		{
			if((outBufferSize >= pOutImageInfo.imageSize)&&(pOutImageInfo.imageSize > 0))
			{
				FILE *pFile = fopen("./testrecv.jpg", "wb");
				fwrite(pOutBuffer, pOutImageInfo.imageSize, 1, pFile);
				fclose( pFile);
			}
		}
		delete(pOutBuffer);
	}
	
	int i=0;
	//while()
#endif

}

void CSearchDlg::OnBnClickedButtonLock()
{
	CArray<NET_SDK_REC_FILE> temp;
	POSITION pos = m_fileList.GetHeadPosition();
	for (int i = 0; i < m_resultList.GetItemCount(); i++)
	{
		NET_SDK_REC_FILE& file = m_fileList.GetNext(pos);
		if (m_resultList.GetCheck(i))
		{
// 			if (RECORD_FILE_IS_WRITING(file.dwRecType))
// 			{
// 				AfxMessageBox(_T("文件正在写，无法执行操作！"));
// 				return;
// 			}
			temp.Add(file);
			m_resultList.SetItemText(i, 4, STR_LOCKED);
			m_resultList.SetCheck(i, FALSE);
			file.bFileLocked = TRUE;
		}
	}

	if (temp.GetSize() == 0)
	{
		AfxMessageBox(STR_NO_SELECT_FILE);
		return;
	}
	
	NET_SDK_REC_FILE *fileToLock = new NET_SDK_REC_FILE[temp.GetSize()];
	
	for (int i = 0; i < temp.GetSize(); i++)
	{
		fileToLock[i] = temp[i];
	}
	
	NET_SDK_LockFile(m_userID, fileToLock, temp.GetSize());
	delete[] fileToLock;
}

void CSearchDlg::OnBnClickedButtonUnlock()
{
	CArray<NET_SDK_REC_FILE> temp;
	POSITION pos = m_fileList.GetHeadPosition();
	for (int i = 0; i < m_resultList.GetItemCount(); i++)
	{
		NET_SDK_REC_FILE& file = m_fileList.GetNext(pos);
		if (m_resultList.GetCheck(i))
		{
// 			if (RECORD_FILE_IS_WRITING(file.dwRecType))
// 			{
// 				AfxMessageBox(_T("文件正在写，无法执行操作！"));
// 				return;
// 			}
			temp.Add(file);
			m_resultList.SetItemText(i, 4, _T(""));
			m_resultList.SetCheck(i, FALSE);
			file.bFileLocked = FALSE;
		}
	}
	if (temp.GetSize() == 0)
	{
		AfxMessageBox(STR_NO_SELECT_FILE);
		return;
	}
	NET_SDK_REC_FILE *fileToUnlock = new NET_SDK_REC_FILE[temp.GetSize()];

	for (int i = 0; i < temp.GetSize(); i++)
	{
		fileToUnlock[i] = temp[i];
	}

	NET_SDK_UnlockFile(m_userID, fileToUnlock, temp.GetSize());
	delete[] fileToUnlock;
}

void CSearchDlg::OnBnClickedButtonDelete()
{
	CArray<NET_SDK_REC_FILE> temp;

	for (int i = 0; i < m_resultList.GetItemCount();)
	{
		if (m_resultList.GetCheck(i))
		{
			POSITION pos = m_fileList.FindIndex(i);
			NET_SDK_REC_FILE& file = m_fileList.GetAt(pos);
// 			if (RECORD_FILE_IS_WRITING(file.dwRecType))
// 			{
// 				AfxMessageBox(_T("文件正在写，无法执行操作！"));
// 				return;
// 			}
			temp.Add(file);
			m_fileList.RemoveAt(pos);
			m_resultList.DeleteItem(i);
			continue;
		}
		i++;
	}

	if (temp.GetSize() == 0)
	{
		AfxMessageBox(STR_NO_SELECT_FILE);
		return;
	}
	
	NET_SDK_REC_FILE *fileToDelete = new NET_SDK_REC_FILE[temp.GetSize()];

	for (int i = 0; i < temp.GetSize(); i++)
	{
		fileToDelete[i] = temp[i];
	}

	BOOL bResult = NET_SDK_DeleteRecFile(m_userID, fileToDelete, temp.GetSize());
	if (!bResult)
	{
		char szLog[128] = {0};
		sprintf_s(szLog, sizeof(szLog), "Delete faild! Error code=%d", NET_SDK_GetLastError());
		AfxMessageBox(szLog);
	}
	delete[] fileToDelete;
}

void CSearchDlg::OnBnClickedButtonBackup()
{
	UpdateData();
	if (m_backPath.IsEmpty())
	{
		AfxMessageBox(STR_PATH_EMPTY);
		return;
	}

	CArray<NET_SDK_REC_FILE> temp;
	POSITION pos = m_fileList.GetHeadPosition();
	for (int i = 0; i < m_resultList.GetItemCount(); i++)
	{
		NET_SDK_REC_FILE& file = m_fileList.GetNext(pos);
		if (m_resultList.GetCheck(i))
		{
// 			if (RECORD_FILE_IS_WRITING(file.dwRecType))
// 			{
// 				AfxMessageBox(_T("文件正在写，无法执行操作！"));
// 				return;
// 			}
			temp.Add(file);
			m_resultList.SetCheck(i, FALSE);
		}
	}

	if (temp.GetSize() == 0)
	{
		AfxMessageBox(STR_NO_SELECT_FILE);
		return;
	}
	
	NET_SDK_REC_FILE *fileToBackup = new NET_SDK_REC_FILE[temp.GetSize()];
	
	for (int i = 0; i < temp.GetSize(); i++)
	{
		fileToBackup[i] = temp[i];
	}
	
// 	CBackupDlg dlg;
// 	dlg.SetBackupInfo(m_userID, fileToBackup, temp.GetSize(), m_backPath);
// 	dlg.DoModal();	
	
	m_pBackDlg = new CBackupDlg;
	ASSERT(m_pBackDlg != NULL);
	m_pBackDlg->SetBackupInfo(m_userID, fileToBackup, temp.GetSize(), m_backPath);
	m_pBackDlg->Create(IDD_BACKUP_DIALOG, this);
	m_pBackDlg->ShowWindow(SW_SHOW);
	//NET_SDK_LockFile(m_userID, fileToLock, temp.GetSize());
	
	delete[] fileToBackup;
}


void CSearchDlg::OnBnClickedButtonBrowser()
{
	CFolderDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetFolderPath();
		SetDlgItemText(IDC_EDIT_BACKUP_PATH, path);
	}
}

void CSearchDlg::OnBnClickedRadioEventtype()
{
	for (int i = IDC_RADIO_EVENTTYPE_MOTION; i <= IDC_RADIO_EVENTTYPE_ALL; i++)
	{
		if (reinterpret_cast<CButton*>(GetDlgItem(i))->GetCheck())
		{
			int n = i - IDC_RADIO_EVENTTYPE_MOTION;
			if (n == 0)
			{
				m_eventType = DD_RECORD_TYPE_MOTION;
			}
			else if (n == 1)
			{
				m_eventType = DD_RECORD_TYPE_SENSOR;
			}
			else if (n == 2)
			{
				m_eventType = 0xffff;
			}
			break;
		}
	}
}

#define MULTI_PLAYBACK
void CSearchDlg::OnNMDblclkListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
#if 0
	//test
	DD_TIME start1, end1;
	start1.year = 114;
	start1.month = 5;
	start1.mday = 19;
	start1.wday = 0;
	start1.hour = 8;
	start1.minute = 7;
	start1.second = 7;

	end1.year = 114;
	end1.month = 5;
	end1.mday = 19;
	end1.wday = 0;
	end1.hour = 12;
	end1.minute = 44;
	end1.second = 3;

	int chnn1 = 0;

	CPlayBackDlg dlg1;
	dlg1.setPlayBackInfo(m_userID, start1, end1, chnn1);
	dlg1.DoModal();

	*pResult = 0;
	return;
#endif

	int index = m_resultList.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	int sel = m_searchType.GetCurSel();
	POSITION pos = NULL;
	if (sel == 0)
	{
		pos = m_fileList.GetHeadPosition();
	}
	else if(sel == 1)
	{
		pos = m_eventList.GetHeadPosition();
	}
	else if(sel == 2)
	{
		pos = m_timeList.GetHeadPosition();
	}
	
	while (index > 0)
	{
		if (sel == 0)
		{
			m_fileList.GetNext(pos);
		}
		else if(sel == 1)
		{
			m_eventList.GetNext(pos);
		}
		else if(sel == 2)
		{
			m_timeList.GetNext(pos);
		}
		index--;
	}
	
	DD_TIME start, end;
	int chnn;
	if (sel == 0)
	{
		NET_SDK_REC_FILE &file = m_fileList.GetNext(pos);
		start = file.startTime;
		end = file.stopTime;
		chnn = file.dwChannel;
	}
	else if(sel == 1)
	{
		NET_SDK_REC_EVENT &event = m_eventList.GetNext(pos);
		start = event.startTime;
		end = event.stopTime;
		chnn = event.dwChannel;
	}
	else if(sel == 2)
	{
		NET_SDK_REC_TIME &time = m_timeList.GetNext(pos);
		start = time.startTime;
		end = time.stopTime;
		chnn = time.dwChannel;
	}
	else if (sel == 3)
	{
		return;
	}
#ifndef MULTI_PLAYBACK
	CPlayBackDlg dlg;
	dlg.setPlayBackInfo(m_userID, start, end, chnn);
	dlg.DoModal();
#else
	CPlayBackDlg* pPlayBackDlg = new CPlayBackDlg;
	ASSERT(pPlayBackDlg);
	pPlayBackDlg->Create(IDD_PLAYBACK_DIALOG, this);
	pPlayBackDlg->setPlayBackInfo(m_userID, start, end, chnn);
	pPlayBackDlg->ShowWindow(SW_SHOW);

	m_listPlaybackDlg.push_back(pPlayBackDlg);
#endif
	*pResult = 0;
}