#pragma once
#include "afxcmn.h"
#include "DVR_NET_SDK.h"
#include "afxdtctl.h"
#include "gridctrl.h"
#include <map>
#include <string>
#include "resource.h"
#include "MsgDlg.h"

#define N9000_CONFIG
// CToolDlg 对话框
using namespace std;
class CToolDlg : public CDialog
{
	DECLARE_DYNAMIC(CToolDlg)

public:
	CToolDlg(CWnd* pParent,  LONG userid, NET_SDK_DEVICEINFO m_deviceInfo);   // 标准构造函数
	virtual ~CToolDlg();

// 对话框数据
	enum { IDD = IDD_TOOL_DIALOG };

protected:
	CListCtrl			m_logList;
	CTime				m_logStartTime;
	CTime				m_logEndTime;
	CTime				m_sysTime;
	BOOL				m_isStartVoiceComm;
	BOOL				m_isOpenMsgCallback;
	BOOL				m_isStartVoiceComm_MR;
	BOOL				m_runTalkTransThread;
	HANDLE				m_talkTransThread;
	HANDLE				m_talkDataFile;
	HANDLE				m_alarmStatusThread;
	LONG				m_voiceHandle;
	LONG				m_talkTransHandle;
	LONG				m_msgCBHandle;
	
	LONG				m_userID;
	int					m_voiceVolume;
	CDateTimeCtrl		m_startTimeCtrl;
	//CDateTimeCtrl		m_endTimeCtrl;
	CDateTimeCtrl		m_systimeCtrl;
	CGridCtrl			m_diskGrid;
	CList<NET_SDK_DISK_INFO> m_diskList;
	CWnd*				m_mainDlg;
	DD_ALARM_STATUS_INFO ec[128];

	std::map<int, string>	m_strMinorLogTypeList;          //n9000用
	void UpdateUI();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	static void CALLBACK TalkTransCallBack(LONG lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, void* pUser);
	static DWORD WINAPI TalkTransSendThread(LPVOID);
	void TalkTransSendProc();

	//void setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid){m_userID = userid; m_deviceinfo = di;}

	//void InitN9000LogTypeList();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSearchlog();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonVoiceComm();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonChangesystime();
	afx_msg void OnBnClickedButtonDiskRefresh();
	afx_msg void OnBnClickedButtonChangeDiskProperty();
	afx_msg void OnBnClickedButtonDiskFormat();
	afx_msg void OnBnClickedButtonMsgCallback();
	void setMainDlg( CWnd* dlg );
	afx_msg void OnBnClickedButtonTalktRans();
	afx_msg void OnBnClickedButtonGetsystime();
	afx_msg void OnBnClickedBtnreboot();
	static DWORD WINAPI AlarmStatusThread( LPVOID params );
	void AlarmStatusProc();
	afx_msg void OnNMCustomdrawSliderVoicecommVolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickerStartTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickerEndTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickerSystime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedLogout();
	

	

public:
	//functions
	void setDeviceInfo();
	CString SetDeviceTime();
	CString SetDeviceModel();
	int SetConIPcams();
	int SetHTTPPort();
	CString SetFirmwareVersion();
	CString SetFirmwareBuildDate();
	CString SetHardwareVersion();
	CString SetLastRecordDate();
	CString SetMACAddress();
	int SetConAnalogcams();
	void checkdisk();
	afx_msg void OnBnClickedRefreshInfo();

private:
	DD_DATE m_lastRecordDate;
	bool build_date_flag;
	NET_SDK_DEVICEINFO	m_deviceinfo;
	CString m_device_time;
	CString m_device_model;
	//int m_con_IP_cams;
	int m_http_port;
	CString m_firmware_version;
	CString m_build_date;
	CString m_hardware_version;
	CString m_last_record_date;
	CString m_MAC_address;
	CString m_NET_SDK_disk_status;
	CString m_disk_Property;

	CString find_date(char * firmware);
	void getLastRecordDate();
	DD_DATE findmaxdate(DD_DATE m_dateList[], int size);
	

	
	
public:
	afx_msg void OnBnClickedButtonAlarmList();




	protected:
		LONG			m_playbackHandle;
	
		LONG			m_channel;
		DD_TIME			m_startTime;
		DD_TIME			m_endTime;
		BOOL			m_isPlay;
		BOOL			m_isOpenAudio;
		BOOL			m_isPause;
		LONG			m_timeElapsed;
		LONG			m_currentSpeed;
		int				m_playbackPos;
		int				m_volume;
		BOOL			m_isDraging;
		BOOL			m_isSavingData;
		HANDLE			m_RecFilehandle;
		BOOL			m_bMultiChannelsPlaybackMode; //是否是多路回放模式
		static void CALLBACK PLayCBFun(LONG lPlayHandle, NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer, void *pUser);

	//	void UpdateUI();


};
