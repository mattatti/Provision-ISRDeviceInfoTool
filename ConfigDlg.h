#pragma once
#include "afxcmn.h"
#include "DVR_NET_SDK.h"
#include "afxwin.h"

#define TIME_EVENT_UPGRADE		101

#define N9000_CONFIG

// CConfigDlg 对话框
class CConfigManager;


class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfigDlg();

// 对话框数据
	enum { IDD = IDD_CONFIG_DIALOG };
	
protected:
	NET_SDK_DEVICEINFO		*m_devieInfo;
	LONG					m_userID;
	BOOL					m_isEnterConfig;
	CString					m_exportPath;
	CString					m_importPath;
	CComboBox				m_chnnCombo;
	LONG					m_lUpgradeHandle;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void EnableChildWindows(BOOL enable);
	void UpdateUI();
	void LoadConfig(CConfigManager *cfgMgr, ULONG test, bool default = false);
	void RegConfig(CConfigManager *cfgMgr, ULONG test);
	BOOL SaveConfig(CConfigManager *cfgMgr, ULONG test);
	
	void LoadBasicConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadDatetimeConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadDSTConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadLiveConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadMainCameraConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadSpotConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadAudioConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadChnnConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadOSDConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadEncodeConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadEncodeAlarmConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadEncodeNetConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadRecPlanConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadMotionPlanConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadSensorPlanConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadNetConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadDDNSConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadMailConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadUserConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadMotionConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadSensorConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadVlossConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadPTZConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadRecordConfig(CConfigManager *cfgMgr, bool isdefault);
	void LoadColorConfig(CConfigManager *cfgMgr, bool isdefault);

	void SaveBasicConfig(CConfigManager *cfgMgr);
	void SaveDatetimeConfig(CConfigManager *cfgMgr);
	void SaveDSTConfig(CConfigManager *cfgMgr);
	void SaveLiveConfig(CConfigManager *cfgMgr);
	void SaveMainCameraConfig(CConfigManager *cfgMgr);
	void SaveSpotConfig(CConfigManager *cfgMgr);
	void SaveAudioConfig(CConfigManager *cfgMgr);
	void SaveChnnConfig(CConfigManager *cfgMgr);
	void SaveOSDConfig(CConfigManager *cfgMgr);
	void SaveEncodeConfig(CConfigManager *cfgMgr);
	void SaveEncodeAlarmConfig(CConfigManager *cfgMgr);
	void SaveEncodeNetConfig(CConfigManager *cfgMgr);
	void SaveRecPlanConfig(CConfigManager *cfgMgr);
	void SaveMotionPlanConfig(CConfigManager *cfgMgr);
	void SaveSensorPlanConfig(CConfigManager *cfgMgr);
	void SaveNetConfig(CConfigManager *cfgMgr);
	void SaveDDNSConfig(CConfigManager *cfgMgr);
	void SaveMailConfig(CConfigManager *cfgMgr);
	void SaveUserConfig(CConfigManager *cfgMgr);
	void SaveMotionConfig(CConfigManager *cfgMgr);
	void SaveSensorConfig(CConfigManager *cfgMgr);
	void SaveVlossConfig(CConfigManager *cfgMgr);
	void SavePTZConfig(CConfigManager *cfgMgr);
	void SaveRecordConfig(CConfigManager *cfgMgr);
	void SaveColorConfig(CConfigManager *cfgMgr);

	DECLARE_MESSAGE_MAP()

public:
	void setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonEnterExitConfig();
	afx_msg void OnBnClickedButtonBrowseExportPath();
	afx_msg void OnBnClickedButtonBrowseImportPath();
	afx_msg void OnBnClickedButtonExport();
	afx_msg void OnBnClickedButtonImport();
	afx_msg void OnBnClickedButtonConfigGet();
	afx_msg void OnBnClickedButtonConfigPathBrowse();
	afx_msg void OnBnClickedButtonSelectAll();
	afx_msg void OnBnClickedButtonSelectReverse();
	afx_msg void OnBnClickedButtonConfigDefault();
	afx_msg void OnBnClickedButtonConfigSave();
	afx_msg void OnBnClickedBtnGetdevice();
	afx_msg void OnBnClickedBtnSetdevicename();
	afx_msg void OnBnClickedButtonUpgradePathBrowse();
	afx_msg void OnBnClickedButtonUpgrade();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnGetdeviceipcaminfo();
};
