// ConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "ConfigDlg.h"
#include "FolderDialog.h"
#include "ConfigManager.h"
#include "SafeFile.h"
#include "SafeFile.h"
#include "ConfigHelpers.h"
#include <cmath>
#include "strdef.h"
#include <io.h>

// CConfigDlg 对话框

IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)

bool UTF8ToGB(char* pDest, int nDestLen, const char* pSrc, int nSrcLen)
{
	if(!pDest || !pSrc || nDestLen <= 0 || nSrcLen <= 0)
		return false;

#ifdef WIN32
	WCHAR *strSrc = NULL;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, NULL, 0);
	strSrc = new WCHAR[i+1];
	MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, pDest, i, NULL, NULL);

	delete []strSrc;
#else

#endif
	return true;
}

void ANSItoUTF8(const char *ansi, long srcLen, char *utf8, long destLen)
{
	wchar_t *pUnicode = new wchar_t[srcLen];

	//ANSI多字节转换为Unicode
	memset(pUnicode, 0, sizeof(wchar_t)*srcLen);
	int retNum = MultiByteToWideChar(CP_ACP, 0, ansi, srcLen, pUnicode, srcLen);

	//Unicode转换为UTF8
	memset(utf8, 0, destLen);
	//预留最后一个字节存放字符串结束符
	WideCharToMultiByte(CP_UTF8, 0, pUnicode, retNum, (char *)utf8, destLen-1, NULL, NULL);
	delete [] pUnicode;
}

inline unsigned char SplitModeToNum(DD_VIEW_SPLIT_MODE mode)
{
	unsigned char num = 1;
	switch(mode)
	{
	case DD_VIEW_SPLIT_2X2:
		num = 4;
		break;
	case DD_VIEW_SPLIT_2X3:
		num = 6;
		break;
	case DD_VIEW_SPLIT_3X3:
		num = 9;
		break;
	case DD_VIEW_SPLIT_4X4:
		num = 16;
		break;
	case DD_VIEW_SPLIT_5X5:
		num = 25;
		break;
	case DD_VIEW_SPLIT_6X6:
		num = 36;
		break;
	default:
		num = 1;
		break;
	}

	return num;
}

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
	, m_isEnterConfig(FALSE)
	, m_exportPath(_T(""))
	, m_importPath(_T(""))
{
}

CConfigDlg::~CConfigDlg()
{

}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_EXPORT_PATH, m_exportPath);
	DDX_Text(pDX, IDC_EDIT_IMPORT_PATH, m_importPath);
	DDX_Control(pDX, IDC_COMBO_CONFIG_CHNN, m_chnnCombo);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CConfigDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_ENTER_EXIT_CONFIG, &CConfigDlg::OnBnClickedButtonEnterExitConfig)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_EXPORT_PATH, &CConfigDlg::OnBnClickedButtonBrowseExportPath)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_IMPORT_PATH, &CConfigDlg::OnBnClickedButtonBrowseImportPath)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CConfigDlg::OnBnClickedButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CConfigDlg::OnBnClickedButtonImport)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_GET, &CConfigDlg::OnBnClickedButtonConfigGet)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_PATH_BROWSE, &CConfigDlg::OnBnClickedButtonConfigPathBrowse)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, &CConfigDlg::OnBnClickedButtonSelectAll)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_REVERSE, &CConfigDlg::OnBnClickedButtonSelectReverse)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_DEFAULT, &CConfigDlg::OnBnClickedButtonConfigDefault)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_SAVE, &CConfigDlg::OnBnClickedButtonConfigSave)
	ON_BN_CLICKED(IDC_Btn_GetDevice, &CConfigDlg::OnBnClickedBtnGetdevice)
	ON_BN_CLICKED(IDC_Btn_SetDeviceName, &CConfigDlg::OnBnClickedBtnSetdevicename)
	ON_BN_CLICKED(IDC_BUTTON_Upgrade_PATH_BROWSE, &CConfigDlg::OnBnClickedButtonUpgradePathBrowse)
	ON_BN_CLICKED(IDC_BUTTON_Upgrade, &CConfigDlg::OnBnClickedButtonUpgrade)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_Btn_GetDeviceIPCamInfo, &CConfigDlg::OnBnClickedBtnGetdeviceipcaminfo)
END_MESSAGE_MAP()


// CConfigDlg 消息处理程序

void CConfigDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CConfigDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	EnableChildWindows(FALSE);
	UpdateUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CConfigDlg::EnableChildWindows(BOOL enable)
{
	CWnd *wnd = GetWindow(GW_CHILD);
	while (wnd != NULL)
	{
		if (wnd->GetDlgCtrlID() != IDC_BUTTON_ENTER_EXIT_CONFIG)
		{
			wnd->EnableWindow(enable);
		}
		wnd = wnd->GetWindow(GW_HWNDNEXT);
	}
}

void CConfigDlg::OnBnClickedButtonEnterExitConfig()
{
	if (!m_isEnterConfig)
	{
		LONG ret = NET_SDK_EnterDVRConfig(m_userID);
		if (ret < 0)
		{
			AfxMessageBox(STR_ENTER_CONFIG_FAILED);
		}
		else
		{
			m_isEnterConfig = TRUE;
			EnableChildWindows(TRUE);
		}
	}
	else
	{
		NET_SDK_ExitDVRConfig(m_userID);
		m_isEnterConfig = FALSE;
		EnableChildWindows(FALSE);
	}
	UpdateUI();
}

void CConfigDlg::UpdateUI()
{
	if (m_isEnterConfig)
	{
		SetDlgItemText(IDC_BUTTON_ENTER_EXIT_CONFIG, STR_EXIT_CONFIG);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_ENTER_EXIT_CONFIG, STR_ENTER_CONFIG);
	}
}

void CConfigDlg::OnBnClickedButtonBrowseExportPath()
{
	CFileDialog dlg(FALSE, _T("dat"), _T("export.dat"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("dat files(*.dat)|*.dat||"));
	if (dlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_EXPORT_PATH, dlg.GetPathName());
	}
}

void CConfigDlg::OnBnClickedButtonBrowseImportPath()
{
	CFileDialog dlg(TRUE, _T("dat"), NULL, OFN_HIDEREADONLY, _T("dat files(*.dat)|*.dat||"));
	if (dlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_IMPORT_PATH, dlg.GetPathName());
	}
}

void CConfigDlg::OnBnClickedButtonExport()
{
	UpdateData();
	if (m_exportPath.IsEmpty())
	{
		AfxMessageBox(STR_PATH_EMPTY);
		return;
	}
	BOOL ret = NET_SDK_GetConfigFile(m_userID, m_exportPath.GetBuffer());
	if (ret)
	{
		AfxMessageBox(STR_SUCCESS, MB_ICONINFORMATION);
	}
	else
	{
		AfxMessageBox(STR_FAILED);
	}
}

void CConfigDlg::OnBnClickedButtonImport()
{
#if 1
	UpdateData();
	if (m_importPath.IsEmpty())
	{
		AfxMessageBox(STR_PATH_EMPTY);
		return ;
	}

	BOOL ret = NET_SDK_SetConfigFile(m_userID, m_importPath.GetBuffer());
	if (ret)
	{
		AfxMessageBox(STR_SUCCESS, MB_ICONINFORMATION);
	}
	else
	{
		AfxMessageBox(STR_FAILED);
	}
#else
	typedef struct _dd_encode_config_n9000_
{
	unsigned long iSize;		//本结构体长度

	unsigned int resolution;	//分辨率，取值参考DD_VIDEO_SIZE
	unsigned short rate;		//帧率
	unsigned short encodeType;	//编码类型，取值参考DD_VIDEO_ENCODE_MODE
	unsigned short quality;		//画质，取值参考DD_IMAGE_QUALITY

	unsigned short minBitrate;	//码流下限
	unsigned short maxBitrate;	//码流上限
}DD_ENCODE_CONFIG_N9000;

	DD_ENCODE_CONFIG_N9000 ec;
	ec.iSize = sizeof(DD_ENCODE_CONFIG_N9000);
	ec.resolution = 0x02000000;
	ec.rate = 25;
	ec.encodeType = 2;
	ec.quality = 1;
	ec.minBitrate = 320;
	ec.maxBitrate = 1024;

	//NET_SDK_SetDVRConfig_SubStreamEncodeInfo(m_userID, 0, &ec,  sizeof(DD_ENCODE_CONFIG_N9000));
	DD_ENCODE_CONFIG_N9000_Ex ex;
	int ret = 0;
	NET_SDK_GetDVRConfig_SubStreamEncodeInfo_Ex(m_userID, 0, &ex, sizeof(DD_ENCODE_CONFIG_N9000_Ex), (LPDWORD)&ret, 0);
	int i = 0;
#endif

}

void CConfigDlg::OnBnClickedButtonConfigGet()
{
	CString path;
	GetDlgItemText(IDC_EDIT_CONFIG_PATH, path);
	if (path.IsEmpty())
	{
		AfxMessageBox(STR_SELECT_PATH);
		return ;
	}
	
	unsigned long test = 0x0;

	CButton *button;
	for (int i = IDC_CHECK_CONFIG_SYSBASIC; i <= IDC_CHECK_CONFIG_COLOR; i++)
	{
		button = (CButton *)GetDlgItem(i);
		if(button->GetCheck())
		{
			test |= 0x1 << (i - IDC_CHECK_CONFIG_SYSBASIC);
		}
	}
	CConfigManager cfgMgr;
	LoadConfig(&cfgMgr, test);

	cfgMgr.Export(path.GetBuffer());
	ShellExecute(NULL, _T("open"), path + _T("\\main.html"), NULL,NULL,SW_SHOWNORMAL);
}

void CConfigDlg::OnBnClickedButtonConfigPathBrowse()
{
	CFolderDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetFolderPath();
		SetDlgItemText(IDC_EDIT_CONFIG_PATH, path);
	}

}

void CConfigDlg::setDeviceInfo( NET_SDK_DEVICEINFO *di, LONG userid )
{
	m_userID = userid; 
	m_devieInfo = di;
	m_chnnCombo.AddString(STR_ALL);
	for (int i = 0; i < m_devieInfo->videoInputNum; i++)
	{
		CString temp;
		temp.Format(_T("%d"), i + 1);
		m_chnnCombo.AddString(temp);
	}
	m_chnnCombo.SetCurSel(0);
}

void CConfigDlg::LoadConfig( CConfigManager *cfgMgr, ULONG test, bool isdefault)
{
	
	if (GETBIT(0))
	{
		LoadBasicConfig(cfgMgr, isdefault);
	}
	if (GETBIT(1))
	{
		LoadDatetimeConfig(cfgMgr, isdefault);
	}
	if (GETBIT(2))
	{
		LoadDSTConfig(cfgMgr, isdefault);
	}	
	if (GETBIT(3))
	{
		LoadLiveConfig(cfgMgr, isdefault);
	}
	if (GETBIT(4))
	{
		LoadMainCameraConfig(cfgMgr, isdefault);
	}
	if (GETBIT(5))
	{
		LoadSpotConfig(cfgMgr, isdefault);
	}
	if (GETBIT(6))
	{
		LoadAudioConfig(cfgMgr, isdefault);
	}
	if (GETBIT(7))
	{
		LoadChnnConfig(cfgMgr, isdefault);
	}
	if (GETBIT(8))
	{
		LoadOSDConfig(cfgMgr, isdefault);
	}
	if (GETBIT(9))
	{
		LoadEncodeConfig(cfgMgr, isdefault);
	}
	if (GETBIT(10))
	{
		LoadEncodeAlarmConfig(cfgMgr, isdefault);
	}
	if (GETBIT(11))
	{
		LoadEncodeNetConfig(cfgMgr, isdefault);
	}
	if (GETBIT(12))
	{
		LoadRecPlanConfig(cfgMgr, isdefault);
	}
	if (GETBIT(13))
	{
		LoadMotionPlanConfig(cfgMgr, isdefault);
	}
	if (GETBIT(14))
	{
		LoadSensorPlanConfig(cfgMgr, isdefault);
	}
	if (GETBIT(15))
	{
		LoadNetConfig(cfgMgr, isdefault);
	}
	if (GETBIT(16))
	{
		LoadDDNSConfig(cfgMgr, isdefault);
	}
	if (GETBIT(17))
	{
		LoadMailConfig(cfgMgr, isdefault);
	}
	if (GETBIT(18))
	{
		LoadUserConfig(cfgMgr, isdefault);
	}
	if (GETBIT(19))
	{
		LoadSensorConfig(cfgMgr, isdefault);
	}
	if (GETBIT(20))
	{
		LoadMotionConfig(cfgMgr, isdefault);
	}
	if (GETBIT(21))
	{
		LoadVlossConfig(cfgMgr, isdefault);
	}
	if (GETBIT(22))
	{
		LoadPTZConfig(cfgMgr, isdefault);
	}
	if(GETBIT(23))
	{
		LoadRecordConfig(cfgMgr, isdefault);
	}
	if (GETBIT(24))
	{
		LoadColorConfig(cfgMgr, isdefault);
	}
}

void CConfigDlg::LoadBasicConfig( CConfigManager *cfgMgr, bool isdefault )
{
	//n9000 里面判断N制还是P制是根据DD_BASIC_CONFIG videoFormat里的值，1表示N制，2表示P制
	unsigned long retLen = 0;
	CSafeMemFile smf;
	DD_BASIC_CONFIG bc;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_SYSTEM_BASIC, -1, &bc, sizeof(bc), &retLen, isdefault);
	DD_DEVICE_INFO di;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_DEVICE_INFO, -1, &di, sizeof(di), &retLen, isdefault);

#ifdef N9000_CONFIG
	//进行字符转换
	int  unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, di.deviceName, -1, NULL, 0);   
	wchar_t *pUnicode = new  wchar_t[unicodeLen+1]; 
	memset(pUnicode, 0, unicodeLen+1);
	MultiByteToWideChar(CP_UTF8, 0, di.deviceName, -1, (LPWSTR)pUnicode, unicodeLen);  
	delete []pUnicode;
#endif

	smf.Write(di.deviceName, DD_MAX_NAME_LEN);
	smf.WriteUInt16(di.deviceID);
	smf.WriteUInt32(bc.videoFormat);
	smf.WriteUInt32(GetBitPos(bc.videoOutResolution));
	smf.WriteUInt32(bc.VGARefresh);
	smf.WriteUInt32(bc.screensaver);
	smf.WriteUInt32(bc.deviceLanguage);
	smf.WriteUInt8(bc.passwordCheck);
	smf.WriteUInt8(bc.RecycleRecord);
	cfgMgr->Feed(0, -1, smf.GetBuffer(), smf.GetLength(), _T("9;s64i2C0C7i4i4C11bx2"), smf.GetLength(), SYSBASIC_HEADER);
}

void CConfigDlg::LoadDatetimeConfig( CConfigManager *cfgMgr, bool isdefault )
{
	unsigned long retLen = 0;
	CSafeMemFile smf;
	DD_DATE_TIME_CONFIG dtc;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_DATE_TIME, -1, &dtc, sizeof(dtc), &retLen, isdefault);
	smf.WriteUInt32(dtc.dateFormat);
	smf.WriteUInt32(dtc.timeFormat);
	smf.WriteUInt32(dtc.timeZone);
	smf.WriteUInt8(dtc.enableNTP);
	smf.WriteUInt16(dtc.ntpPort);
	smf.Write(dtc.ntpServerAddr, DD_MAX_URL_BUF_LEN);
	cfgMgr->Feed(1, -1, smf.GetBuffer(), smf.GetLength(), _T("6;C1C12C2bi2s260"), smf.GetLength(), DATETIME_HEADER);
}

void CConfigDlg::LoadDSTConfig( CConfigManager *cfgMgr, bool isdefault )
{
#ifdef N9000_CONFIG
	//N9000SDK暂不支持该功能
	return ;
#endif

	unsigned long retLen = 0;
	CSafeMemFile smf;
	DD_DAYLIGHT_INFO di;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_DAYLIGHT_INFO, -1, &di, sizeof(di), &retLen, isdefault);

	smf.WriteUInt8(di.enable);
	smf.WriteUInt32(di.offSet / 3600);
	smf.WriteUInt32(di.type);
	if (di.type)
	{
		smf.WriteUInt32(di.InMonth);
		smf.WriteUInt8(di.InMday);
		smf.WriteUInt32(di.InSecond);

		smf.WriteUInt32(di.OutMonth);
		smf.WriteUInt8(di.OutMday);
		smf.WriteUInt32(di.OutSecond);

		cfgMgr->Feed(2, -1, smf.GetBuffer(), smf.GetLength(), _T("9;bi4C3C4Bi4C4Bi4"), smf.GetLength(), DST_DATE_HEADER, 0);
	}
	else
	{
		smf.WriteUInt32(di.InMonth);
		smf.WriteUInt8(di.InWeekIndex);
		smf.WriteUInt32(di.InWday);
		smf.WriteUInt32(di.InSecond);

		smf.WriteUInt32(di.OutMonth);
		smf.WriteUInt8(di.OutWeekIndex);
		smf.WriteUInt32(di.OutWday);
		smf.WriteUInt32(di.OutSecond);

		cfgMgr->Feed(2, -1, smf.GetBuffer(), smf.GetLength(), _T("11;bi4C3C4BC5i4C4BC5i4"), smf.GetLength(), DST_WEEK_HEADER, 1);
	}
}

void CConfigDlg::LoadLiveConfig( CConfigManager *cfgMgr, bool isdefault )
{
#ifdef N9000_CONFIG
	//N9000SDK暂不支持该功能
	return;
#endif

	unsigned long retLen = 0;
	int chnn = m_chnnCombo.GetCurSel() - 1;
	DD_LIVE_DISPLAY ld;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_DISPLAY, -1, &ld, sizeof(ld), &retLen, isdefault);
	
	{
		CSafeMemFile smf;
		smf.WriteUInt8(ld.showTime);
		smf.WriteUInt8(ld.showNetwork);
		smf.WriteUInt8(ld.showHDD);
		smf.WriteUInt8(ld.showUSB);
		smf.WriteUInt8(ld.showAlarmIn);
		smf.WriteUInt8(ld.showAlarmOut);

		cfgMgr->Feed(3, -1, smf.GetBuffer(), smf.GetLength(), _T("6;bx6"), smf.GetLength(), LIVE_HEADER);
	}

	{
		CSafeMemFile smf;
		int sublen = 2;
		int len = 0;
		if (chnn >= 0)
		{
			len = 2;
			smf.WriteUInt8(ld.showCameraName[chnn]);
			smf.WriteUInt8(ld.showRecordStatus[chnn]);
		}
		else
		{
			len = 2 * m_devieInfo->videoInputNum;
			for (int i = 0; i < m_devieInfo->videoInputNum; i++)
			{
				smf.WriteUInt8(ld.showCameraName[i]);
				smf.WriteUInt8(ld.showRecordStatus[i]);
			}
		}
		
		cfgMgr->Feed(4, chnn, smf.GetBuffer(), len, _T("2;bx2"), sublen, LIVE_CHNN_HEADER);
	}	
}

void CConfigDlg::LoadMainCameraConfig( CConfigManager *cfgMgr, bool isdefault )
{
#ifdef N9000_CONFIG
	//N9000SDK暂不支持该功能
	return;
#endif

	unsigned long retLen = 0;
	int vnum = m_devieInfo->videoInputNum;
	DD_LIVE_VIDEO_GROUP *lvg = new DD_LIVE_VIDEO_GROUP[vnum];

	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_MAIN_CAMERA, -1, lvg, sizeof(DD_LIVE_VIDEO_GROUP) * vnum, &retLen, isdefault);

	int n = SplitModeToNum((DD_VIEW_SPLIT_MODE)lvg->splitMode);
	
	int gnum = ceil((double)vnum / n);
	
	
	CSafeMemFile smf;
	char text[20];
	
	int choice = -1;
	switch(lvg->splitMode)
	{
	case DD_VIEW_SPLIT_1X1:
		choice = 0;
		break;
	case DD_VIEW_SPLIT_2X2:
		choice = 1;
		break;
	case DD_VIEW_SPLIT_2X3:
		choice = 2;
		break;
	case DD_VIEW_SPLIT_3X3:
		choice = 3;
		break;
	case DD_VIEW_SPLIT_4X4:
		choice = 4;
		break;
	case DD_VIEW_SPLIT_5X5:
		choice = 5;
		break;
	case DD_VIEW_SPLIT_6X6:
		choice = 6;
		break;
	}

	char *temp = new char[n];
	for (int i = 0; i < gnum; i++)
	{
		smf.WriteUInt32(choice);
		smf.WriteUInt16(lvg[i].holdTime);
		memset(temp, -1, n);
		for (int j = 0; j < vnum; j++)
		{
			if (lvg[i].channel[j] != 0xff)
			{
				temp[lvg[i].channel[j]] = j;
			}
		}

		for (int j = 0; j < n; j++)
		{
			memset(text, 0x0, 20);
			if (temp[j] >= 0)
			{
				sprintf_s(text, 20, _T("channel %d"), temp[j] + 1);
			}
			else
			{
				sprintf_s(text, 20, _T("none"));
			}
			smf.Write(text, 20);	
		}
	}
	delete[] temp;

	int len = smf.GetLength();
	int sublen = 6 + 20 * n;
	switch (lvg->splitMode)
	{
	case DD_VIEW_SPLIT_1X1:
		cfgMgr->Feed(5, -1, smf.GetBuffer(), len, _T("3;C13i2s20"), sublen, SPLIT_HEADER_1x1, 0);
		break;
	case DD_VIEW_SPLIT_2X2:
		cfgMgr->Feed(5, -1, smf.GetBuffer(), len, _T("6;C13i2s20x4"), sublen, SPLIT_HEADER_2x2, 1);
		break;
	case DD_VIEW_SPLIT_2X3:
		cfgMgr->Feed(5, -1, smf.GetBuffer(), len, _T("8;C13i2s20x6"), sublen, SPLIT_HEADER_2x3, 2);
		break;
	case DD_VIEW_SPLIT_3X3:
		cfgMgr->Feed(5, -1, smf.GetBuffer(), len, _T("11;C13i2s20x9"), sublen, SPLIT_HEADER_3x3, 3);
		break;
	case DD_VIEW_SPLIT_4X4:
		cfgMgr->Feed(5, -1, smf.GetBuffer(), len, _T("18;C13i2s20x16"), sublen, SPLIT_HEADER_4x4, 4);
		break;
	case DD_VIEW_SPLIT_5X5:
		cfgMgr->Feed(5, -1, smf.GetBuffer(), len, _T("27;C13i2s20x25"), sublen, SPLIT_HEADER_5x5, 5);
		break;
	case DD_VIEW_SPLIT_6X6:
		cfgMgr->Feed(5, -1, smf.GetBuffer(), len, _T("38;C13i2s20x36"), sublen, SPLIT_HEADER_6x6, 6);
		break;
	}
	delete[] lvg;
}

void CConfigDlg::LoadSpotConfig( CConfigManager *cfgMgr, bool isdefault )
{
#ifdef N9000_CONFIG
	return;
#endif

	unsigned long retLen = 0;
	int vnum = m_devieInfo->videoInputNum;
	DD_LIVE_VIDEO_GROUP *lvg = new DD_LIVE_VIDEO_GROUP[vnum];

	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_SPOT_CAMERA, -1, lvg, sizeof(DD_LIVE_VIDEO_GROUP) * vnum, &retLen, isdefault);

	int n = SplitModeToNum((DD_VIEW_SPLIT_MODE)lvg->splitMode);
	
	int gnum = ceil((double)vnum / n);
	
	CSafeMemFile smf;
	char text[20];
	
	int choice = -1;
	switch(lvg->splitMode)
	{
	case DD_VIEW_SPLIT_1X1:
		choice = 0;
		break;
	case DD_VIEW_SPLIT_2X2:
		choice = 1;
		break;
	case DD_VIEW_SPLIT_2X3:
		choice = 2;
		break;
	case DD_VIEW_SPLIT_3X3:
		choice = 3;
		break;
	case DD_VIEW_SPLIT_4X4:
		choice = 4;
		break;
	case DD_VIEW_SPLIT_5X5:
		choice = 5;
		break;
	case DD_VIEW_SPLIT_6X6:
		choice = 6;
		break;
	}

	char *temp = new char[n];
	for (int i = 0; i < gnum; i++)
	{
		smf.WriteUInt32(choice);
		smf.WriteUInt16(lvg[i].holdTime);
		memset(temp, -1, n);
		for (int j = 0; j < vnum; j++)
		{
			if (lvg[i].channel[j] != 0xff)
			{
				temp[lvg[i].channel[j]] = j;
			}
		}

		for (int j = 0; j < n; j++)
		{
			memset(text, 0x0, 20);
			if (temp[j] >= 0)
			{
				sprintf_s(text, 20, _T("channel %d"), temp[j] + 1);
			}
			else
			{
				sprintf_s(text, 20, _T("none"));
			}
			smf.Write(text, 20);	
		}
	}
	delete[] temp;

	int len = smf.GetLength();
	int sublen = 6 + 20 * n;
	switch (lvg->splitMode)
	{
	case DD_VIEW_SPLIT_1X1:
		cfgMgr->Feed(6, -1, smf.GetBuffer(), len, _T("3;C13i2s20"), sublen, SPLIT_HEADER_1x1);
		break;
	case DD_VIEW_SPLIT_2X2:
		cfgMgr->Feed(6, -1, smf.GetBuffer(), len, _T("6;C13i2s20x4"), sublen, SPLIT_HEADER_2x2);
		break;
	case DD_VIEW_SPLIT_2X3:
		cfgMgr->Feed(6, -1, smf.GetBuffer(), len, _T("8;C13i2s20x6"), sublen, SPLIT_HEADER_2x3);
		break;
	case DD_VIEW_SPLIT_3X3:
		cfgMgr->Feed(6, -1, smf.GetBuffer(), len, _T("11;C13i2s20x9"), sublen, SPLIT_HEADER_3x3);
		break;
	case DD_VIEW_SPLIT_4X4:
		cfgMgr->Feed(6, -1, smf.GetBuffer(), len, _T("18;C13i2s20x16"), sublen, SPLIT_HEADER_4x4);
		break;
	case DD_VIEW_SPLIT_5X5:
		cfgMgr->Feed(6, -1, smf.GetBuffer(), len, _T("27;C13i2s20x25"), sublen, SPLIT_HEADER_5x5);
		break;
	case DD_VIEW_SPLIT_6X6:
		cfgMgr->Feed(6, -1, smf.GetBuffer(), len, _T("38;C13i2s20x36"), sublen, SPLIT_HEADER_6x6);
		break;
	}
	delete[] lvg;
}

void CConfigDlg::LoadAudioConfig( CConfigManager *cfgMgr, bool isdefault )
{
#ifdef N9000_CONFIG
	return ;
#endif

	unsigned long retLen = 0;
	DD_LIVE_AUDIO_GROUP *lag = new DD_LIVE_AUDIO_GROUP[m_devieInfo->audioInputNum];

	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_AUDIO, -1, lag, 
		sizeof(DD_LIVE_AUDIO_GROUP) * m_devieInfo->audioInputNum, &retLen, isdefault);
	int num = retLen / sizeof (DD_LIVE_AUDIO_GROUP);

	CSafeMemFile smf;
	for (int i = 0; i < num; i++)
	{
		smf.WriteUInt16(lag[i].holdTime);
		smf.WriteUInt8(lag[i].volume);
		smf.WriteUInt8(lag[i].channel + 1);
	}
	int sublen = 4;
	cfgMgr->Feed(7, -1, smf.GetBuffer(), smf.GetLength(), _T("3;i2BB"), sublen, LIVE_AUDIO_HEADER);
	delete[] lag;
}

void CConfigDlg::LoadChnnConfig( CConfigManager *cfgMgr, bool isdefault )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_CHANNEL_CONFIG *cc = new DD_CHANNEL_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_CHNN_CONFIG, chnn, cc, 
		sizeof(DD_CHANNEL_CONFIG) * m_devieInfo->videoInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_CHANNEL_CONFIG);
	char szlog[512] = {0};
	sprintf(szlog, "\r\n************** num = %d ********\r\n", retLen);
	OutputDebugStringA(szlog);
	CSafeMemFile smf;
	for (int i = 0; i < num; i++)
	{

#ifdef N9000_CONFIG
		//字符转换
		int  unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, cc[i].name, -1, NULL, 0);   
		wchar_t *pUnicode = new  wchar_t[unicodeLen+1]; 
		memset(pUnicode, 0, unicodeLen+1);
		MultiByteToWideChar(CP_UTF8, 0, cc[i].name, -1, (LPWSTR)pUnicode, unicodeLen);  
		delete []pUnicode;
#endif
		smf.WriteUInt8(cc[i].hide);
		smf.Write(cc[i].name, 132);
	}

	int sublen = 133;
	cfgMgr->Feed(8, chnn, smf.GetBuffer(), smf.GetLength(), _T("2;bs132"), sublen, CHNN_HEADER);
	delete []cc;
}

void CConfigDlg::LoadOSDConfig( CConfigManager *cfgMgr, bool isdefault )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_VIDEO_OSD_CONFIG *voc = new DD_VIDEO_OSD_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_VIDEO_OSD, chnn, voc, 
		sizeof(DD_VIDEO_OSD_CONFIG) * m_devieInfo->videoInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_VIDEO_OSD_CONFIG);

	//n9000传上来的值是对的，但这里转换显示的时候有些问题
	CSafeMemFile smf;
	for (int i = 0; i < num; i++)
	{
		smf.WriteUInt8(voc[i].enableCameraName);
		smf.WriteUInt8(voc[i].enableTimeStamp);
		smf.WriteUInt8(voc[i].enableTimeStampWithWeek);
		smf.WriteUInt8(voc[i].enableDefineText);

		smf.WriteUInt8(voc[i].cameraName.x);
		smf.WriteUInt8(voc[i].cameraName.y);
		
		smf.WriteUInt8(voc[i].timeStamp.x);
		smf.WriteUInt8(voc[i].timeStamp.y);
		
		smf.WriteUInt8(voc[i].defineText.x);
		smf.WriteUInt8(voc[i].defineText.y);

		smf.Write(voc[i].text, 132);
	}

	int sublen = 142;
	cfgMgr->Feed(9, chnn, smf.GetBuffer(), smf.GetLength(), _T("8;bx4px3s132"), sublen, OSD_HEADER);
	delete[] voc;
}

void CConfigDlg::LoadEncodeConfig( CConfigManager *cfgMgr, bool isdefault )
{
#ifdef N9000_CONFIG
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_ENCODE_CONFIG_N9000 *ec = new DD_ENCODE_CONFIG_N9000[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_SCHEDULE, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG_N9000) * m_devieInfo->videoInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_ENCODE_CONFIG_N9000);
#else
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_ENCODE_CONFIG *ec = new DD_ENCODE_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_SCHEDULE, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG) * m_devieInfo->videoInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_ENCODE_CONFIG);
#endif

	CSafeMemFile smf;
	for (int i = 0; i < num; i++)
	{
		smf.WriteUInt32(GetBitPos(ec[i].resolution));
		smf.WriteUInt16(ec[i].rate);
		smf.WriteUInt32(ec[i].encodeType);
		smf.WriteUInt32(ec[i].quality);
		smf.WriteUInt32(ec[i].maxBitrate);
		smf.WriteUInt32(ec[i].minBitrate);
	}

	int sublen = 22;
	cfgMgr->Feed(10, chnn, smf.GetBuffer(), smf.GetLength(), _T("6;C9i2C8C10i4i4"), sublen, ENCODE_HEADER);
	delete[] ec;
}

void CConfigDlg::LoadEncodeNetConfig( CConfigManager *cfgMgr, bool isdefault )
{
#ifdef N9000_CONFIG
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_ENCODE_CONFIG_N9000 *ec = new DD_ENCODE_CONFIG_N9000[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_NETWORK, chnn, ec, sizeof(DD_ENCODE_CONFIG_N9000) * m_devieInfo->videoInputNum, &retLen, isdefault);
	int num = retLen / sizeof(DD_ENCODE_CONFIG_N9000);
#else
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_ENCODE_CONFIG *ec = new DD_ENCODE_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_NETWORK, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG) * m_devieInfo->videoInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_ENCODE_CONFIG);
#endif

	CSafeMemFile smf;
	for (int i = 0; i < num; i++)
	{
		smf.WriteUInt32(GetBitPos(ec[i].resolution));
		smf.WriteUInt16(ec[i].rate);
		smf.WriteUInt32(ec[i].encodeType);
		smf.WriteUInt32(ec[i].quality);
		smf.WriteUInt32(ec[i].maxBitrate);
		smf.WriteUInt32(ec[i].minBitrate);
	}

	int sublen = 22;
	cfgMgr->Feed(12, chnn, smf.GetBuffer(), smf.GetLength(), _T("6;C9i2C8C10i4i4"), sublen, ENCODE_HEADER);
	delete[] ec;
}

void CConfigDlg::LoadRecPlanConfig( CConfigManager *cfgMgr, bool isdefault )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;

	DD_WEEK_SCHEDULE* pWeekSchedule = new DD_WEEK_SCHEDULE[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_RECORD_SCHEDULE_SCHEDULE, chnn, pWeekSchedule, 
		sizeof(DD_WEEK_SCHEDULE) * m_devieInfo->videoInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_WEEK_SCHEDULE);

	//for(int i = 0; i < num; i++)
	//{
	//	for(int week=0; week < 7; week++)
	//	{
	//		TRACE("week %d:\n", week);
	//		for(int hour = 0; hour < 24; hour++)
	//		{
	//			TRACE("hour %d: ", hour);
	//			for(int bit = 0; bit < 60; bit++)
	//			{
	//				if((m_pWeekSchedule[i].week[week].hour[hour] >> bit) & 0x01)
	//				{
	//					TRACE("1");
	//				}
	//				else
	//				{
	//					TRACE("0");
	//				}
	//			}
	//			TRACE("\n");
	//		}
	//	}
	//}

	delete [] pWeekSchedule;
}

void CConfigDlg::LoadSensorPlanConfig( CConfigManager *cfgMgr, bool isdefault )
{

}

void CConfigDlg::LoadNetConfig( CConfigManager *cfgMgr, bool isdefault )
{
	unsigned long retLen = 0;
	DD_NETWORK_IP_CONFIG nic;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_IP, -1, &nic, 
		sizeof(DD_NETWORK_IP_CONFIG), &retLen, isdefault);
	DD_NETWORK_ADVANCE_CONFIG nac;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_ADVANCE, -1, &nac, 
		sizeof(DD_NETWORK_ADVANCE_CONFIG), &retLen, isdefault);
	{
		CSafeMemFile smf;

		smf.WriteUInt8(nic.useDHCP);
		smf.WriteUInt32(nic.IP);
		smf.WriteUInt32(nic.subnetMask);
		smf.WriteUInt32(nic.gateway);
		smf.WriteUInt32(nic.preferredDNS);
		smf.WriteUInt32(nic.alternateDNS);
		smf.WriteUInt8(nic.useDHCP);
		smf.Write(nic.account, 132);
		smf.Write(nic.password, 132);
		cfgMgr->Feed(17, -1, smf.GetBuffer(), smf.GetLength(), _T("9;bPx5bs132x2"), smf.GetLength(), NET_BASIC_HEADER);
	}

	{
		CSafeMemFile smf;

		smf.WriteUInt16(nac.httpPort);
		smf.WriteUInt16(nac.datePort);
		smf.WriteUInt16(nac.messagePort);
		smf.WriteUInt16(nac.alarmPort);
		smf.WriteUInt16(nac.OnlineUserNum);
		smf.WriteUInt32(nac.multiCastIP);
		smf.WriteUInt32(nac.mtuByteNum);
		cfgMgr->Feed(18, -1, smf.GetBuffer(), smf.GetLength(), _T("7;i2x5Pi4"), smf.GetLength(), NET_ADVANCE_HEADER);
	}
}

void CConfigDlg::LoadDDNSConfig( CConfigManager *cfgMgr, bool isdefault )
{
	unsigned long retLen = 0;
	DD_DDNS_CONFIG dc;

	/*N9000中，DDNS服务器类型对应关系为：
		typedef enum _DDNS_SERVER_TYPE
		{
			DDNS_NONE =0,
			DDNS_88IP,		//www.88ip.net     
			DDNS_DNS2P,		//www.dns2p.net
			DDNS_MEIBU,     //www.meibu.com
			DDNS_DYNDNS,    //www.dyndns.com
			DDNS_NOIPDNS,   //www.no-ip.com
			DDNS_DVRDYDNS,  //www.dvrdydns.com
			DDNS_MINTDNS,	//MintDNS 
			DDNS_3322,		//www.3322.org  
			DDNS_123DDNS,	//www.123ddns.com
			DDNS_EASTERNDNS,//www.easterndns.com,
			DDNS_MYQSEE,	//www.myq-see.com 
			DDNS_DDNSBR,	//www.ddns.com.br
			DDNS_TECVOZDDNS,//www.tecvozddns.com.br
			DDNS_HIVIEW,	//www.hiview.org
			DDNS_AUTODDNS,	//www.autoddns.com
			DDNS_END,
		}DDNS_SERVER_TYPE;
	*/

	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_DDNS, -1, &dc, 
		sizeof(DD_DDNS_CONFIG), &retLen, isdefault);

	{
		CSafeMemFile smf;

		smf.WriteUInt8(dc.enable);
		smf.WriteUInt16(dc.interval / 60);
		smf.WriteUInt32(dc.useDDNSServer);
		smf.WriteUInt8(dc.userHostDomain);
		smf.Write(dc.userName, 132);
		smf.Write(dc.password, 132);
		smf.Write(dc.hostDomain, 132);
		
		cfgMgr->Feed(19, -1, smf.GetBuffer(), smf.GetLength(), _T("7;bi2C14bs132x3"), smf.GetLength(), DDNS_HEADER);
	}
}

void CConfigDlg::LoadMailConfig( CConfigManager *cfgMgr, bool isdefault )
{
	unsigned long retLen = 0;
	DD_SMTP_CONFIG sc;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_SMTP, -1, &sc, 
		sizeof(DD_SMTP_CONFIG), &retLen, isdefault);

	{
		CSafeMemFile smf;

		smf.WriteUInt16(sc.port);
		smf.WriteUInt8(sc.enableSSL);
		smf.Write(sc.server, 260);
		smf.Write(sc.sendAddress, 260);
		smf.Write(sc.password, 132);
		smf.Write(sc.receiveAddress[0], 260);
		smf.Write(sc.receiveAddress[1], 260);
		smf.Write(sc.receiveAddress[2], 260);

		cfgMgr->Feed(20, -1, smf.GetBuffer(), smf.GetLength(), _T("8;i2bs260x2s132s260x3"), smf.GetLength(), SMTP_HEADER);
	}
}

void CConfigDlg::LoadUserConfig( CConfigManager *cfgMgr, bool isdefault )
{
	unsigned long retLen = 0;
	DD_ACCOUNT_CONFIG *users = new DD_ACCOUNT_CONFIG[DD_MAX_ACCOUNT_NUM];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ACCOUNT, -1, users, 
		sizeof(DD_MOTION_CONFIG) * DD_MAX_ACCOUNT_NUM, &retLen, isdefault);
	int num = retLen / sizeof(DD_ACCOUNT_CONFIG);
	
	CSafeMemFile smf;
	int i = 0;
	while (/*_tcslen(users[i].name)*/ i < num)
	{
		smf.WriteUInt8(users[i].enable);
		smf.WriteUInt8(users[i].bindMAC);
		smf.WriteUInt32(users[i].group);
		smf.Write(users[i].MAC, 8);
		smf.Write(users[i].name, 132);
		smf.Write(users[i].password, 132);

		smf.WriteUInt8(users[i].logSearch);
		smf.WriteUInt8(users[i].systemSetup);
		smf.WriteUInt8(users[i].fileManagement);
		smf.WriteUInt8(users[i].diskManagement);
		smf.WriteUInt8(users[i].remoteLogin);
		smf.WriteUInt8(users[i].twoWayAudio);
		smf.WriteUInt8(users[i].systemMaintain);
		smf.WriteUInt8(users[i].OnlineUserManagement);
		smf.WriteUInt8(users[i].shutdown);
		smf.WriteUInt8(users[i].alarmOutCtrl);
		smf.WriteUInt8(users[i].netAlarm);
		smf.WriteUInt8(users[i].netSerialCtrl);

		i++;
	}
	cfgMgr->Feed(21, -1, smf.GetBuffer(), smf.GetLength(), _T("18;bbC17Ms132x2bx12"), smf.GetLength(), USER_HEADER);

	delete[] users;
}

void CConfigDlg::LoadMotionConfig( CConfigManager *cfgMgr, bool isdefault )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;

	DD_MOTION_CONFIG *mc = new DD_MOTION_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_MOTION_SETUP, chnn, mc, 
		sizeof(DD_MOTION_CONFIG) * m_devieInfo->videoInputNum, &retLen, isdefault);
	
	int num = retLen / sizeof(DD_MOTION_CONFIG);

	DD_TRIGGER_ALARM_OUT *tao = new DD_TRIGGER_ALARM_OUT[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_MOTION_ALARM_OUT, chnn, tao, 
		sizeof(DD_TRIGGER_ALARM_OUT) * m_devieInfo->videoInputNum, &retLen, isdefault);
	
	{
		CSafeMemFile smf;
		for (int i = 0; i < num; i++)
		{
			smf.WriteUInt8(mc[i].enable);
			smf.WriteUInt16(mc[i].holdTime);
			smf.WriteUInt32(mc[i].area.sensitivity);
			smf.WriteUInt16(mc[i].area.widthNum);
			smf.WriteUInt16(mc[i].area.heightNum);
		}

		int sublen = 11;
		cfgMgr->Feed(24, chnn, smf.GetBuffer(), smf.GetLength(), _T("5;bi2i4i2i2"), sublen, MOTION_SETUP_HEADER);
	}
	
	{
		CSafeMemFile smf;
		for (int i = 0; i < num; i++)
		{
			smf.WriteUInt8(tao[i].toBuzzer);
			smf.WriteUInt8(tao[i].ShowFullScreen);
			smf.WriteUInt8(tao[i].sendEmail);
			smf.WriteUInt8(tao[i].toUploadToAlarmCentre);
			smf.WriteUInt32(tao[i].toAlarmOut);
		}

		int sublen = 8;
		cfgMgr->Feed(25, chnn, smf.GetBuffer(), smf.GetLength(), _T("5;bBbbh"), sublen, ALARM_TRIGGER_HEADER);
	}

	delete[] mc;
	delete[] tao;
}

void CConfigDlg::LoadSensorConfig( CConfigManager *cfgMgr, bool isdefault )
{
	unsigned long retLen = 0;
    int chnn = m_chnnCombo.GetCurSel() - 1;
	DD_SENSOR_CONFIG *sc = new DD_SENSOR_CONFIG[m_devieInfo->sensorInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_SENSOR_SETUP, chnn, sc, 
		sizeof(DD_SENSOR_CONFIG) * m_devieInfo->sensorInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_SENSOR_CONFIG);

	DD_TRIGGER_ALARM_OUT *tao = new DD_TRIGGER_ALARM_OUT[m_devieInfo->sensorInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_SENSOR_ALARM_OUT, chnn, tao, 
		sizeof(DD_TRIGGER_ALARM_OUT) * m_devieInfo->sensorInputNum, &retLen, isdefault);

	DD_TRIGGER_RECORD *triggerRec = new DD_TRIGGER_RECORD[m_devieInfo->sensorInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_SENSOR_TO_RECORD, chnn, triggerRec, 
		sizeof(DD_TRIGGER_RECORD) * m_devieInfo->sensorInputNum, &retLen, isdefault);

#ifndef N9000_CONFIG
		{
			CSafeMemFile smf;
			for (int i = 0; i < num; i++)
			{
				smf.WriteUInt8(sc[i].enable);
				smf.WriteUInt32(sc[i].bNO);
				smf.WriteUInt16(sc[i].holdTime);
				smf.Write(sc[i].name, 132);
			}

			int sublen = 139;
			cfgMgr->Feed(22, chnn, smf.GetBuffer(), smf.GetLength(), _T("4;bC15i2s132"), sublen, SENSOR_SETUP_HEADER);
		}

		{
			CSafeMemFile smf;
			for (int i = 0; i < num; i++)
			{
				smf.WriteUInt8(tao[i].toBuzzer);
				smf.WriteUInt8(tao[i].ShowFullScreen);
				smf.WriteUInt8(tao[i].sendEmail);
				smf.WriteUInt8(tao[i].toUploadToAlarmCentre);
				smf.WriteUInt32(tao[i].toAlarmOut);
			}

			int sublen = 8;
			cfgMgr->Feed(23, chnn, smf.GetBuffer(), smf.GetLength(), _T("5;bBbbh"), sublen, ALARM_TRIGGER_HEADER);
		}
#endif

	delete[] sc;
	delete[] tao;
}

void CConfigDlg::LoadVlossConfig( CConfigManager *cfgMgr, bool isdefault )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;

	DD_TRIGGER_ALARM_OUT *tao = new DD_TRIGGER_ALARM_OUT[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_VLOSS_ALARM_OUT, chnn, tao, 
		sizeof(DD_TRIGGER_ALARM_OUT) * m_devieInfo->videoInputNum, &retLen, isdefault);
	
	int num = retLen / sizeof(DD_TRIGGER_ALARM_OUT);

	{
		CSafeMemFile smf;
		for (int i = 0; i < num; i++)
		{
			smf.WriteUInt8(tao[i].toBuzzer);
			smf.WriteUInt8(tao[i].ShowFullScreen);
			smf.WriteUInt8(tao[i].sendEmail);
			smf.WriteUInt8(tao[i].toUploadToAlarmCentre);
			smf.WriteUInt32(tao[i].toAlarmOut);
		}

		if (num > 0)
		{
			int sublen = 8;
			cfgMgr->Feed(26, chnn, smf.GetBuffer(), smf.GetLength(), _T("5;bBbbh"), sublen, ALARM_TRIGGER_HEADER);
		}
	}

	delete[] tao;
}

void CConfigDlg::LoadPTZConfig( CConfigManager *cfgMgr, bool isdefault )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;

	DD_PTZ_CONFIG *pc = new DD_PTZ_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_PTZ_SETUP/*DD_CONFIG_ITEM_PTZ_PRESET*/, chnn, pc, 
		sizeof(DD_PTZ_CONFIG) * m_devieInfo->videoInputNum, &retLen, isdefault);
	int num = retLen / sizeof(DD_PTZ_CONFIG);

	DD_BUZZER_CONFIG bc;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_BUZZER_SETUP, -1, &bc, 
		sizeof(DD_BUZZER_CONFIG), &retLen, isdefault);

	

	{
		CSafeMemFile smf;
		for (int i = 0; i < num; i++)
		{
			smf.WriteUInt8(pc[i].enable);
			smf.WriteUInt8(pc[i].address);
			smf.WriteUInt32(pc[i].protocol);
			smf.WriteUInt32(pc[i].serial.baudRate);
			smf.WriteUInt32(pc[i].serial.dataBit);
			smf.WriteUInt32(pc[i].serial.stopBit);
			smf.WriteUInt32(pc[i].serial.parity);
			smf.WriteUInt32(pc[i].serial.dataFlowControl);
		}

		int sublen = 26;
		cfgMgr->Feed(27, chnn, smf.GetBuffer(), smf.GetLength(), _T("8;bBC16i4x5"), sublen, PTZ_SETUP_HEADER);
	}

	{
		CSafeMemFile smf;
		smf.WriteUInt8(bc.enable);
		smf.WriteUInt16(bc.holdTime);
		int sublen = 3;
		cfgMgr->Feed(28, -1, smf.GetBuffer(), smf.GetLength(), _T("2;bi2"), sublen, BUZZER_HEADER);
	}

	delete[] pc;
}

void CConfigDlg::LoadMotionPlanConfig( CConfigManager *cfgMgr, bool isdefault )
{

}

void CConfigDlg::LoadEncodeAlarmConfig( CConfigManager *cfgMgr, bool isdefault )
{
#ifdef N9000_CONFIG
	//分辨率参考 DD_VGA_RESOLUTION
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_ENCODE_CONFIG_N9000 *ec = new DD_ENCODE_CONFIG_N9000[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_ALARM, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG_N9000) * m_devieInfo->videoInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_ENCODE_CONFIG_N9000);
#else
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_ENCODE_CONFIG *ec = new DD_ENCODE_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_ALARM, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG) * m_devieInfo->videoInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_ENCODE_CONFIG);
#endif


	CSafeMemFile smf;
	for (int i = 0; i < num; i++)
	{
		smf.WriteUInt32(GetBitPos(ec[i].resolution));
		smf.WriteUInt16(ec[i].rate);
		smf.WriteUInt32(ec[i].encodeType);
		smf.WriteUInt32(ec[i].quality);
		smf.WriteUInt32(ec[i].maxBitrate);
		smf.WriteUInt32(ec[i].minBitrate);
	}

	int sublen = 22;
	cfgMgr->Feed(11, chnn, smf.GetBuffer(), smf.GetLength(), _T("6;C9i2C8C10i4i4"), sublen, ENCODE_HEADER);
	delete[] ec;
}

void CConfigDlg::LoadRecordConfig( CConfigManager *cfgMgr, bool isdefault )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_RECORD_CONFIG *rc = new DD_RECORD_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_RECORD_SETUP, chnn, rc, 
		sizeof(DD_RECORD_CONFIG) * m_devieInfo->videoInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_RECORD_CONFIG);

	CSafeMemFile smf;
	for (int i = 0; i < num; i++)
	{
		smf.WriteUInt8(rc[i].bOnlyVideo);
		smf.WriteUInt8(rc[i].bWithAudio);
		smf.WriteUInt8(rc[i].bindAudioChannel);
		smf.WriteUInt8(rc[i].bRedundancy);
		smf.WriteUInt16(rc[i].preAlarmTime);
		smf.WriteUInt16(rc[i].postAlarmTime);
		smf.WriteUInt16(rc[i].expired);
	}

	int sublen = 10;
	cfgMgr->Feed(13, chnn, smf.GetBuffer(), smf.GetLength(), _T("7;bbBbi2x3"), sublen, RECORD_HEADER);
	delete[] rc;
}

void CConfigDlg::OnBnClickedButtonSelectAll()
{
	CButton *button;
	for (int i = IDC_CHECK_CONFIG_SYSBASIC; i <= IDC_CHECK_CONFIG_COLOR; i++)
	{
		button = (CButton *)GetDlgItem(i);
		button->SetCheck(true);
	}
}

void CConfigDlg::OnBnClickedButtonSelectReverse()
{
	CButton *button;
	for (int i = IDC_CHECK_CONFIG_SYSBASIC; i <= IDC_CHECK_CONFIG_COLOR; i++)
	{
		button = (CButton *)GetDlgItem(i);
		button->SetCheck(!button->GetCheck());
	}
}

void CConfigDlg::OnBnClickedButtonConfigDefault()
{
	CString path;
	GetDlgItemText(IDC_EDIT_CONFIG_PATH, path);
	if (path.IsEmpty())
	{
		AfxMessageBox(STR_SELECT_PATH);
		return ;
	}

	unsigned long test = 0x0;

	CButton *button;
	for (int i = IDC_CHECK_CONFIG_SYSBASIC; i <= IDC_CHECK_CONFIG_RECORD; i++)
	{
		button = (CButton *)GetDlgItem(i);
		if(button->GetCheck())
		{
			test |= 0x1 << (i - IDC_CHECK_CONFIG_SYSBASIC);
		}
	}
	CConfigManager cfgMgr;
	LoadConfig(&cfgMgr, test, TRUE);

	cfgMgr.Export(path.GetBuffer());
	ShellExecute(NULL, _T("open"), path + _T("\\main.html"), NULL,NULL,SW_SHOWNORMAL);
}

void CConfigDlg::OnBnClickedButtonConfigSave()
{
	CString path;
	GetDlgItemText(IDC_EDIT_CONFIG_PATH, path);
	if (path.IsEmpty())
	{
		AfxMessageBox(STR_SELECT_PATH);
		return ;
	}
	
	unsigned long test = 0x0;

	CButton *button;
	for (int i = IDC_CHECK_CONFIG_SYSBASIC; i <= IDC_CHECK_CONFIG_COLOR; i++)
	{
		button = (CButton *)GetDlgItem(i);
		if(button->GetCheck())
		{
			test |= 0x1 << (i - IDC_CHECK_CONFIG_SYSBASIC);
		}
	}
	CConfigManager cfgMgr;
	RegConfig(&cfgMgr, test);
	cfgMgr.Import(path.GetBuffer());
	if (SaveConfig(&cfgMgr, test))
	{
		AfxMessageBox(_T("OK"), MB_ICONINFORMATION);
	}
	else
	{
		AfxMessageBox(STR_FAILED);
	}
}

void CConfigDlg::RegConfig( CConfigManager *cfgMgr, ULONG test )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	if (GETBIT(0))
	{
		cfgMgr->Feed(0, -1, NULL, 0, _T("9;s64i2C0C7i4i4C11bx2"), 0, SYSBASIC_HEADER);
	}
	if (GETBIT(1))
	{
		cfgMgr->Feed(1, -1, NULL, 0, _T("6;C1C12C2bi2s260"), 0, DATETIME_HEADER);
	}
	if (GETBIT(2))
	{
		cfgMgr->Feed(2, -1, NULL, 0, _T("6;C1C12C2bi2s260"), 0, DST_DATE_HEADER);
		cfgMgr->Feed(2, -1, NULL, 0, _T("11;bi4C3C4BC5i4C4BC5i4"), 0, DST_WEEK_HEADER);
	}	
	if (GETBIT(3))
	{
		cfgMgr->Feed(3, -1, NULL, 0, _T("6;bx6"), 0, LIVE_HEADER);
		cfgMgr->Feed(4, chnn, NULL, 0, _T("2;bx2"), 0, LIVE_CHNN_HEADER);
	}
	if (GETBIT(4))
	{
		cfgMgr->Feed(5, -1, NULL, 0, _T("3;C13i2s20"), 0, SPLIT_HEADER_1x1);//统一配置有两种数据格式，全部按顺序压进去，用type区分
		cfgMgr->Feed(5, -1, NULL, 0, _T("6;C13i2s20x4"), 0, SPLIT_HEADER_2x2);
		cfgMgr->Feed(5, -1, NULL, 0, _T("8;C13i2s20x6"), 0, SPLIT_HEADER_2x3);
		cfgMgr->Feed(5, -1, NULL, 0, _T("11;C13i2s20x9"), 0, SPLIT_HEADER_3x3);
		cfgMgr->Feed(5, -1, NULL, 0, _T("18;C13i2s20x16"), 0, SPLIT_HEADER_4x4);
		cfgMgr->Feed(5, -1, NULL, 0, _T("27;C13i2s20x25"), 0, SPLIT_HEADER_5x5);
		cfgMgr->Feed(5, -1, NULL, 0, _T("38;C13i2s20x36"), 0, SPLIT_HEADER_6x6);
	}
	if (GETBIT(5))
	{
		cfgMgr->Feed(6, -1, NULL, 0, _T("3;C13i2s20"), 0, SPLIT_HEADER_1x1);
	}
	if (GETBIT(6))
	{
		cfgMgr->Feed(7, -1, NULL, 0, _T("3;i2BB"), 0, LIVE_AUDIO_HEADER);
	}
	if (GETBIT(7))
	{
		cfgMgr->Feed(8, chnn, NULL, 0, _T("2;bs132"), 0, CHNN_HEADER);
	}
	if (GETBIT(8))
	{
		cfgMgr->Feed(9, chnn, NULL, 0, _T("8;bx4px3s132"), 0, OSD_HEADER);
	}
	if (GETBIT(9))
	{
		cfgMgr->Feed(10, chnn, NULL, 0, _T("6;C9i2C8C10i4i4"), 0, ENCODE_HEADER);
	}
	if (GETBIT(10))
	{
		cfgMgr->Feed(11, chnn, NULL, 0, _T("6;C9i2C8C10i4i4"), 0, ENCODE_HEADER);
	}
	if (GETBIT(11))
	{
		cfgMgr->Feed(12, chnn, NULL, 0, _T("6;C9i2C8C10i4i4"), 0, ENCODE_HEADER);
	}
	if (GETBIT(12))
	{
	}
	if (GETBIT(13))
	{
	}
	if (GETBIT(14))
	{
	}
	if (GETBIT(15))
	{
		cfgMgr->Feed(17, -1, NULL, 0, _T("9;bPx5bs132x2"), 0, NET_BASIC_HEADER);
		cfgMgr->Feed(18, -1, NULL, 0, _T("7;i2x5Pi4"), 0, NET_ADVANCE_HEADER);
	}
	if (GETBIT(16))
	{
		cfgMgr->Feed(19, -1, NULL, 0, _T("7;bi2C14bs132x3"), 0, DDNS_HEADER);
	}
	if (GETBIT(17))
	{
		cfgMgr->Feed(20, -1, NULL, 0, _T("8;i2bs260x2s132s260x3"), 0, SMTP_HEADER);
	}
	if (GETBIT(18))
	{
		cfgMgr->Feed(21, -1, NULL, 0, _T("18;bbC17Ms132x2bx12"), 0, USER_HEADER);
	}
	if (GETBIT(19))
	{
		cfgMgr->Feed(22, -1, NULL, 0, _T("4;bC15i2s132"), 0, SENSOR_SETUP_HEADER);
		cfgMgr->Feed(23, -1, NULL, 0, _T("5;bBbbh"), 0, ALARM_TRIGGER_HEADER);
	}
	if (GETBIT(20))
	{
		cfgMgr->Feed(24, chnn, NULL, 0, _T("5;bi2i4i2i2"), 0, MOTION_SETUP_HEADER);
		cfgMgr->Feed(25, chnn, NULL, 0, _T("5;bBbbh"), 0, ALARM_TRIGGER_HEADER);
	}
	if (GETBIT(21))
	{
		cfgMgr->Feed(26, chnn, NULL, 0, _T("5;bBbbh"), 0, ALARM_TRIGGER_HEADER);
	}
	if (GETBIT(22))
	{
		cfgMgr->Feed(27, chnn, NULL, 0, _T("8;bBC16i4x5"), 0, PTZ_SETUP_HEADER);
		cfgMgr->Feed(28, -1, NULL, 0, _T("2;bi2"), 0, BUZZER_HEADER);
	}
	if(GETBIT(23))
	{
		cfgMgr->Feed(13, chnn, NULL, 0, _T("7;bbBbi2x3"), 0, RECORD_HEADER);
	}
	if (GETBIT(24))
	{
		cfgMgr->Feed(29, chnn, NULL, 0, _T("4;Bx4"), 0, COLOR_HEADER);
	}
}

BOOL CConfigDlg::SaveConfig( CConfigManager *cfgMgr, ULONG test )
{
	if (GETBIT(0))
	{
		if (AfxMessageBox(STR_EDIT_SYSTEM_CONFIG,
			MB_YESNO) == IDYES)
		{
			SaveBasicConfig(cfgMgr);
		}
		return TRUE;
	}
	if (GETBIT(1))
	{
		SaveDatetimeConfig(cfgMgr);
	}
	if (GETBIT(2))
	{
		SaveDSTConfig(cfgMgr);
	}	
	if (GETBIT(3))
	{
		SaveLiveConfig(cfgMgr);
	}
	if (GETBIT(4))
	{
		SaveMainCameraConfig(cfgMgr);
	}
	if (GETBIT(5))
	{
		SaveSpotConfig(cfgMgr);
	}
	if (GETBIT(6))
	{
		SaveAudioConfig(cfgMgr);
	}
	if (GETBIT(7))
	{
		SaveChnnConfig(cfgMgr);
	}
	if (GETBIT(8))
	{
		SaveOSDConfig(cfgMgr);
	}
	if (GETBIT(9))
	{
		SaveEncodeConfig(cfgMgr);
	}
	if (GETBIT(10))
	{
		SaveEncodeAlarmConfig(cfgMgr);
	}
	if (GETBIT(11))
	{
		SaveEncodeNetConfig(cfgMgr);
	}
	if (GETBIT(12))
	{
		SaveRecPlanConfig(cfgMgr);
	}
	if (GETBIT(13))
	{
		SaveMotionPlanConfig(cfgMgr);
	}
	if (GETBIT(14))
	{
		SaveSensorPlanConfig(cfgMgr);
	}
	if (GETBIT(15))
	{
		SaveNetConfig(cfgMgr);
	}
	if (GETBIT(16))
	{
		SaveDDNSConfig(cfgMgr);
	}
	if (GETBIT(17))
	{
		SaveMailConfig(cfgMgr);
	}
	if (GETBIT(18))
	{
		SaveUserConfig(cfgMgr);
	}
	if (GETBIT(19))
	{
		SaveSensorConfig(cfgMgr);
	}
	if (GETBIT(20))
	{
		SaveMotionConfig(cfgMgr);
	}
	if (GETBIT(21))
	{
		SaveVlossConfig(cfgMgr);
	}
	if (GETBIT(22))
	{
		SavePTZConfig(cfgMgr);
	}
	if(GETBIT(23))
	{
		SaveRecordConfig(cfgMgr);
	}
	if (GETBIT(24))
	{
		SaveColorConfig(cfgMgr);
	}
	return TRUE;
}

void CConfigDlg::SaveBasicConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	
	DD_BASIC_CONFIG bc;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_SYSTEM_BASIC, -1, &bc, sizeof(bc), &retLen, FALSE);
	DD_DEVICE_INFO di;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_DEVICE_INFO, -1, &di, sizeof(di), &retLen, FALSE);
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(0, NULL, len, sublen, ch, type);//buffer 是 NULL的话 会返回数据长度
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(0, buff, len, sublen, ch, type);
	
	CSafeMemFile smf(buff, len);
	
	smf.Read(di.deviceName, DD_MAX_NAME_LEN);
	di.deviceID = smf.ReadUInt16();
	bc.videoFormat = smf.ReadUInt32();
	bc.videoOutResolution = 0;
	SETBIT(bc.videoOutResolution, smf.ReadUInt32());

	bc.VGARefresh = smf.ReadUInt32();
	bc.screensaver = smf.ReadUInt32();
	bc.deviceLanguage = smf.ReadUInt32();
	bc.passwordCheck = smf.ReadUInt8();
	bc.RecycleRecord = smf.ReadUInt8();
	
#ifdef N9000_CONFIG
	//进行字符转换
	int  unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, di.deviceName, -1, NULL, 0);   
	wchar_t *pUnicode = new  wchar_t[unicodeLen+1]; 
	memset(pUnicode, 0, unicodeLen+1);
	int dwMinSize;
	MultiByteToWideChar(CP_ACP, 0, di.deviceName, -1, (LPWSTR)pUnicode, unicodeLen);  

	memset(di.deviceName, 0, DD_MAX_NAME_LEN);
	dwMinSize = WideCharToMultiByte(CP_UTF8,NULL,pUnicode,-1,NULL,0,NULL,FALSE);
	WideCharToMultiByte(CP_UTF8,NULL,pUnicode,-1,di.deviceName,dwMinSize,NULL,FALSE);

	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_SYSTEM_BASIC, -1, &bc, sizeof(bc));
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_DEVICE_INFO, -1, &di, sizeof(di));
	delete[] pUnicode;
#else
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_SYSTEM_BASIC, -1, &bc, sizeof(bc));
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_DEVICE_INFO, -1, &di, sizeof(di));
#endif
	
	delete[] buff;
}

void CConfigDlg::SaveDatetimeConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	DD_DATE_TIME_CONFIG dtc;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_DATE_TIME, -1, &dtc, sizeof(dtc), &retLen, FALSE);

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(1, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(1, buff, len, sublen, ch, type);

	CSafeMemFile smf(buff, len);
	dtc.dateFormat = smf.ReadUInt32();
	dtc.timeFormat = smf.ReadUInt32();
	dtc.timeZone = smf.ReadUInt32();
	dtc.enableNTP = smf.ReadUInt8();
	dtc.ntpPort = smf.ReadUInt16();
	smf.Read(dtc.ntpServerAddr, DD_MAX_URL_BUF_LEN);
	
	BOOL ret = NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_DATE_TIME, -1, &dtc, sizeof(dtc));
	delete[] buff;
}

void CConfigDlg::SaveDSTConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;

	DD_DAYLIGHT_INFO di;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_DAYLIGHT_INFO, -1, &di, sizeof(di), &retLen, FALSE);
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(2, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(2, buff, len, sublen, ch, type);
	
	CSafeMemFile smf(buff, len);
	
	di.enable = smf.ReadUInt8();
	di.offSet = smf.ReadUInt32() * 3600;
	di.type = smf.ReadUInt32();

	if (type == 0)
	{
		di.InMonth = smf.ReadUInt32();
		di.InWeekIndex = smf.ReadUInt8();
		di.InWday = smf.ReadUInt32();
		di.InSecond= smf.ReadUInt32();

		di.OutMonth = smf.ReadUInt32();
		di.OutWeekIndex = smf.ReadUInt8();
		di.OutWday = smf.ReadUInt32();
		di.OutSecond = smf.ReadUInt32();
	}
	else if (type == 1)
	{
		di.InMonth = smf.ReadUInt32();
		di.InMday = smf.ReadUInt8();
		di.InSecond = smf.ReadUInt32();

		di.OutMonth = smf.ReadUInt32();
		di.OutMday = smf.ReadUInt8();
		di.OutSecond = smf.ReadUInt32();
	}

	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_DAYLIGHT_INFO, -1, &di, sizeof(di));
	delete[] buff;
}

void CConfigDlg::SaveLiveConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	int chnn = m_chnnCombo.GetCurSel() - 1;
	DD_LIVE_DISPLAY ld;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_DISPLAY, -1, &ld, sizeof(ld), &retLen, FALSE);
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	{
		cfgMgr->GetData(3, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(3, buff, len, sublen, ch, type);

		CSafeMemFile smf(buff, len);
		ld.showTime = smf.ReadUInt8();
		ld.showNetwork = smf.ReadUInt8();
		ld.showHDD = smf.ReadUInt8();
		ld.showUSB = smf.ReadUInt8();
		ld.showAlarmIn = smf.ReadUInt8();
		ld.showAlarmOut = smf.ReadUInt8();

		delete[] buff;
	}

	{
		cfgMgr->GetData(4, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(4, buff, len, sublen, ch, type);

		CSafeMemFile smf(buff, len);

		int num = len / sublen;

		for (int i = 0; i < num; i++)
		{
			if (chnn == -1 || chnn == i || ch == chnn)//如果保存全部通道 或者 保存通道与当前通道相同 保存通道与表格通道相同
			{
				if (num == 1)//表格里只有一个数据的时候 ch是 有效的 i是无效的
				{
					i = ch;
				}
				ld.showCameraName[i] = smf.ReadUInt8();
				ld.showRecordStatus[i] = smf.ReadUInt8();
			}
			else
			{
				smf.Seek(sublen, CFile::current);//不满足条件跳过这一行数据
			}
		}
		
		delete[] buff;
	}	
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_DISPLAY, -1, &ld, sizeof(ld));
}

void CConfigDlg::SaveMainCameraConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	int vnum = m_devieInfo->videoInputNum;
	DD_LIVE_VIDEO_GROUP *lvg = new DD_LIVE_VIDEO_GROUP[vnum];

	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_MAIN_CAMERA, -1, lvg, sizeof(DD_LIVE_VIDEO_GROUP) * vnum, &retLen, FALSE);
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(5, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(5, buff, len, sublen, ch, type);
	
	int gnum = len / sublen;
	CSafeMemFile smf(buff, len);
	
	unsigned long splitmode;

	switch (sublen)
	{
	case 26:
		splitmode = DD_VIEW_SPLIT_1X1;
		break;
	case 86:
		splitmode = DD_VIEW_SPLIT_2X2;
		break;
	case 126:
		splitmode = DD_VIEW_SPLIT_2X3;
		break;
	case 186:
		splitmode = DD_VIEW_SPLIT_3X3;
		break;
	case 326:
		splitmode = DD_VIEW_SPLIT_4X4;
		break;
	case 506:
		splitmode = DD_VIEW_SPLIT_5X5;
		break;
	case 726:
		splitmode = DD_VIEW_SPLIT_6X6;
		break;
	}

	char text[20];
	for (int i = 0; i < gnum; i++)
	{
		smf.ReadUInt32();
		lvg[i].splitMode = splitmode; 
		lvg[i].holdTime = smf.ReadUInt16();
		
		memset(lvg[i].channel, 0xff, _countof(lvg[i].channel));
		int n = (sublen - 6) / 20;
		
		for (int j = 0; j < n; j++)
		{
			smf.Read(text, 20);
			if (_tcscmp(text, "none"))
			{
				int channel = -1;
				sscanf_s(text, _T("channel %d"), &channel);
				lvg[i].channel[channel - 1] = j;
			}
		}
	}

	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_MAIN_CAMERA, -1, lvg, retLen);
	delete[] lvg;
}

void CConfigDlg::SaveSpotConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	int vnum = m_devieInfo->videoInputNum;
	DD_LIVE_VIDEO_GROUP *lvg = new DD_LIVE_VIDEO_GROUP[vnum];

	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_SPOT_CAMERA, -1, lvg, sizeof(DD_LIVE_VIDEO_GROUP) * vnum, &retLen, FALSE);

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(6, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(6, buff, len, sublen, ch, type);

	int gnum = len / sublen;
	CSafeMemFile smf(buff, len);

	unsigned long splitmode = DD_VIEW_SPLIT_1X1;
	
	char text[20];
	for (int i = 0; i < gnum; i++)
	{
		smf.ReadUInt32();
		lvg[i].splitMode = splitmode; 
		lvg[i].holdTime = smf.ReadUInt16();

		memset(lvg[i].channel, 0xff, _countof(lvg[i].channel));
		int n = (sublen - 6) / 20;

		for (int j = 0; j < n; j++)
		{
			smf.Read(text, 20);
			if (_tcscmp(text, "none"))
			{
				int channel = -1;
				sscanf_s(text, _T("channel %d"), &channel);
				lvg[i].channel[channel - 1] = j;
			}
		}
	}

	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_SPOT_CAMERA, -1, lvg, retLen);
	delete[] lvg;
}

void CConfigDlg::SaveAudioConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	DD_LIVE_AUDIO_GROUP *lag = new DD_LIVE_AUDIO_GROUP[m_devieInfo->audioInputNum];
 
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_AUDIO, -1, lag, 
		sizeof(DD_LIVE_AUDIO_GROUP) * m_devieInfo->audioInputNum, &retLen, FALSE);
	
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;
	
	cfgMgr->GetData(7, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(7, buff, len, sublen, ch, type);

	CSafeMemFile smf(buff, len);
	
	int num = len / sublen;

	for (int i = 0; i < num; i++)
	{
		lag[i].holdTime = smf.ReadUInt16();
		lag[i].volume = smf.ReadUInt8();
		lag[i].channel = smf.ReadUInt8() - 1;
	}

	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_LIVE_AUDIO, -1, lag, retLen);

	delete[] lag;
	delete[] buff;
}

void CConfigDlg::SaveChnnConfig( CConfigManager *cfgMgr )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	//读取所有通道的数据 保存特定通道的数据
	DD_CHANNEL_CONFIG *cc = new DD_CHANNEL_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_CHNN_CONFIG, -1, cc, 
		sizeof(DD_CHANNEL_CONFIG) * m_devieInfo->videoInputNum, &retLen, FALSE);

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(8, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(8, buff, len, sublen, ch, type);

	int num = len / sublen;
	CSafeMemFile smf(buff, len);

	for (int i = 0; i < num; i++)
	{
		if (chnn == -1 || chnn == i || ch == chnn)
		{
			if (num == 1)
			{
				i = ch;
			}

			cc[i].hide = smf.ReadUInt8();
			smf.Read(cc[i].name, 132);
		}
		else
		{
			smf.Seek(sublen, CFile::current);
		}
	}
	
	int size = chnn < 0 ? retLen : sizeof(DD_CHANNEL_CONFIG);

#ifdef N9000_CONFIG
	
	for (int i = 0; i < num; i++)
	{
		if (chnn == -1 || chnn == i || ch == chnn)
		{
			if (num == 1)
			{
				i = ch;
			}

			//进行字符转换
			int  unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, cc[i].name, -1, NULL, 0);   
			wchar_t *pUnicode = new  wchar_t[unicodeLen+1]; 
			memset(pUnicode, 0, unicodeLen+1);
			int dwMinSize;
			MultiByteToWideChar(CP_ACP, 0, cc[i].name, -1, (LPWSTR)pUnicode, unicodeLen);  

			memset(cc[i].name, 0, DD_MAX_NAME_LEN);
			dwMinSize = WideCharToMultiByte(CP_UTF8,NULL,pUnicode,-1,NULL,0,NULL,FALSE);
			WideCharToMultiByte(CP_UTF8,NULL,pUnicode,-1,cc[i].name,dwMinSize,NULL,FALSE);
			delete[] pUnicode;
		}
	}
	
#endif
	
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_CHNN_CONFIG, chnn, &cc[chnn < 0? 0:chnn], size);

	delete []cc;
	delete []buff;
}

void CConfigDlg::SaveOSDConfig( CConfigManager *cfgMgr )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_VIDEO_OSD_CONFIG *voc = new DD_VIDEO_OSD_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_VIDEO_OSD, -1, voc, 
		sizeof(DD_VIDEO_OSD_CONFIG) * m_devieInfo->videoInputNum, &retLen, FALSE);
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(9, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(9, buff, len, sublen, ch, type);

	int num = len / sublen;
	CSafeMemFile smf(buff, len);

	for (int i = 0; i < num; i++)
	{
		if (chnn == -1 || chnn == i || ch == chnn)
		{
			if (num == 1)
			{
				i = ch;
			}

			voc[i].enableCameraName = smf.ReadUInt8();
			voc[i].enableTimeStamp = smf.ReadUInt8();
			voc[i].enableTimeStampWithWeek = smf.ReadUInt8();
			voc[i].enableDefineText = smf.ReadUInt8();

			voc[i].cameraName.x = smf.ReadUInt8();
			voc[i].cameraName.y = smf.ReadUInt8();

			voc[i].timeStamp.x = smf.ReadUInt8();
			voc[i].timeStamp.y = smf.ReadUInt8();

			voc[i].defineText.x = smf.ReadUInt8();
			voc[i].defineText.y = smf.ReadUInt8();

			smf.Read(voc[i].text, 132);
		}
		else
		{
			smf.Seek(sublen, CFile::current);
		}
	}
	
#ifdef N9000_CONFIG
	//test 取值范围 0~10000
	voc[3].timeStamp.x = 9400;
	voc[3].timeStamp.y = 9400;
	voc[3].enableCameraName = TRUE;
	voc[3].enableTimeStamp = TRUE;
#endif
	int size = chnn < 0 ? retLen : sizeof(DD_VIDEO_OSD_CONFIG);
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_VIDEO_OSD, chnn, &voc[chnn < 0? 0:chnn], size);

	delete[] voc;
	delete[] buff;
}

void CConfigDlg::SaveEncodeConfig( CConfigManager *cfgMgr )
{
#ifdef N9000_CONFIG
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_ENCODE_CONFIG_N9000 *ec = new DD_ENCODE_CONFIG_N9000[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_SCHEDULE, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG_N9000) * m_devieInfo->videoInputNum, &retLen, FALSE);
#else
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_ENCODE_CONFIG *ec = new DD_ENCODE_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_SCHEDULE, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG) * m_devieInfo->videoInputNum, &retLen, FALSE);
#endif

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(10, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(10, buff, len, sublen, ch, type);

	int num = len / sublen;
	CSafeMemFile smf(buff, len);

	for (int i = 0; i < num; i++)
	{
		if (chnn == -1 || chnn == i || ch == chnn)
		{
			if (num == 1)
			{
				i = ch;
			}
#ifdef N9000_CONFIG
			//test 分辨率参考 DD_VGA_RESOLUTION
			ec[i].resolution = DD_VGA_1920X1080;
			ec[i].rate = 25 ;
#else
			SETBIT(ec[i].resolution, smf.ReadUInt32());
			ec[i].rate = smf.ReadUInt16();
#endif

			ec[i].encodeType = smf.ReadUInt32();
			ec[i].quality = smf.ReadUInt32();
			ec[i].maxBitrate = smf.ReadUInt32();
			ec[i].minBitrate = smf.ReadUInt32();
		}
		else
		{
			smf.Seek(sublen, CFile::current);
		}
	}

	int size = chnn < 0 ? retLen : sizeof(DD_ENCODE_CONFIG_N9000);
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_SCHEDULE, chnn, &ec[chnn < 0? 0:chnn], size);

	delete[] ec;
	delete[] buff;
}

void CConfigDlg::SaveEncodeAlarmConfig( CConfigManager *cfgMgr )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;

#ifdef N9000_CONFIG
	DD_ENCODE_CONFIG_N9000 *ec = new DD_ENCODE_CONFIG_N9000[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_ALARM, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG_N9000) * m_devieInfo->videoInputNum, &retLen, FALSE);
#else
	DD_ENCODE_CONFIG *ec = new DD_ENCODE_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_ALARM, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG) * m_devieInfo->videoInputNum, &retLen, FALSE);
#endif

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(11, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(11, buff, len, sublen, ch, type);

	int num = len / sublen;
	CSafeMemFile smf(buff, len);

	for (int i = 0; i < num; i++)
	{
		if (chnn == -1 || chnn == i || ch == chnn)
		{
			if (num == 1)
			{
				i = ch;
			}

#ifdef N9000_CONFIG
			//test 分辨率参考 DD_VGA_RESOLUTION
			ec[i].resolution = DD_VGA_1920X1080;
			ec[i].rate = 25 ;
#else
			SETBIT(ec[i].resolution, smf.ReadUInt32());
			ec[i].rate = smf.ReadUInt16();
#endif
			ec[i].encodeType = smf.ReadUInt32();
			ec[i].quality = smf.ReadUInt32();
			ec[i].maxBitrate = smf.ReadUInt32();
			ec[i].minBitrate = smf.ReadUInt32();
		}
		else
		{
			smf.Seek(sublen, CFile::current);
		}
	}

	int size = chnn < 0 ? retLen : sizeof(DD_ENCODE_CONFIG_N9000);
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_ALARM, chnn, &ec[chnn < 0? 0:chnn], size);

	delete[] ec;
	delete[] buff;
}

void CConfigDlg::SaveEncodeNetConfig( CConfigManager *cfgMgr )
{
#ifdef N9000_CONFIG
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_ENCODE_CONFIG_N9000 *ec = new DD_ENCODE_CONFIG_N9000[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_NETWORK, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG_N9000) * m_devieInfo->videoInputNum, &retLen, FALSE);
#else
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_ENCODE_CONFIG *ec = new DD_ENCODE_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_NETWORK, chnn, ec, 
		sizeof(DD_ENCODE_CONFIG) * m_devieInfo->videoInputNum, &retLen, FALSE);
#endif

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(12, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(12, buff, len, sublen, ch, type);

	int num = len / sublen;
	CSafeMemFile smf(buff, len);

	for (int i = 0; i < num; i++)
	{
		if (chnn == -1 || chnn == i || ch == chnn)
		{
			if (num == 1)
			{
				i = ch;
			}

#ifdef N9000_CONFIG
			ec[i].resolution = DD_VGA_352X240;
			ec[i].rate = 15;
#else
			SETBIT(ec[i].resolution, smf.ReadUInt32());
			ec[i].rate = smf.ReadUInt16();
#endif
			ec[i].encodeType = smf.ReadUInt32();
			ec[i].quality = smf.ReadUInt32();
			ec[i].maxBitrate = smf.ReadUInt32();
			ec[i].minBitrate = smf.ReadUInt32();
		}
		else
		{
			smf.Seek(sublen, CFile::current);
		}
	}

	ec[3].resolution = DD_VGA_704X480;
	ec[3].rate = 13;
	int size = chnn < 0 ? retLen : sizeof(DD_ENCODE_CONFIG);
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_ENCODE_NETWORK, chnn, &ec[chnn < 0? 0:chnn], size);

	delete[] ec;
	delete[] buff;
}

void CConfigDlg::SaveRecPlanConfig( CConfigManager *cfgMgr )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	DWORD dataLen = sizeof(DD_WEEK_SCHEDULE);

	DD_WEEK_SCHEDULE *pWeekSchedule = new DD_WEEK_SCHEDULE[m_devieInfo->videoInputNum];
	memset(pWeekSchedule, 0, sizeof(DD_WEEK_SCHEDULE)*m_devieInfo->videoInputNum);

	if(chnn >= 0)
	{
		//把第周day这天的第day小时设置排程
		for(int day = 0; day < 7; day++)
		{
			memset(&pWeekSchedule[chnn].week[day].hour[day], 0xff, 8);
		}
	}
	else
	{
		for(int i = 0; i < m_devieInfo->videoInputNum; i++)
		{
			for(int day = 0; day < 7; day++)
			{
				memset(&pWeekSchedule[i].week[day].hour[day], 0xff, 8);
			}
		}
		dataLen *= m_devieInfo->videoInputNum;
	}

	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_RECORD_SCHEDULE_SCHEDULE, chnn, &pWeekSchedule[chnn < 0 ? 0 : chnn], dataLen);

	delete [] pWeekSchedule;
}

void CConfigDlg::SaveMotionPlanConfig( CConfigManager *cfgMgr )
{

}

void CConfigDlg::SaveSensorPlanConfig( CConfigManager *cfgMgr )
{

}

void CConfigDlg::SaveNetConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	DD_NETWORK_IP_CONFIG nic;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_IP, -1, &nic, 
		sizeof(DD_NETWORK_IP_CONFIG), &retLen, FALSE);
	DD_NETWORK_ADVANCE_CONFIG nac;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_ADVANCE, -1, &nac, 
		sizeof(DD_NETWORK_ADVANCE_CONFIG), &retLen, FALSE);
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	{
		cfgMgr->GetData(17, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(17, buff, len, sublen, ch, type);

		CSafeMemFile smf(buff, len);

		nic.useDHCP = smf.ReadUInt8();
		nic.IP = smf.ReadUInt32();
		nic.subnetMask = smf.ReadUInt32();
		nic.gateway = smf.ReadUInt32();
		nic.preferredDNS = smf.ReadUInt32();
		nic.alternateDNS = smf.ReadUInt32();
		nic.useDHCP = smf.ReadUInt8();
		smf.Read(nic.account, 132);
		smf.Read(nic.password, 132);
		
		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_IP, -1, &nic, sizeof(DD_NETWORK_IP_CONFIG));
		delete[] buff;
	}

	{
		cfgMgr->GetData(18, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(18, buff, len, sublen, ch, type);

		CSafeMemFile smf(buff, len);

		nac.httpPort = smf.ReadUInt16();
		nac.datePort = smf.ReadUInt16();
		nac.messagePort = smf.ReadUInt16();
		nac.alarmPort = smf.ReadUInt16();
		nac.OnlineUserNum = smf.ReadUInt16();
		nac.multiCastIP = smf.ReadUInt32();
		nac.mtuByteNum = smf.ReadUInt32();

		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_ADVANCE, -1, &nac, sizeof(DD_NETWORK_ADVANCE_CONFIG));
		delete[] buff;
	}
}

void CConfigDlg::SaveDDNSConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	DD_DDNS_CONFIG dc;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_DDNS, -1, &dc, 
		sizeof(DD_DDNS_CONFIG), &retLen, FALSE);

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	{
		cfgMgr->GetData(19, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(19, buff, len, sublen, ch, type);

#ifdef N9000_CONFIG
	/*N9000中，DDNS服务器类型对应关系为：
		typedef enum _DDNS_SERVER_TYPE
		{
			DDNS_NONE =0,
			DDNS_88IP,		//www.88ip.net     
			DDNS_DNS2P,		//www.dns2p.net
			DDNS_MEIBU,     //www.meibu.com
			DDNS_DYNDNS,    //www.dyndns.com
			DDNS_NOIPDNS,   //www.no-ip.com
			DDNS_DVRDYDNS,  //www.dvrdydns.com
			DDNS_MINTDNS,	//MintDNS 
			DDNS_3322,		//www.3322.org  
			DDNS_123DDNS,	//www.123ddns.com
			DDNS_EASTERNDNS,//www.easterndns.com,
			DDNS_MYQSEE,	//www.myq-see.com 
			DDNS_DDNSBR,	//www.ddns.com.br
			DDNS_TECVOZDDNS,//www.tecvozddns.com.br
			DDNS_HIVIEW,	//www.hiview.org
			DDNS_AUTODDNS,	//www.autoddns.com
			DDNS_END,
		}DDNS_SERVER_TYPE;
	*/
		//test
		dc.enable = 0;
		dc.useDDNSServer = 3;
		memcpy(dc.userName, "aabbccdd", 9);
#else
		CSafeMemFile smf(buff, len);

		dc.enable = smf.ReadUInt8();
		dc.interval = smf.ReadUInt16() * 60;
		dc.useDDNSServer = smf.ReadUInt32();
		dc.userHostDomain = smf.ReadUInt8();
		smf.Read(dc.userName, 132);
		smf.Read(dc.password, 132);
		smf.Read(dc.hostDomain, 132);
#endif

		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_DDNS, -1, &dc, sizeof(DD_DDNS_CONFIG));
		delete[] buff;
	}
}

void CConfigDlg::SaveMailConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	DD_SMTP_CONFIG sc;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_SMTP, -1, &sc, 
		sizeof(DD_SMTP_CONFIG), &retLen, FALSE);

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	{
		cfgMgr->GetData(20, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(20, buff, len, sublen, ch, type);
		
		CSafeMemFile smf(buff, len);

		sc.port = smf.ReadUInt16();
		sc.enableSSL = smf.ReadUInt8();
		smf.Read(sc.server, 260);
		smf.Read(sc.sendAddress, 260);
		smf.Read(sc.password, 132);
		smf.Read(sc.receiveAddress[0], 260);
		smf.Read(sc.receiveAddress[1], 260);
		smf.Read(sc.receiveAddress[2], 260);

		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_SMTP, -1, &sc, sizeof(DD_SMTP_CONFIG));
		delete[] buff;
	}
}

void CConfigDlg::SaveUserConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	DD_ACCOUNT_CONFIG *users = new DD_ACCOUNT_CONFIG[DD_MAX_ACCOUNT_NUM];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_ACCOUNT, -1, users, 
		sizeof(DD_ACCOUNT_CONFIG) * DD_MAX_ACCOUNT_NUM, &retLen, FALSE);
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(21, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(21, buff, len, sublen, ch, type);
	CSafeMemFile smf(buff, len);

	int num = len / sublen;
	for(int i = 0; i < num; i++)
	{
		users[i].enable = smf.ReadUInt8();
		users[i].bindMAC = smf.ReadUInt8();
		users[i].group = smf.ReadUInt32();
		smf.Read(users[i].MAC, 8);
		smf.Read(users[i].name, 132);
		smf.Read(users[i].password, 132);

		users[i].logSearch = smf.ReadUInt8();
		users[i].systemSetup = smf.ReadUInt8();
		users[i].fileManagement = smf.ReadUInt8();
		users[i].diskManagement = smf.ReadUInt8();
		users[i].remoteLogin = smf.ReadUInt8();
		users[i].twoWayAudio = smf.ReadUInt8();
		users[i].systemMaintain = smf.ReadUInt8();
		users[i].OnlineUserManagement = smf.ReadUInt8();
		users[i].shutdown = smf.ReadUInt8();
		users[i].alarmOutCtrl = smf.ReadUInt8();
		users[i].netAlarm = smf.ReadUInt8();
		users[i].netSerialCtrl = smf.ReadUInt8();
	}
		
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_ACCOUNT, -1, users, sizeof(DD_ACCOUNT_CONFIG) * DD_MAX_ACCOUNT_NUM);
	delete[] users;
	delete[] buff;
}

void CConfigDlg::SaveMotionConfig( CConfigManager *cfgMgr )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	unsigned long retLen2 = 0;

	//N9000 时的 DD_CONFIG_ITEM_MOTION_ALARM_OUT 与 DD_CONFIG_ITEM_MOTION_ALARM_OUT 
	//此时DD_TRIGGER_ALARM_OUT的ShowFullScreen变量，1表示弹出，0xff表示不弹出，不代表通道
	DD_MOTION_CONFIG *mc = new DD_MOTION_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_MOTION_SETUP, chnn, mc, 
		sizeof(DD_MOTION_CONFIG) * m_devieInfo->videoInputNum, &retLen, FALSE);

	DD_TRIGGER_ALARM_OUT *tao = new DD_TRIGGER_ALARM_OUT[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_MOTION_ALARM_OUT, chnn, tao, 
		sizeof(DD_TRIGGER_ALARM_OUT) * m_devieInfo->videoInputNum, &retLen2, FALSE);
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;
	
	{
		cfgMgr->GetData(24, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(24, buff, len, sublen, ch, type);

		int num = len / sublen;
		CSafeMemFile smf(buff, len);

		for (int i = 0; i < num; i++)
		{
			if (chnn == -1 || chnn == i || ch == chnn)
			{
				if (num == 1)
				{
					i = ch;
				}

				mc[i].enable = smf.ReadUInt8();
				mc[i].holdTime = smf.ReadUInt16();
				mc[i].area.sensitivity = smf.ReadUInt32();
				mc[i].area.widthNum = smf.ReadUInt16();
				mc[i].area.heightNum = smf.ReadUInt16();
			}
			else
			{
				smf.Seek(sublen, CFile::current);
			}
		}
#ifdef N9000_CONFIG
		mc[0].enable = 0;
		mc[0].holdTime = 5;
		mc[0].area.sensitivity = 5;
		mc[2].enable = 0;
		mc[2].holdTime = 5;
		mc[2].area.sensitivity = 5;
		mc[0].area.area[0][0] = 0;
		mc[0].area.area[0][1] = 0;
		mc[0].area.area[1][0] = 0;
		mc[0].area.area[1][1] = 0;
		mc[0].area.area[2][0] = 0;
		mc[0].area.area[2][1] = 0;
		mc[2].area.area[0][0] = 0;
		mc[2].area.area[0][1] = 0;
		mc[2].area.area[1][0] = 0;
		mc[2].area.area[1][1] = 0;
		mc[2].area.area[2][0] = 0;
		mc[2].area.area[2][1] = 0;
#endif
		int size = chnn < 0 ? retLen : sizeof(DD_MOTION_CONFIG);
		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_MOTION_SETUP, chnn, &mc[chnn < 0? 0:chnn], size);
		delete[] buff;
	}
	
	{
		cfgMgr->GetData(25, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(25, buff, len, sublen, ch, type);

		int num = len / sublen;
		CSafeMemFile smf(buff, len);

		
		for (int i = 0; i < num; i++)
		{
			if (chnn == -1 || chnn == i || ch == chnn)
			{
				if (num == 1)
				{
					i = ch;
				}

				tao[i].toBuzzer = smf.ReadUInt8();
				tao[i].ShowFullScreen = smf.ReadUInt8();
				tao[i].sendEmail = smf.ReadUInt8();
				tao[i].toUploadToAlarmCentre = smf.ReadUInt8();
				tao[i].toAlarmOut = smf.ReadUInt32();
			}
			else
			{
				smf.Seek(sublen, CFile::current);
			}
		}

		int size = chnn < 0 ? retLen2 : sizeof(DD_TRIGGER_ALARM_OUT);
#ifdef N9000_CONFIG
		tao[0].sendEmail = 1;
		tao[0].ShowFullScreen = 1;
		tao[0].toBuzzer = 1;
		tao[3].sendEmail = 1;
		tao[3].ShowFullScreen = 1;
		tao[3].toBuzzer = 1;
		tao[2].sendEmail = 0;
		tao[2].ShowFullScreen = 0xff;
		tao[2].toBuzzer = 1;
#endif
		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_MOTION_ALARM_OUT, chnn, &tao[chnn < 0? 0:chnn], size);
		delete[] buff;
	}

	delete[] mc;
	delete[] tao;
}

void CConfigDlg::SaveSensorConfig( CConfigManager *cfgMgr )
{
	unsigned long retLen = 0;
	unsigned long retLen2 = 0;
	int chnn = m_chnnCombo.GetCurSel() - 1;

	DD_SENSOR_CONFIG *sc = new DD_SENSOR_CONFIG[m_devieInfo->sensorInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_SENSOR_SETUP, -1, sc, 
		sizeof(DD_SENSOR_CONFIG) * m_devieInfo->sensorInputNum, &retLen, FALSE);

	DD_TRIGGER_ALARM_OUT *tao = new DD_TRIGGER_ALARM_OUT[m_devieInfo->sensorInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_SENSOR_ALARM_OUT, -1, tao, 
		sizeof(DD_TRIGGER_ALARM_OUT) * m_devieInfo->sensorInputNum, &retLen2, FALSE);
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	{
		cfgMgr->GetData(22, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(22, buff, len, sublen, ch, type);

#ifdef N9000_CONFIG
		//test
		sc[0].enable = 1;
		sc[0].bNO = 1;
		sc[0].holdTime = 10;
		sc[1].enable = 1;
		sc[1].bNO = 0;
		sc[2].enable = 1;
		sc[2].bNO = 1;
		sc[3].enable = 0;
		sc[3].bNO = 0;

#else
		int num = len / sublen;
		CSafeMemFile smf(buff, len);

		for (int i = 0; i < num; i++)
		{
			if (chnn == -1 || chnn == i || ch == chnn)
			{
				if (num == 1)
				{
					i = ch;
				}

				sc[i].enable = smf.ReadUInt8();
				sc[i].bNO = smf.ReadUInt32();
				sc[i].holdTime = smf.ReadUInt16();
				smf.Read(sc[i].name, 132);
			}
			else
			{
				smf.Seek(sublen, CFile::current);
			}
		}
#endif
		int size = chnn < 0 ? retLen : sizeof(DD_SENSOR_CONFIG);
		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_SENSOR_SETUP, chnn, &sc[chnn < 0? 0:chnn], size);
		delete[] buff;
	}

	{
		cfgMgr->GetData(23, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(23, buff, len, sublen, ch, type);

#ifdef N9000_CONFIG
		//test
		tao[0].sendEmail = 1;
		tao[0].ShowFullScreen = 1;
		tao[0].toBuzzer = 1;
		tao[1].sendEmail = 1;
		tao[1].ShowFullScreen = 2;
		tao[1].toBuzzer = 0;
		tao[2].sendEmail = 0;
		tao[2].ShowFullScreen = 0xff;
		tao[2].toBuzzer = 1;
#else
		int num = len / sublen;
		CSafeMemFile smf(buff, len);
	
		for (int i = 0; i < num; i++)
		{
			if (chnn == -1 || chnn == i || ch == chnn)
			{
				if (num == 1)
				{
					i = ch;
				}

				tao[i].toBuzzer = smf.ReadUInt8();
				tao[i].ShowFullScreen = smf.ReadUInt8();
				tao[i].sendEmail = smf.ReadUInt8();
				tao[i].toUploadToAlarmCentre = smf.ReadUInt8();
				tao[i].toAlarmOut = smf.ReadUInt32();
			}
			else
			{
				smf.Seek(sublen, CFile::current);
			}
		}
#endif
 		int size = chnn < 0 ? retLen2 : sizeof(DD_TRIGGER_ALARM_OUT);
		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_SENSOR_ALARM_OUT, chnn, &tao[chnn < 0? 0:chnn], size);
		delete[] buff;
	}

	delete[] sc;
	delete[] tao;
}

void CConfigDlg::SaveVlossConfig( CConfigManager *cfgMgr )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	
	DD_TRIGGER_ALARM_OUT *tao = new DD_TRIGGER_ALARM_OUT[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_VLOSS_ALARM_OUT, chnn, tao, 
		sizeof(DD_TRIGGER_ALARM_OUT) * m_devieInfo->videoInputNum, &retLen, FALSE);
	
	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	{
		cfgMgr->GetData(26, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(26, buff, len, sublen, ch, type);
		
		int num = len / sublen;
		CSafeMemFile smf(buff, len);

		
		for (int i = 0; i < num; i++)
		{
			if (chnn == -1 || chnn == i || ch == chnn)
			{
				if (num == 1)
				{
					i = ch;
				}

				tao[i].toBuzzer = smf.ReadUInt8();
				tao[i].ShowFullScreen = smf.ReadUInt8();
				tao[i].sendEmail = smf.ReadUInt8();
				tao[i].toUploadToAlarmCentre = smf.ReadUInt8();
				tao[i].toAlarmOut = smf.ReadUInt32();
			}
			else
			{
				smf.Seek(sublen, CFile::current);
			}
		}
#ifdef N9000_CONFIG
		tao[0].sendEmail = 1;
		tao[0].ShowFullScreen = 1;
		tao[0].toBuzzer = 1;
		tao[1].sendEmail = 1;
		tao[1].ShowFullScreen = 2;
		tao[1].toBuzzer = 0;
#endif
		int size = chnn < 0 ? retLen : sizeof(DD_TRIGGER_ALARM_OUT);
		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_VLOSS_ALARM_OUT, chnn, &tao[chnn < 0? 0:chnn], size);
		delete[] buff;
	}
}

void CConfigDlg::SavePTZConfig( CConfigManager *cfgMgr )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	unsigned long retLen2 = 0;

	DD_PTZ_CONFIG *pc = new DD_PTZ_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_PTZ_SETUP, chnn, pc, 
		sizeof(DD_PTZ_CONFIG) * m_devieInfo->videoInputNum, &retLen, FALSE);

	DD_BUZZER_CONFIG bc;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_BUZZER_SETUP, -1, &bc, 
		sizeof(DD_BUZZER_CONFIG), &retLen2, FALSE);

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	{
		cfgMgr->GetData(27, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(27, buff, len, sublen, ch, type);
		
		int num = len / sublen;
		CSafeMemFile smf(buff, len);
		
		for (int i = 0; i < num; i++)
		{
			if (chnn == -1 || chnn == i || ch == chnn)
			{
				if (num == 1)
				{
					i = ch;
				}

				pc[i].enable = smf.ReadUInt8();
				pc[i].address = smf.ReadUInt8();
				pc[i].protocol = smf.ReadUInt32();
				pc[i].serial.baudRate = smf.ReadUInt32();
				pc[i].serial.dataBit = smf.ReadUInt32();
				pc[i].serial.stopBit = smf.ReadUInt32();
				pc[i].serial.parity = smf.ReadUInt32();
				pc[i].serial.dataFlowControl = smf.ReadUInt32();
			}
			else
			{
				smf.Seek(sublen, CFile::current);
			}
		}

#ifdef N9000_CONFIG
				/*
		enum N9000_PROTOCOL_TYPE
		{
		PROTOCOL_NULL = 0,
		PROTOCOL_PELCOP,
		PROTOCOL_PELCOD,
		PROTOCOL_LILIN,
		PROTOCOL_MINKING,
		PROTOCOL_NEON,
		PROTOCOL_STAR,
		PROTOCOL_VIDO,
		PROTOCOL_DSCP,
		PROTOCOL_VISCA,
		PROTOCOL_TVT,
		PROTOCOL_COC,
		PROTOCOL_END
		};


		//串口类型值定义
		enum PORT_BAUDRATE
		{
		SBR_110 = 0,
		SBR_300,
		SBR_600,
		SBR_1200,
		SBR_2400,
		SBR_4800,
		SBR_9600,
		SBR_19200,
		SBR_38400,
		SBR_57600,
		SBR_115200,
		SBR_230400,
		SBR_460800,
		SBR_921600
		};
		*/
		//test
		pc[0].protocol = 10;
		pc[0].address = 2;
		pc[0].serial.baudRate = 5;
		pc[2].protocol = 4;
		pc[2].address = 3;
		pc[2].serial.baudRate = 7;
#endif

		int size = chnn < 0 ? retLen : sizeof(DD_PTZ_CONFIG);
		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_PTZ_SETUP, chnn, &pc[chnn < 0? 0:chnn], size);
		delete[] buff;
	}

	{
		cfgMgr->GetData(28, NULL, len, sublen, ch, type);
		BYTE *buff = new BYTE[len];
		cfgMgr->GetData(28, buff, len, sublen, ch, type);
		
		int num = len / sublen;
		CSafeMemFile smf(buff, len);

		bc.enable = smf.ReadUInt8();
		bc.holdTime = smf.ReadUInt16();

#ifdef N9000_CONFIG
		//test
		bc.holdTime = 120; //120表示2分钟，一分钟60秒
#endif
		NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_BUZZER_SETUP, -1, &bc, sizeof(DD_BUZZER_CONFIG));
		delete[] buff;
	}

	delete[] pc;
}

void CConfigDlg::SaveRecordConfig( CConfigManager *cfgMgr )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_RECORD_CONFIG *rc = new DD_RECORD_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_RECORD_SETUP, chnn, rc, 
		sizeof(DD_RECORD_CONFIG) * m_devieInfo->videoInputNum, &retLen, FALSE);

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(13, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(13, buff, len, sublen, ch, type);
	
	int num = len / sublen;
	CSafeMemFile smf(buff, len);

	for (int i = 0; i < num; i++)
	{
		if (chnn == -1 || chnn == i || ch == chnn)
		{
			if (num == 1)
			{
				i = ch;
			}

			rc[i].bOnlyVideo = smf.ReadUInt8();
			rc[i].bWithAudio = smf.ReadUInt8();
			rc[i].bindAudioChannel = smf.ReadUInt8();
			rc[i].bRedundancy = smf.ReadUInt8();
			rc[i].preAlarmTime = smf.ReadUInt16();
			rc[i].postAlarmTime = smf.ReadUInt16();
			rc[i].expired = smf.ReadUInt16();
		}
		else
		{
			smf.Seek(sublen, CFile::current);
		}
	}

	int size = chnn < 0 ? retLen : sizeof(DD_RECORD_CONFIG);
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_RECORD_SETUP, chnn, &rc[chnn < 0? 0:chnn], size);
	delete[] buff;
	delete[] rc;
}

void CConfigDlg::LoadColorConfig( CConfigManager *cfgMgr, bool isdefault )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_DEVICE_INFO deviceInfo;
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_DEVICE_INFO, -1, &deviceInfo, 
		sizeof(DD_DEVICE_INFO) , &retLen, isdefault);

	DD_VIDEO_COLOR_CONFIG *vcc = new DD_VIDEO_COLOR_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_VIDEO_COLOR, chnn, vcc, 
		sizeof(DD_VIDEO_COLOR_CONFIG) * m_devieInfo->videoInputNum, &retLen, isdefault);

	int num = retLen / sizeof(DD_VIDEO_COLOR_CONFIG);

	CSafeMemFile smf;
	for (int i = 0; i < num; i++)
	{
		smf.WriteUInt8(vcc[i].videoColor[0].brightness);
		smf.WriteUInt8(vcc[i].videoColor[0].contrast);
		smf.WriteUInt8(vcc[i].videoColor[0].hue);
		smf.WriteUInt8(vcc[i].videoColor[0].saturation);
	}

	int sublen = 4;
	cfgMgr->Feed(29, chnn, smf.GetBuffer(), smf.GetLength(), _T("4;Bx4"), sublen, COLOR_HEADER);
	delete[] vcc;
}

void CConfigDlg::SaveColorConfig( CConfigManager *cfgMgr )
{
	int chnn = m_chnnCombo.GetCurSel() - 1;
	unsigned long retLen = 0;
	DD_VIDEO_COLOR_CONFIG *vcc = new DD_VIDEO_COLOR_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_VIDEO_COLOR, chnn, vcc, 
		sizeof(DD_VIDEO_COLOR_CONFIG) * m_devieInfo->videoInputNum, &retLen, FALSE);

	long len = 0;
	long sublen = 0;
	int type = 0;
	int ch = -1;

	cfgMgr->GetData(29, NULL, len, sublen, ch, type);
	BYTE *buff = new BYTE[len];
	cfgMgr->GetData(29, buff, len, sublen, ch, type);

	int num = len / sublen;
	CSafeMemFile smf(buff, len);

	for (int i = 0; i < num; i++)
	{
		if (chnn == -1 || chnn == i || ch == chnn)
		{
			if (num == 1)
			{
				i = ch;
			}

			vcc[i].videoColor->brightness = smf.ReadUInt8();
			vcc[i].videoColor->contrast = smf.ReadUInt8();
			vcc[i].videoColor->hue = smf.ReadUInt8();
			vcc[i].videoColor->saturation = smf.ReadUInt8();			
		}
		else
		{
			smf.Seek(sublen, CFile::current);
		}
	}

#ifdef N9000_CONFIG
	vcc[0].videoColor->brightness = 203;
	vcc[0].videoColor->contrast = 204;
	vcc[0].videoColor->hue = 55;
	vcc[0].videoColor->saturation = 88;
	vcc[4].videoColor->brightness = 88;
	vcc[4].videoColor->contrast = 88;
	vcc[4].videoColor->hue = 88;
	vcc[4].videoColor->saturation = 88;
#endif
	int size = chnn < 0 ? retLen : sizeof(DD_VIDEO_COLOR_CONFIG);
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_VIDEO_COLOR, chnn, &vcc[chnn < 0? 0:chnn], size);
	delete[] buff;
	delete[] vcc;
}

void CConfigDlg::OnBnClickedBtnGetdevice()
{
	//读取所有通道的数据 保存特定通道的数据
	DWORD retLen = 0;
	DD_CHANNEL_CONFIG *cc = new DD_CHANNEL_CONFIG[m_devieInfo->videoInputNum];
	NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_CHNN_CONFIG, -1, cc, 
		sizeof(DD_CHANNEL_CONFIG) * m_devieInfo->videoInputNum, &retLen, FALSE);

	//获取设备名称并转换成可显示
	char szDest[512] = {0};
	UTF8ToGB(szDest, sizeof(szDest), cc[0].name, DD_MAX_CAMERA_NAME_BUF_LEN);
	GetDlgItem(IDC_EDIT_DeviceName)->SetWindowText(szDest);
	AfxMessageBox(szDest);
}

void CConfigDlg::OnBnClickedBtnSetdevicename()
{
	DD_CHANNEL_CONFIG *cc = new DD_CHANNEL_CONFIG[m_devieInfo->videoInputNum];

	CString strDeviceName;
	GetDlgItem(IDC_EDIT_DeviceName)->GetWindowText(strDeviceName);
	char szDest[512] = {0};
	memset(szDest, 0, sizeof(szDest));
	strcpy(szDest, strDeviceName.GetBuffer(0));
	ANSItoUTF8(szDest, sizeof(szDest), cc[0].name, DD_MAX_CAMERA_NAME_BUF_LEN);

	int size = sizeof(DD_CHANNEL_CONFIG);
	NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_CHNN_CONFIG, 0, &cc[0], size);
}

void CConfigDlg::OnBnClickedButtonUpgradePathBrowse()
{
	CString fname;
	CFileDialog dlg(TRUE, _T(".tar"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("tar Files(*.tar)|*.tar|All Files(*.*)|*.*||"));

	if (dlg.DoModal() == IDOK)
	{
		fname = dlg.GetPathName();
		GetDlgItem(IDC_EDIT_UpgradeFilePath)->SetWindowText(fname.GetBuffer(0));
	}
}

void CConfigDlg::OnBnClickedButtonUpgrade()
{
	CString strPath;
	GetDlgItem(IDC_EDIT_UpgradeFilePath)->GetWindowText(strPath);
	if (_access(strPath.GetBuffer(0), 0) == -1)
	{
		AfxMessageBox("Please select Upgrade file!");
		return;
	}
	m_lUpgradeHandle = NET_SDK_Upgrade(m_userID, strPath.GetBuffer(0));
	if (m_lUpgradeHandle > 0)
	{
		::SetTimer(GetSafeHwnd(), TIME_EVENT_UPGRADE, 1000, NULL);
	}
}

void CConfigDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (TIME_EVENT_UPGRADE == nIDEvent)
	{
		int nState = NET_SDK_GetUpgradeState(m_lUpgradeHandle);
		int nPos = NET_SDK_GetUpgradeProgress(m_lUpgradeHandle);
		if (nPos == -1)
		{
		}
		else if (nPos >= 0 && nPos <= 100)
		{

		}

		TRACE("Upgrade Pos=%d, nState=%d\r\n", nPos, nState);
	}

	CDialog::OnTimer(nIDEvent);
}

void CConfigDlg::OnBnClickedBtnGetdeviceipcaminfo()
{
 	NET_SDK_IPC_DEVICE_INFO ipcInfo[32] = {0};
 	LONG lIPCCount = 0;
	CString strLog;
 	BOOL bRet = NET_SDK_GetDeviceIPCInfo(m_userID, ipcInfo, sizeof(ipcInfo), &lIPCCount);
 	if (bRet)
 	{
		char szTemp[512] = {0};
		for (int i = 0; i < lIPCCount; i++)
		{
			sprintf_s(szTemp, sizeof(szTemp), "channel=%d,IP=%s,UserName=%s,Port=%d, status=%d\r\n", ipcInfo[i].channel, ipcInfo[i].szServer, ipcInfo[i].username, ipcInfo[i].nPort, ipcInfo[i].status);
			strLog += szTemp;
		}
		AfxMessageBox(strLog);
 	}
	else
	{
		AfxMessageBox("faild to get device IPC info!");
	}
}
