
#pragma once
#include "afxcmn.h"


// CFormatDlg 对话框

class CFormatDlg : public CDialog
{
	DECLARE_DYNAMIC(CFormatDlg)

public:
	CFormatDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFormatDlg();

// 对话框数据
	enum { IDD = IDD_FORMAT_DIALOG };
	void SetFormatHandle(LONG handle){m_formatHandle = handle;}

protected:
	LONG			m_formatHandle;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_progressCtrl;
};
