// ToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "ToolDlg.h"
#include "GridCellCombo.h"
#include "FormatDlg.h"
#include <WinSock2.h>
#include <Wincrypt.h>
#include "strdef.h"
#include <MMSystem.h>
#include "resource.h"
#include "MsgDlg.h"
#include "PlayBackDlg.h"

// CToolDlg 对话框

IMPLEMENT_DYNAMIC(CToolDlg, CDialog)
// bool onCloseMsgWindowFlag;
CToolDlg::CToolDlg(CWnd* pParent /*=NULL*/,LONG userid, NET_SDK_DEVICEINFO m_deviceInfo)
	: CDialog(CToolDlg::IDD, pParent)
	//, m_logStartTime(0)
	, m_logEndTime(0)
	, m_isStartVoiceComm(FALSE)
	, m_isStartVoiceComm_MR(FALSE)
	, m_isOpenMsgCallback(FALSE)
	, m_runTalkTransThread(FALSE)
	, m_talkDataFile(INVALID_HANDLE_VALUE)
	, m_voiceHandle(-1)
	, m_voiceVolume(0)
	, m_sysTime(CTime::GetCurrentTime())
	, m_device_time(_T("1"))
	, m_device_model(_T(""))
	//, m_con_IP_cams(0)
	, m_http_port(0)
	, m_firmware_version(_T(""))
	, m_build_date(_T(""))
	, m_hardware_version(_T(""))
	, m_last_record_date(_T(""))
	, m_MAC_address(_T(""))
{
	
#ifdef N9000_CONFIG
//	InitN9000LogTypeList(); //初始化N9000日志列表
#endif
	m_userID = userid;
	m_deviceinfo =   m_deviceInfo;
	
}

CToolDlg::~CToolDlg()
{
}

void CToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	//DDX_GridControl(pDX, IDC_CUSTOM_DISK_LIST, m_diskGrid);
	DDX_Text(pDX, IDC_DEVICE_TIME, m_device_time);
	DDV_MaxChars(pDX, m_device_time, 50);
	DDX_Text(pDX, IDC_DEVICE_MODEL, m_device_model);
	DDV_MaxChars(pDX, m_device_model, 50);
	//DDX_Text(pDX, IDC_DEVICE_CON_IP_CAMS, m_con_IP_cams);
	//DDV_MinMaxInt(pDX, m_con_IP_cams, 0, 128);
	DDX_Text(pDX, IDC_DEVICE_HTTP_PORT, m_http_port);
	DDV_MinMaxInt(pDX, m_http_port, 1024, 65535);
	DDX_Text(pDX, IDC_DEVICE_FIRMWARE, m_firmware_version);
	DDV_MaxChars(pDX, m_firmware_version, 50);
	DDX_Text(pDX, IDC_DEVICE_FIRMWARE_BUILD_DATE, m_build_date);
	DDX_Text(pDX, IDC_DEVICE_HARDWARE_VERSION, m_hardware_version);
	DDV_MaxChars(pDX, m_hardware_version, 50);
	DDX_Text(pDX, IDC_DEVICE_LAST_RECORD_DATE, m_last_record_date);
	DDV_MaxChars(pDX, m_last_record_date, 50);
	DDX_Text(pDX, IDC_DEVICE_MAC_ADDRESS, m_MAC_address);
	DDV_MaxChars(pDX, m_MAC_address, 20);
}


BEGIN_MESSAGE_MAP(CToolDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CToolDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CToolDlg::OnBnClickedCancel)

	ON_WM_HSCROLL()
	
	//ON_BN_CLICKED(IDC_BUTTON_DISK_REFRESH, &CToolDlg::OnBnClickedButtonDiskRefresh)
	//ON_BN_CLICKED(IDC_BUTTON_CHANGE_DISK_PROPERTY, &CToolDlg::OnBnClickedButtonChangeDiskProperty)
	//ON_BN_CLICKED(IDC_BUTTON_DISK_FORMAT, &CToolDlg::OnBnClickedButtonDiskFormat)
	ON_BN_CLICKED(IDC_BUTTON_MSG_CALLBACK, &CToolDlg::OnBnClickedButtonMsgCallback)
	
	ON_BN_CLICKED(IDC_BUTTON1, &CToolDlg::OnBnClickedLogout)
	ON_BN_CLICKED(IDC_BUTTON4, &CToolDlg::OnBnClickedRefreshInfo)
	//ON_WM_CTLCOLOR()
//	ON_BN_CLICKED(IDC_BUTTON_ALARM_LIST, &CToolDlg::OnBnClickedButtonAlarmList)
END_MESSAGE_MAP()


// CToolDlg 消息处理程序

void CToolDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CToolDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

BOOL CToolDlg::OnInitDialog()
{
	
	setDeviceInfo();
	
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CToolDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CPoint point;
	GetCursorPos(&point);

	CRect rect;
	//GetDlgItem(IDC_SLIDER_VOICECOMM_VOLUME)->GetWindowRect(&rect);
	
	UpdateData();

	if (rect.PtInRect(point))
	{
		unsigned short volume = 0;
		double temp = (((double)m_voiceVolume) / 100) * 0xffff;
		volume = (short )temp;

		NET_SDK_SetVoiceComClientVolume(m_voiceHandle, volume);	
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CToolDlg::UpdateUI()
{

	if (m_isOpenMsgCallback)
	{
		//color
		
		//pDC->SetTextColor(250);
		SetDlgItemText(IDC_BUTTON_MSG_CALLBACK, "Refresh Alarm");
		
	
	}
	else if(!m_isOpenMsgCallback || onCloseMsgWindowFlag)
	{
		//pDC->SetTextColor(0);
		SetDlgItemText(IDC_BUTTON_MSG_CALLBACK, "Refresh Alarm");
	}

	//GetDlgItem(IDC_SLIDER_VOICECOMM_VOLUME)->EnableWindow(m_isStartVoiceComm);

	BOOL hasDisk = m_diskGrid.GetRowCount() > 1;
	
	//GetDlgItem(IDC_BUTTON_CHANGE_DISK_PROPERTY)->EnableWindow(hasDisk);
	//GetDlgItem(IDC_BUTTON_DISK_FORMAT)->EnableWindow(hasDisk);
	
}

void CToolDlg::OnBnClickedButtonMsgCallback()
{
	/*if (m_isOpenMsgCallback)
	{
		*/
		

	//	NET_SDK_CloseAlarmChan(m_msgCBHandle);
	//	m_mainDlg->SendMessage(WM_SHOW_MSG_DLG, 0, 0);
		m_isOpenMsgCallback = FALSE;

	//	WaitForSingleObject(m_alarmStatusThread, INFINITE);
		mymutex = 1;
		

/*	}
	else
	{
	*/	
		LONG handle  = NET_SDK_SetupAlarmChan(m_userID);
		if (handle > 0)
		{
		
			m_msgCBHandle = handle;
			m_mainDlg->SendMessage(WM_SHOW_MSG_DLG, 0, 1);

			m_isOpenMsgCallback = TRUE;
		//	m_alarmStatusThread = CreateThread(NULL, 0, &CToolDlg::AlarmStatusThread, this, 0, NULL);
		}
		else
		{
			AfxMessageBox(STR_FAILED);
		}
		
	//}
//	UpdateUI();
}

void CToolDlg::setMainDlg( CWnd* dlg )
{
	m_mainDlg = dlg;
}


void CToolDlg::OnBnClickedLogout()
{
	HWND handle = nullptr;
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	int retval = 0;
	const int  BUFSIZE = 400;
	TCHAR  buffer[BUFSIZE] = TEXT("");
	TCHAR** lppPart = { NULL };
	retval = GetFullPathName("Provision-ISR.Device.Information.exe",
		BUFSIZE,
		buffer,
		lppPart);
	ShellExecute(handle, NULL, buffer, NULL, NULL, SW_SHOWNORMAL);
	Sleep(100);
	exit(1);
}


void  CToolDlg::setDeviceInfo()
{
	m_device_time=SetDeviceTime();
	m_device_model = SetDeviceModel();
	//m_con_IP_cams = SetConIPcams();
//  m_con_IP_cams = SetConAnalogcams();
	m_http_port = SetHTTPPort();
	m_firmware_version = SetFirmwareVersion();
	m_build_date = SetFirmwareBuildDate();
	m_hardware_version = SetHardwareVersion();
	m_last_record_date = SetLastRecordDate();
	m_MAC_address = SetMACAddress();

	
}

int CToolDlg::SetConAnalogcams()
{
	DD_DEVICE_INFO di;
	unsigned long ret;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_DEVICE_INFO, -1, &di, sizeof(di), &ret, 0);
	int a = di.netVideoInNum;
	
	return di.localVideoInNum;
}



CString CToolDlg::SetDeviceTime()
{
	char deviceTime[50]{};
	DD_TIME p_time;
	int nTryTime = 5;
	BOOL bRet = FALSE;
	unsigned long time = 0;

	bool er=NET_SDK_ChangTime(m_userID, time);
	LONG error = NET_SDK_GetLastError();
	while (nTryTime > 0)
	{
		if (NET_SDK_GetDeviceTime(m_userID, &p_time)) {
			bRet = TRUE;
			p_time.year += 1900;
			p_time.month += 1;
			p_time.hour -= 2;
			if (p_time.second < 10 && p_time.minute < 10)
			{
				sprintf_s(deviceTime, sizeof(deviceTime), "%d/%d/%d %d:0%d:0%d",
					p_time.mday, p_time.month, p_time.year, p_time.hour, p_time.minute, p_time.second);
			}
			if (p_time.second < 10 && p_time.minute>=10)
			{
				sprintf_s(deviceTime, sizeof(deviceTime), "%d/%d/%d %d:%d:0%d",
					p_time.mday, p_time.month, p_time.year, p_time.hour, p_time.minute, p_time.second);
			}
			if (p_time.second >= 10 && p_time.minute < 10)
			{
				sprintf_s(deviceTime, sizeof(deviceTime), "%d/%d/%d %d:0%d:%d",
					p_time.mday, p_time.month, p_time.year, p_time.hour, p_time.minute, p_time.second);
			}
			else if(p_time.second >= 10 && p_time.minute >= 10)
			sprintf_s(deviceTime, sizeof(deviceTime), "%d/%d/%d %d:%d:%d",
				p_time.mday, p_time.month, p_time.year, p_time.hour, p_time.minute, p_time.second);
			break;
		}
		else {
			nTryTime--;
			::Sleep(500);
		}
	}
	return deviceTime;
}

CString CToolDlg::SetDeviceModel()
{
	if (m_deviceinfo.deviceProduct[0] != '\0')
	{
		return 	m_deviceinfo.deviceProduct;
	}
	else
	{
		char deviceName[50];
		int num = 0;
		unsigned long retLen = 0;
		DD_DEVICE_INFO *devInfo = new DD_DEVICE_INFO;

		NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_DEVICE_INFO, -1, devInfo,
			sizeof(DD_DEVICE_INFO), &retLen, 0);
		num = retLen / sizeof(DD_DEVICE_INFO);
		strcpy_s(deviceName, devInfo->deviceName);
		return deviceName;
	}


	
}
int CToolDlg::SetConIPcams()
{
	NET_SDK_IPC_DEVICE_INFO ipcInfo[128] = { 0 };
	int camcount = 0;
	LONG lIPCCount = 0;
	BOOL bRet0 = NET_SDK_GetDeviceIPCInfo(m_userID, ipcInfo, sizeof(ipcInfo), &lIPCCount);
	if (bRet0)
	{
		//NVR5 16400 1U 192.168.2.200
		for (int i = 0; i < lIPCCount; i++)
		{
			if (ipcInfo[i].status == 1)
				camcount++;
		}
		return  camcount;
	}
	else
	{
		return  0;
	}
}

int CToolDlg::SetHTTPPort()
{
	unsigned long retLen = 0;

	DD_NETWORK_ADVANCE_CONFIG *advNetCfg = new DD_NETWORK_ADVANCE_CONFIG;

	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_ADVANCE, -1, advNetCfg,
		sizeof(DD_NETWORK_ADVANCE_CONFIG), &retLen, 0);
	int num = retLen / sizeof(DD_NETWORK_ADVANCE_CONFIG);
	return advNetCfg->httpPort;
}

CString CToolDlg::SetFirmwareVersion()
{
	if (m_deviceinfo.firmwareVersion[0] != '\0')
	{
		build_date_flag = false;
		return m_deviceinfo.firmwareVersion;
	}

	else if (m_deviceinfo.firmwareVersionEx[0] != '\0')
	{
		build_date_flag = true;
		return m_deviceinfo.firmwareVersionEx;
		
		//pDC->TextOutW(300, 200, CString(pDoc->m_ossiaBuildDate));
	}
}

CString CToolDlg::SetFirmwareBuildDate()
{
	if (build_date_flag)
	{
		return find_date(m_deviceinfo.firmwareVersionEx);
	}
	else
		return "";
}



CString CToolDlg::find_date(char * firmware)
{
	//finds 6digits in a row and take it into a char array of 6 cells
	// then parse it into date
	//and then save it as char array
	char m_ossiaBuildDate[15];
	const int digit_in_a_row = 6;
	char datebuffer[7];
	int cnt = 0;
	for (int i = 0; firmware[i] != '\0'; i++)
	{
		if (firmware[i] <= '9' && firmware[i] >= '0')
		{
			datebuffer[cnt] = firmware[i];
			cnt++;
		}
		else
			cnt = 0;
		if (cnt == digit_in_a_row)
			break;

	}
	datebuffer[6] = '\0';

	char yearbuffer[3];
	yearbuffer[0] = datebuffer[0];
	yearbuffer[1] = datebuffer[1];
	yearbuffer[2] = '\0';
	int yeartemp = atoi(yearbuffer);
	yeartemp += 2000;
	char monthbuffer[3];
	monthbuffer[0] = datebuffer[2];
	monthbuffer[1] = datebuffer[3];
	monthbuffer[2] = '\0';
	int monthtemp = atoi(monthbuffer);
	char daybuffer[3];
	daybuffer[0] = datebuffer[4];
	daybuffer[1] = datebuffer[5];
	daybuffer[2] = '\0';
	int daytemp = atoi(daybuffer);

	sprintf_s(m_ossiaBuildDate, sizeof(m_ossiaBuildDate), "%d/%d/%d",
		daytemp, monthtemp, yeartemp);
	return m_ossiaBuildDate;
}

CString CToolDlg::SetHardwareVersion()
{
	return m_deviceinfo.hardwareVersion;
}

CString CToolDlg::SetLastRecordDate()
{
	CString strTemp;
	getLastRecordDate();
	if (m_lastRecordDate.month == NULL)
	{
		strTemp.Format("No Data, Check System Alarms.");
	}
	else
	strTemp.Format("%02d/%02d/%02d",
		m_lastRecordDate.mday, m_lastRecordDate.month, m_lastRecordDate.year);
	return strTemp;
}


void CToolDlg::getLastRecordDate()
{
	DD_DATE m_dateList[200];
	DD_TIME p_time;
	p_time.hour -= 2;
	DD_TIME p_time_start = p_time;
	p_time_start.hour -= 4;
	int datecnt = 0;
	LONG handle = NET_SDK_FindRecDate(m_userID);
	DD_DATE date;
	if (handle > 0)
	{
		while (true)
		{

			LONG result = NET_SDK_FindNextRecDate(handle, &date);
			if (result == NET_SDK_NOMOREFILE)
			{
				break;
			}
			if (result == NET_SDK_FILE_NOFIND) {
				//m_No_Record_File_Found = true;
				break;
			}

			m_dateList[datecnt] = date;
			datecnt++;
		}
		//if (m_No_Record_File_Found == false)
			m_lastRecordDate = findmaxdate(m_dateList, datecnt - 1);
		

		NET_SDK_FindRecDateClose(handle);
	}
	else
	{
		;
	//	m_No_Record_File_Found = true;
	}

}

DD_DATE CToolDlg::findmaxdate(DD_DATE m_dateList[], int size)
{
	int maxyear = m_dateList[0].year;
	int maxmonth = m_dateList[0].month;
	int maxday = m_dateList[0].mday;
	auto lastrecdate = m_dateList[0];
	if (lastrecdate.month > 12) 
	{
		lastrecdate.mday = NULL;
		lastrecdate.month = NULL;
		lastrecdate.year = NULL;
		return lastrecdate;
	}
	for (int i = 1; i < size; i++)
	{
		int tempm = m_dateList[i].month;
		int tempd = m_dateList[i].mday;
		int tempy = m_dateList[i].year;
		if (tempy > maxyear)
		{
			maxyear = m_dateList[i].year;
			maxmonth = tempm;
			maxday = tempd;
		}
		if (tempm > maxmonth)
		{
			maxmonth = tempm;
			maxday = tempd;
		}
		if (tempd > maxday && tempy == maxyear && tempm == maxmonth)
		{
			maxday = tempd;
			lastrecdate = m_dateList[i];
		}
	}
	return lastrecdate;
}

CString CToolDlg::SetMACAddress()
{
	CString  MACaddress;
	MACaddress.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"),
		m_deviceinfo.deviceMAC[0], m_deviceinfo.deviceMAC[1], m_deviceinfo.deviceMAC[2], m_deviceinfo.deviceMAC[3],
		m_deviceinfo.deviceMAC[4], m_deviceinfo.deviceMAC[5]);

	if (MACaddress[0] == '0' && MACaddress[4] == '0' && MACaddress[6]== '0')
		return "Limited User Permissions";
	else 
	{
		return MACaddress;
	}
}

void CToolDlg::checkdisk()
{
	int diskcount = 0;
	m_diskList.RemoveAll();

	LONG diskHandle = NET_SDK_FindDisk(m_userID);
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

			if (diskInfo.diskStatus == NET_SDK_DISK_NORMAL)
			{
				m_NET_SDK_disk_status = STR_NORMAL;
			}
			if (diskInfo.diskStatus == NET_SDK_DISK_NO_FORMAT)
			{
				m_NET_SDK_disk_status = STR_UNFORMAT;
			}

			if (diskInfo.diskProperty == NET_SDK_DISK_READ_WRITE)
			{
				m_disk_Property = STR_READ_WRITE;
			}
			if (diskInfo.diskProperty == NET_SDK_DISK_ONLY_READ)
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

}

void CToolDlg::OnBnClickedRefreshInfo()
{
	OnInitDialog();
}