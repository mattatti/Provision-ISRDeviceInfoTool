// SDKDEMODlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "SDKDEMODlg.h"
#include "LoginDlg.h"
#include "LiveDlg.h"
#include "SearchDlg.h"

#include "ToolDlg.h"
#include "ConfigDlg.h"
#include "MsgDlg.h"
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
#include "IPTool_SearchAndNotifySDK.h"
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "strdef.h"
#include "resource.h"


// CSDKDEMODlg 对话框
//#define test_ipv6
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
void WINAPI SearchCallBack(char* hwaddr, int opt, const char* szXmlData, void *pParam)
{
	TRACE("hwaddr = %s, opt = %d, szXmlData=%s \n", hwaddr, opt, szXmlData);
	return ;
}
#endif
int mymutex = 1;
CMsgDlg a;

CSDKDEMODlg::CSDKDEMODlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSDKDEMODlg::IDD, pParent)
	, m_userID(-1)
	, m_liveDlg(NULL)
	, m_bankSignedDlg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_liveDlg = new CLiveDlg;
	//m_searchDlg = new CSearchDlg;
	//m_toolDlg = new CToolDlg;
	//m_configDlg = new CConfigDlg;
	m_msgDlg = new CMsgDlg;

	ZeroMemory(&m_deviceInfo, sizeof(NET_SDK_DEVICEINFO));
}

CSDKDEMODlg::~CSDKDEMODlg()
{
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
	IPTOOL_SEARCH_AND_NOTIFY_Stop_Search();
#endif
	if (m_liveDlg)
	{
		delete m_liveDlg;
		m_liveDlg = NULL;
	}
	if (m_toolDlg)
	{
		delete m_toolDlg;
		m_toolDlg = NULL;
	}
//	if (m_configDlg)
//	{
//		delete m_configDlg;
//		m_configDlg = NULL;
//	}
//	if (m_searchDlg)
//	{
//		delete m_searchDlg;
///		m_searchDlg = NULL;
//	}

	if (m_msgDlg)
	{
		delete m_msgDlg;
		m_msgDlg = NULL;
	}

}


void CSDKDEMODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_TAB, m_tab);
	
}

BEGIN_MESSAGE_MAP(CSDKDEMODlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSDKDEMODlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSDKDEMODlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_MAIN_TAB, &CSDKDEMODlg::OnTcnSelchangeMainTab)
	ON_MESSAGE(WM_SHOW_MSG_DLG, &CSDKDEMODlg::OnShowMsgDlg)
//	ON_BN_CLICKED(IDC_BUTTON_LogTest, &CSDKDEMODlg::OnBnClickedButtonLogtest)
END_MESSAGE_MAP()


// CSDKDEMODlg 消息处理程序

BOOL CSDKDEMODlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//m_tab.InsertItem(0, STR_LIVE);
	//m_tab.InsertItem(1, STR_SEARCH_BACKUP);
    //m_tab.InsertItem(2, STR_CONFIG);
	m_tab.InsertItem(0, "Info");

	CRect tabRect;
	m_tab.GetClientRect(&tabRect);

	tabRect.left += 2;
	tabRect.top += 20;
	tabRect.bottom -= 3;
	tabRect.right -= 3;

	
	m_liveDlg->Create(IDD_LIVE_DIALOG, &m_tab);
	m_liveDlg->MoveWindow(&tabRect);
	
	//m_searchDlg->Create(IDD_SEARCH_DIALOG, &m_tab);
	//m_searchDlg->MoveWindow(&tabRect);

	

	//m_configDlg->Create(IDD_CONFIG_DIALOG, &m_tab);
	//m_configDlg->MoveWindow(&tabRect);

	m_msgDlg->Create(IDD_MSG_DIALOG, NULL);
	
	m_msgDlg->ShowWindow(FALSE);

	NET_SDK_DEVICE_IP_INFO pDeviceIPInfo;
	memset(&pDeviceIPInfo, 0, sizeof(NET_SDK_DEVICE_IP_INFO));
	strcpy(pDeviceIPInfo.szMac, "00:18:AE:51:CF:FC");	//设备当前mac地址
	strcpy(pDeviceIPInfo.szIpAddr, "192.168.7.193");		
	strcpy(pDeviceIPInfo.szMark, "255.255.255.0");		
	strcpy(pDeviceIPInfo.szGateway, "192.168.7.1");			
    strcpy(pDeviceIPInfo.szDdns1, "192.168.7.1");
    strcpy(pDeviceIPInfo.szDdns2, "8.8.8.8");
	strcpy(pDeviceIPInfo.szPassword, "123456");				//admin用户的密码
	pDeviceIPInfo.ucIPMode = 0;
	bool bRet = NET_SDK_ModifyDeviceNetInfo(&pDeviceIPInfo);

	NET_SDK_SetSDKMessageCallBack(0, 0, excertionCallback, NULL);
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
	IPTOOL_SEARCH_AND_NOTIFY_Start_Search(SearchCallBack, this);
#endif
	if(m_userID == -1)//主动登录方式，未登录
	{
		//登录
		CLoginDlg dlg;	
		dlg.SetIp(m_IP);
		dlg.SetPort(m_port);
		
		while(true)
		{
			if (IDOK == dlg.DoModal())
			{
				
#ifdef test_ipv6
				LONG userId = NET_SDK_LoginEx("[fe80::14eb:95ff:fe40:c812]"/*dlg.GetIP().GetBuffer()*/, dlg.GetPort(), dlg.GetUserName().GetBuffer(), dlg.GetPassword().GetBuffer(), &m_deviceInfo,\
					dlg.GetConnectType(), dlg.GetDeviceSN().GetBuffer());
#else
				LONG userId = NET_SDK_LoginEx(dlg.GetIP().GetBuffer(), dlg.GetPort(), dlg.GetUserName().GetBuffer(), dlg.GetPassword().GetBuffer(), &m_deviceInfo,\
					dlg.GetConnectType(), dlg.GetDeviceSN().GetBuffer());
#endif
				if (userId >= 0)
				{
					m_IP = dlg.GetIP().GetBuffer();
					m_userID = userId;
					break;
				}
				else
				{
					LONG error = NET_SDK_GetLastError();
					CString msg = NET_SDK_GetErrorMsg(&error);
					if (error == NET_SDK_DEVICE_OFFLINE)
					{
						AfxMessageBox("Device offline!");
					}
					else
					{
						AfxMessageBox(STR_LOGIN_FAILED);
					}
				}
			}
			else
			{
				OnCancel();
				return FALSE;
			}
		}
		m_deviceInfo.deviceIP = inet_addr(m_IP);
	}
	else	//被动登录方式
	{
		m_deviceInfo.deviceIP = m_deviceInfo.deviceIP;;
	}

	
	AfxMessageBox(_T("Loading takes a few seconds, Press ok to confirm."));
	
	m_toolDlg = new CToolDlg(NULL,m_userID, m_deviceInfo);
	m_toolDlg->Create(IDD_TOOL_DIALOG, &m_tab);
	m_toolDlg->MoveWindow(&tabRect);
	m_msgDlg->SetWindowPos(NULL, 0, 300, 620, 450,NULL);
	m_toolDlg->setMainDlg(this);
	m_toolDlg->ShowWindow(SW_SHOW);


	//5

	LPNET_SDK_DEVICEINFO a=nullptr;

	NET_SDK_GetDeviceInfo(m_userID, a);

	NET_SDK_CHANNEL_PTZ *pOutChannelPtz = new NET_SDK_CHANNEL_PTZ[m_deviceInfo.videoInputNum];
	int returnListNum = 0;
	NET_SDK_GetSupportPtzList(m_userID, m_deviceInfo.videoInputNum, pOutChannelPtz, &returnListNum);




	NET_SDK_CAMERA_TYPE *pCameraType=nullptr;
	NET_SDK_GetPTZCameraType(m_userID, pCameraType);

	NET_SDK_DEVICE_FIRMWARE_TYPE pFirmware;
	NET_SDK_GetDeviceFirmwareType(m_userID, &pFirmware);

	//NET_SDK_SetSDKMessageCallBack(0, 0, excertionCallback, NULL);
	
	NET_SDK_SetDVRMessageCallBack(msgCallback, this);
	m_ipMap[m_userID] = m_IP;
	m_msgDlg->ShowWindow(SW_SHOW);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
/*void CSDKDEMODlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	progress
	CSDKDEMODlg::OnShowWindow(bShow, nStatus);
	m_progress.SetRange(0, 100);

	m_progress.SetPos(0);

	m_progress.SetStep(1);
	this->KillTimer(1);
	this->SetTimer(1, 20, NULL);

	// TODO: Add your message handler code here
}
void CSDKDEMODlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CSDKDEMODlg::OnTimer(nIDEvent);


	for (int i = 1; i <= 100; i++)
	{
		Sleep(50);
		m_progress.SetPos(i);

	}

	this->EndDialog(true);
}*/
void CSDKDEMODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
		
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CSDKDEMODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSDKDEMODlg::OnBnClickedOk()
{
	
}

void CSDKDEMODlg::OnBnClickedCancel()
{
	//OnCancel();
	if (IDYES == AfxMessageBox(STR_CONFIRM_QUIT, MB_YESNO))
	{
		if (m_userID >= 0)
		{
			NET_SDK_Logout(m_userID);
			m_userID = -1;
		}
		OnCancel();
	}
}


void CSDKDEMODlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CSDKDEMODlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	
	if (!IsWindow(m_tab.GetSafeHwnd()))
	{
		return ;
	}
	//if (m_tab.GetCurSel() == 0)
	//{
	//	m_liveDlg->OnMove(x, y);
	//}
}

void CSDKDEMODlg::OnTcnSelchangeMainTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	int sel = m_tab.GetCurSel();
//	if (m_liveDlg->IsWindowVisible())
//	{
//		m_liveDlg->ShowWindow(SW_HIDE);
//	}
	
//	if (m_searchDlg->IsWindowVisible())
//	{
//		m_searchDlg->ShowWindow(SW_HIDE);
//	}

	if (m_toolDlg->IsWindowVisible())
	{
		m_toolDlg->ShowWindow(SW_HIDE);
	}
	
//	if (m_configDlg->IsWindowVisible())
//	{
//		m_configDlg->ShowWindow(SW_HIDE);
//	}

	switch (sel)
	{
	//case 0:
	//	{
	//		m_liveDlg->ShowWindow(SW_SHOW);
	//		break;
	//	}
	//case 1:
	//	{
	//		m_searchDlg->ShowWindow(SW_SHOW);
	//		break;
	//	}
//	case 2:
//		{
//			m_configDlg->ShowWindow(SW_SHOW);
//			break;
//		}
	case 0:
		{
			m_toolDlg->ShowWindow(SW_SHOW);
			break;
		}
	default:break;
	}
	*pResult = 0;
}

void CALLBACK CSDKDEMODlg::excertionCallback( DWORD dwType, LONG lUserID, LONG lHandle, void *pUser )
{
	/*CString temp;
	temp.Format(_T("type:%d, userID:%d, handle:%d"), dwType, lUserID, lHandle);
	AfxMessageBox(temp);*/
	if (NETWORK_DISCONNECT == dwType)
	{
		CString temp;
		temp.Format(_T("\r\ntype:%d, userID:%d, handle:%d---NETWORK_DISCONNECT\r\n"), dwType, lUserID, lHandle);
		::OutputDebugString(temp);
	}
	else if (NETWORK_RECONNECT == dwType)
	{
		CString temp;
		temp.Format(_T("\r\ntype:%d, userID:%d, handle:%d---NETWORK_RECONNECT\r\n"), dwType, lUserID, lHandle);
		::OutputDebugString(temp);
	}
	else if (NETWORK_CH_DISCONNECT == dwType)
	{
		CString temp;
		temp.Format(_T("\r\ntype:%d, userID:%d, channel:%d---NETWORK_CH_DISCONNECT\r\n"), dwType, lUserID, lHandle);
		::OutputDebugString(temp);
	}
	else if (NETWORK_CH_RECONNECT == dwType)
	{
		CString temp;
		temp.Format(_T("\r\ntype:%d, userID:%d, channel :%d---NETWORK_CH_RECONNECT\r\n"), dwType, lUserID, lHandle);
		::OutputDebugString(temp);
	}
}

BOOL CALLBACK CSDKDEMODlg::msgCallback( LONG lCommand, LONG lUserID, char *pBuf, DWORD dwBufLen, void *pUser )
{
	a.m_msgList.DeleteAllItems();
	if (mymutex==1) {
		mymutex = 0;
		bool ossiaflag;
		CString temp, temp2, ttemp, ttemp2, ttype;
		CSDKDEMODlg *pThis = (CSDKDEMODlg *)pUser;

		const auto MAX_CHANNEL_NUMBER = 128;
		DD_ALARM_STATUS_INFO ec[MAX_CHANNEL_NUMBER];
		DWORD lpBytesReturned = 0;
		
		CList<NET_SDK_DISK_INFO> m_diskList;

		int diskcount = 0;
		m_diskList.RemoveAll();

		LONG diskHandle = NET_SDK_FindDisk(lUserID);
		CString m_NET_SDK_disk_status;
		CString m_disk_Property;

		if (diskHandle > 0)
		{
			while (true)
			{
				NET_SDK_DISK_INFO diskInfo;
				if (!NET_SDK_GetNextDiskInfo(diskHandle, &diskInfo))
				{
					NET_SDK_FindDiskClose(diskHandle);
					if (diskcount == 0)
					{
						//	m_disk_status = _T("NODISK");
						m_NET_SDK_disk_status = "No Disk";
					}
					break;
				}

				m_diskList.AddTail(diskInfo);

				//if (diskInfo.diskStatus == NET_SDK_DISK_NORMAL)
				//{
				//	m_NET_SDK_disk_status = STR_NORMAL;
				//}
				if (diskInfo.diskStatus == NET_SDK_DISK_NO_FORMAT)
				{
					m_NET_SDK_disk_status = "Unformatted Disk";
					ttype = "Disk Error";
					ttemp2.Format("Unformatted Disk");
					//m_NET_SDK_disk_status = "Unformatted Disk";
				}

				//if (diskInfo.diskProperty == NET_SDK_DISK_READ_WRITE) 
				//{
				//	m_disk_Property = STR_READ_WRITE;
				//}
				if (diskInfo.diskProperty == NET_SDK_DISK_ONLY_READ) //what is this alarm
				{
					m_disk_Property = STR_READ_ONLY;
				}
				diskcount++;
			}
		}
		if (diskcount == 0)
		{
			//	m_disk_status = _T("NODISK");
			m_NET_SDK_disk_status = "No Disk";
		}
		if (ttemp2 != "") 
		{
			temp2.Format("Type:%s | %s", _T(ttype), _T(ttemp2));
			pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp2.GetBuffer());
			temp2.ReleaseBuffer();
			ttemp2 = "";
		}
		
		
		//----------------------------------------------------------------------------------------//
		NET_SDK_GetAlarmStatus(lUserID, ec, MAX_CHANNEL_NUMBER * sizeof(DD_ALARM_STATUS_INFO), &lpBytesReturned);
		const auto alarm_isize = ec[0].iSize;
		
		if (alarm_isize < MAX_CHANNEL_NUMBER) 
		{
			for (unsigned int i = 0; ; i++)
			{
				ossiaflag = true;
				switch (ec[i].alarmType)
				{
				case NET_SDK_N9000_ALARM_TYPE_NET_DISCONNECT:
					ttype = "NET_DISCONNECT";
					ttemp.Format(STR_CHANNEL_D, ec[i].chanl + 1);
					break;
				case NET_SDK_N9000_ALARM_TYPE_VLOSS:
					ttype = STR_VIDEO_LOSS;
					ttemp.Format(STR_CHANNEL_D, ec[i].chanl + 1);

					break;
				case NET_SDK_N9000_ALARM_TYPE_FRONT_OFFLINE:
					ttype = "Camera Offline";
					ttemp.Format(STR_CHANNEL_D, ec[i].chanl + 1);

					break;
				case NET_SDK_N9000_ALARM_TYPE_NO_DISK:
					if (m_NET_SDK_disk_status != "Unformatted Disk") 
					{
						
							ttype = "Disk Error";
							ttemp.Format("No Disk");
						
					}
					break;
				case NET_SDK_N9000_ALARM_TYPE_HDD_PULL_OUT:
					ttype = "Disk Error";
					ttemp.Format("HDD Pull Out");
					break;
				case NET_SDK_N9000_ALARM_TYPE_DISK_IO_ERROR: // what is this alarm ? any use ?
					ttype = "Disk Error";
					ttemp.Format("Disk IO Error");
					break;
					/* TODO add custom alarm cases here */
				default:;
				}
				if (ttemp  != "") 
				{
					temp.Format("Type:%s | %s", _T(ttype), _T(ttemp));
					pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
					temp.ReleaseBuffer();
					ttemp = "";
				}
				if(ec[i].iSize != alarm_isize)
				{
					temp2.Format("----------------------------------------");
					pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp2.GetBuffer());
					temp.ReleaseBuffer();
					ttemp = "";
					break;
				}
			}
		}


		 if (lCommand == NET_SDK_ALARM && ossiaflag == false)
		{
			int diskcount = 0;
			

			LONG diskHandle = NET_SDK_FindDisk(1);
			if (diskHandle > 0)
			{
				NET_SDK_DISK_INFO diskInfo;
				if (!NET_SDK_GetNextDiskInfo(diskHandle, &diskInfo))
				{
					NET_SDK_FindDiskClose(diskHandle);
					if (diskcount == 0)
					{
						ttype = "Disk Error";
						ttemp.Format("No Disk");
						temp.Format("Type:%s | %s", _T(ttype), _T(ttemp));
						pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
						temp.ReleaseBuffer();
					}

				}
			}
			int num = dwBufLen / sizeof(NET_SDK_ALARMINFO);
			NET_SDK_ALARMINFO *ai = reinterpret_cast<NET_SDK_ALARMINFO *>(pBuf);

			for (int i = 0; i < num; i++)
			{
				switch (ai[i].dwAlarmType)
				{

				case NET_SDK_ALARM_TYPE_SENSOR:
					ttype = STR_SENSOR;
					ttemp.Format(STR_SENSOR_D, ai[i].dwSensorIn);
					break;
				case NET_SDK_ALARM_TYPE_VLOSS:
					ttype = STR_VIDEO_LOSS;
					ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel + 1);
					break;
				case NET_SDK_ALARM_TYPE_SHELTER:
					ttype = STR_SHELTER;
					ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel + 1);
					break;
				case NET_SDK_ALARM_TYPE_DISK_FULL:
					ttype = STR_DISK_FULL;
					ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
					break;
				case NET_SDK_ALARM_TYPE_DISK_UNFORMATTED:
					ttype = STR_DISK_UNFORMAT;
					ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
					break;
				case NET_SDK_ALARM_TYPE_DISK_WRITE_FAIL:
					ttype = STR_DISK_READ_WRITE_ERROR;
					ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
					break;
				}
				if (strcmp(ttemp.GetBuffer(), "") != 0) {
					temp.Format("Type:%s | %s", _T(ttype), _T(ttemp));
					pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
					temp.ReleaseBuffer();
				}

				if(i+1 == num)
				{
					temp.Format("----------------------------------------");
					pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
					temp.ReleaseBuffer();
					break;
				}
			}

		}
		else if (lCommand == NET_SDK_RECORD)
		{
			int num = dwBufLen / sizeof(NET_SDK_RECORD_STATUS);
			NET_SDK_RECORD_STATUS *rs = reinterpret_cast<NET_SDK_RECORD_STATUS *>(pBuf);

			for (int i = 0; i < num; i++)
			{
				CString temp;
#if 0
				CString ttype;
				switch (rs[i].dwRecordType)
				{
				case DD_RECORD_TYPE_MANUAL:
					ttype = STR_MANUAL_RECORD;
					break;
				case DD_RECORD_TYPE_SCHEDULE:
					ttype = STR_SCHEDULE_RECORD;
					break;
				case DD_RECORD_TYPE_MOTION:
					ttype = STR_MOTION_RECORD;
					break;
				case DD_RECORD_TYPE_SENSOR:
					ttype = STR_SENSOR_RECORD;
					break;
				case DD_RECORD_TYPE_BEHAVIOR:
					ttype = STR_BEHIVOR_RECORD;
					break;
				case DD_RECORD_TYPE_NONE:
					ttype = STR_NONE_RECORD;
					break;
				}
				temp.Format(STR_RECORD_MSG_FORMAT, pThis->m_ipMap[lUserID], rs->dwChannel, ttype);
#endif
				CString ttype = "";

				if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_SENSOR))
				{
					if (ttype.IsEmpty())
					{
						ttype = STR_SENSOR_RECORD;
					}
					else
					{
						ttype += "|";
						ttype += STR_SENSOR_RECORD;
					}
				}
				if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_BEHAVIOR))
				{
					if (ttype.IsEmpty())
					{
						ttype = STR_BEHIVOR_RECORD;
					}
					else
					{
						ttype += "|";
						ttype += STR_BEHIVOR_RECORD;
					}
				}
				
			}
		}
		else
		{
			ASSERT(FALSE);
		}
		//NET_SDK_CloseAlarmChan(m_msgCBHandle);
		return 0;
	}
}

LRESULT CSDKDEMODlg::OnShowMsgDlg(WPARAM wparam, LPARAM lparam)
{
	int ifShow = (int)lparam;
	if (ifShow)
	{
		m_msgDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_msgDlg->ShowWindow(SW_HIDE);
	}
	return 0;
}

DWORD WINAPI CSDKDEMODlg::OnLoginThread(LPVOID lParam)
{
	NET_SDK_LOGIN_INFO *pLoginInfo = static_cast<NET_SDK_LOGIN_INFO*>(lParam);
	if (!pLoginInfo)
	{
		return -1;
	}

	NET_SDK_DEVICEINFO deviceInfo = {0};
	DWORD dwTime = ::GetTickCount();
	TRACE("NET_SDK_Login begin, IP=%s,SN=%s, connectType=%d \r\n", pLoginInfo->deviceIP, pLoginInfo->deviceSN, pLoginInfo->connectType);
#ifdef test_ipv6
	LONG lUserID = NET_SDK_LoginEx("[fe80::14eb:95ff:fe40:c812]"/*pLoginInfo->deviceIP*/, pLoginInfo->devicePort, pLoginInfo->userName, pLoginInfo->passwd, &deviceInfo,\
		(NET_SDK_CONNECT_TYPE)pLoginInfo->connectType, pLoginInfo->deviceSN);
#else
	LONG lUserID = NET_SDK_LoginEx(pLoginInfo->deviceIP, pLoginInfo->devicePort, pLoginInfo->userName, pLoginInfo->passwd, &deviceInfo,\
		(NET_SDK_CONNECT_TYPE)pLoginInfo->connectType, pLoginInfo->deviceSN);
#endif
	TRACE("NET_SDK_Login end, IP=%s,SN=%s, connectType=%d, use time=%d \r\n", pLoginInfo->deviceIP, pLoginInfo->deviceSN, pLoginInfo->connectType, GetTickCount()-dwTime);

	if (pLoginInfo)
	{
		delete pLoginInfo;
		pLoginInfo = NULL;
	}
	return 0;
}