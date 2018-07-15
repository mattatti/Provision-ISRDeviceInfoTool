#pragma once
#include "GridCtrl.h"

// CCruiseDlg 对话框

class CCruiseDlg : public CDialog
{
	DECLARE_DYNAMIC(CCruiseDlg)

public:
	CCruiseDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCruiseDlg();

// 对话框数据
	enum { IDD = IDD_CRUISE_DIALOG };

protected:
	CGridCtrl		m_grid;
	int				m_cruiseNum;
	BYTE			*m_data;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int GetCruiseNum(){return m_cruiseNum;}
	void GetCruiseInfo(void *data);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonDeleteall();
};
