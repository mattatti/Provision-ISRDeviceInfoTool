#pragma once
#include "afxwin.h"
#include "DVR_NET_SDK.h"
#include "afxcmn.h"

// CDeviceDlg 对话框
const static int DISCOVER_WAIT_SEC = 4;

class CDeviceDlg : public CDialog
{
	DECLARE_DYNAMIC(CDeviceDlg)

public:
	CDeviceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDeviceDlg();
	CString GetIp(){return m_IP;}
	WORD GetPort(){return m_Port;}

// 对话框数据
	enum { IDD = IDD_DEVICE_DIALOG };

protected:
	CProgressCtrl m_progress;
	CListBox m_deviceList;
	CString m_IP;
	WORD	m_Port;
	int		m_curProgress;

	NET_SDK_DEVICE_DISCOVERY_INFO m_devs[100];
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLbnDblclkListDevice();
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	
	static DWORD WINAPI DiscoverThread(LPVOID param);
public:
	afx_msg void OnBnClickedProductSubId();
};
