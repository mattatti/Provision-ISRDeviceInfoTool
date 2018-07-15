#pragma once
#include "afxcmn.h"
#include "DVR_NET_SDK.h"
#include "BackupDlg.h"
#include "PlayBackDlg.h"
#include <map>
#include <list>
using namespace std;


// CSearchDlg 对话框

typedef map<void*, CBackupDlg*>				mapBackDlg;
typedef map<void*, CBackupDlg*>::iterator	mapBackDlgIter;
typedef list<CPlayBackDlg*>					listPlaybackDlg;
typedef list<CPlayBackDlg*>::iterator		listPlaybackDlgIter;

class CSearchDlg : public CDialog
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSearchDlg();

// 对话框数据
	enum { IDD = IDD_SEARCH_DIALOG };

protected:
	CTabCtrl				m_searchType;
	NET_SDK_DEVICEINFO		*m_deviceInfo;
	CList<NET_SDK_REC_FILE> m_fileList;
	CList<NET_SDK_REC_EVENT>m_eventList;
	CList<NET_SDK_REC_TIME> m_timeList;
	CList<DD_DATE>			m_dateList;
	LONG					m_eventType;		
	LONG					m_userID;
	int						m_chnn;
	CTime					m_searchDate;
	CListCtrl				m_resultList;
	CString					m_backPath;
	mapBackDlg				m_mapBackDlg;
	CBackupDlg*				m_pBackDlg;
	listPlaybackDlg			m_listPlaybackDlg;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void SetDeviceInfo(NET_SDK_DEVICEINFO* di, LONG userId);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTabSearchtype(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonLock();
	afx_msg void OnBnClickedButtonUnlock();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonBackup();
	afx_msg void OnBnClickedButtonBrowser();
	afx_msg void OnBnClickedRadioEventtype();
	afx_msg void OnNMDblclkListResult(NMHDR *pNMHDR, LRESULT *pResult);
};
