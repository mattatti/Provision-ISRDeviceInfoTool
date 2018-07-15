// SDKDEMODlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "DVR_NET_SDK.h"
#include "resource.h"


#define CONFIG_FILE_NAME	_T("SDK_config.ini")


//登陆信息定义
typedef struct _net_sdk_login_info
{
	char				deviceIP[32];		//device IP
	unsigned short		devicePort;			//port
	unsigned short		connectType;		//connect type (NET_SDK_CONNECT_TYPE)
	char				userName[32];		//
	char				passwd[32];
	char				deviceSN[32];		//device serial number(only for P2P mode)
	char				resv[16];			//reserve
}NET_SDK_LOGIN_INFO, *LPNET_SDK_LOGIN_INFO;


class CLiveDlg;
class CSearchDlg;
class CToolDlg;
class CConfigDlg;
class CMsgDlg;
class CBankSignedDlg;

// CSDKDEMODlg 对话框
class CSDKDEMODlg : public CDialog
{
// 构造
public:
	CSDKDEMODlg(CWnd* pParent = NULL);	// 标准构造函数
	~CSDKDEMODlg();
// 对话框数据
	enum { IDD = IDD_SDKDEMO_DIALOG };

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	
	void SetDeviceInfo(CString ip, WORD port){m_IP = ip;m_port = port;}

	void SetDeviceInfo(LONG userID, NET_SDK_DEVICEINFO *pDeviceInfo)
	{
		m_userID = userID;
		m_deviceInfo = *pDeviceInfo;
	}

// 实现
protected:
	HICON m_hIcon;
	CTabCtrl m_tab;
	CString m_IP;
	WORD	m_port;
	
	LONG					m_userID;
	NET_SDK_DEVICEINFO		m_deviceInfo;
	CLiveDlg				*m_liveDlg;
	CSearchDlg				*m_searchDlg;
	CToolDlg				*m_toolDlg;
	CConfigDlg				*m_configDlg;
	CMsgDlg					*m_msgDlg;
	CMap<LONG, LONG&, CString, CString&> m_ipMap;
	CBankSignedDlg			*m_bankSignedDlg;	//

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg
		void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnTimer(UINT_PTR nIDEvent);
	void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTcnSelchangeMainTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnShowMsgDlg(WPARAM wparam, LPARAM lparam);
	static void CALLBACK excertionCallback(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser);
	static BOOL CALLBACK msgCallback(LONG lCommand, LONG lUserID, char *pBuf, DWORD dwBufLen, void *pUser);

	afx_msg void OnBnClickedButtonLogtest();

	static DWORD WINAPI OnLoginThread(LPVOID lParam);
	
};
