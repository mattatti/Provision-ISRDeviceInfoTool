#pragma once


// CConnectModeDlg 对话框

class CConnectModeDlg : public CDialog
{
	DECLARE_DYNAMIC(CConnectModeDlg)

public:
	CConnectModeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConnectModeDlg();

// 对话框数据
	enum { IDD = IDD_CONNECT_MODE };
	enum 
	{
		CONNECT_MODE_LOGIN_TO_DEVICE,
		CONNECT_MODE_REGISTER_TO_CENTER,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedRadioLogin();
	afx_msg void OnBnClickedRadioAuto();
	
public:
	UINT m_registerPort;
	int m_connectMode;
};
