#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"


// CMsgDlg 对话框

class CMsgDlg : public CDialog
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMsgDlg();

// 对话框数据
	enum { IDD = IDD_MSG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnRecieveMsg(WPARAM wparam, LPARAM lparam);
	afx_msg void OnBnClickedButtonMsgClear();
public:
	CListCtrl m_msgList;
	//afx_msg void OnBnClickedCloseMsgDlg();
};
