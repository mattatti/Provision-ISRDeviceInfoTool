#pragma once
#include "DVR_NET_SDK.h"
#include "afxcmn.h"
#include "MyWaveOut.h"
#include "afxwin.h"
#include <map>

using std::map;

// CLiveDlg 对话框
#define MAX_DIVIDE_NUM		16
#define MAX_CHANNEL_NUM		16

//#define _WRITE_USER_FILE_
class CLiveDlg : public CDialog
{
	DECLARE_DYNAMIC(CLiveDlg)

public:
	CLiveDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLiveDlg();

// 对话框数据
	enum { IDD = IDD_LIVE_DIALOG };

protected:
	map<LONG, NET_SDK_DEVICEINFO *> m_mapDeviceInfo;
	LONG				m_playHandle[MAX_DIVIDE_NUM];
	int					m_playChnn[MAX_DIVIDE_NUM];
	BOOL				m_isStartLive[MAX_DIVIDE_NUM];
	LONG				m_userID;
	BOOL				m_isOpenAudio;
	int					m_audioDivide;
	CBitmap				m_ptzBitmap[9];
	int					m_chnn;
	int					m_brightness;
	int					m_hue;
	int					m_saturation;
	int					m_contrast;
	int					m_volume;
	int					m_currentDivide;
	CSliderCtrl			m_PTZSpeedCtrl;
	BOOL				m_isOpenIris[MAX_CHANNEL_NUM];
	BOOL				m_isOpenRandomScan[MAX_CHANNEL_NUM];	//随机扫描
	BOOL				m_isOpenAutoScan[MAX_CHANNEL_NUM];		//自动扫描
	int					m_presetNo;
	BOOL				m_isTrackRecording[MAX_CHANNEL_NUM];
	BOOL				m_isTrackStart[MAX_CHANNEL_NUM];
	BOOL				m_isCruiseStart[MAX_CHANNEL_NUM];
	int					m_presetSel[MAX_CHANNEL_NUM];
	int					m_cruiseSel[MAX_CHANNEL_NUM];
	int					m_cruiseNo;
	
	BOOL				m_isSingleLive;
	CRect				m_oldRect;

	BOOL				m_isSavingData[MAX_DIVIDE_NUM];

	LONG				m_lAudioDecHandle[MAX_DIVIDE_NUM];
	NET_SDK_STREAM_TYPE	m_eLiveStreamType;
	BOOL				m_bManualAlarm;
	unsigned char*		m_pAudioOutBuffer;

	BOOL				m_bEnablePTZ3DCtrl;	//
	BOOL				m_bPTZ3DCtrling;	//using 3D PTZ control
	int					m_sPointX;
	int					m_sPointY;
	int					m_ePointX;
	int					m_ePointY;
	BOOL				m_bMouseLBDown;
	CComboBox			m_deviceList;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void StartLive();
	void StopLive();

	void StartLiveTest(int chnl);
	void StopLiveTest(int chnl);

	void GetChnnInfo(int chnn);
	void UpdateUI();
	void DrawGrid();

	DECLARE_MESSAGE_MAP()

public:
	void SetDeviceInfo(NET_SDK_DEVICEINFO* di, LONG userId);
	void OnMove(int x, int y);
	void OnPTZCtrl(int id);
	void OnPTZCtrlStop(int id);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonLive();
	afx_msg void OnCbnSelchangeComboChannel();
	afx_msg void OnLiveCapture();
	afx_msg void OnLiveAudio();
	afx_msg void OnUpdateLiveAudio(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedButtonColorDefault();
	afx_msg void OnBnClickedButtonColorSave();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClickedDivide(UINT index);
	afx_msg void OnDBClickedDivide(UINT index);
	afx_msg void OnStnDblclickLiveArea();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonNear();
	afx_msg void OnBnClickedButtonFar();
	afx_msg void OnBnClickedButtonOut();
	afx_msg void OnBnClickedButtonIn();
	afx_msg void OnBnClickedButtonIris();
	afx_msg void OnBnClickedButtonAutoScan();
	afx_msg void OnBnClickedButtonPresetSet();
	afx_msg void OnBnClickedButtonPresetGo();
	afx_msg void OnBnClickedButtonPresetDel();
	afx_msg void OnBnClickedButtonCruiseSetup();
	afx_msg void OnBnClickedButtonTrackRecord();
	afx_msg void OnBnClickedButtonTrack();
	afx_msg void OnBnClickedButtonCruise();
	afx_msg void OnCbnSelchangeComboPreset();
	afx_msg void OnCbnSelchangeComboCruise();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonCapture();
	afx_msg void OnBnClickedButtonLivesave();

	static void CALLBACK LiveCallBack(LONG lLiveHandle, NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer, void *pUser);
	static void CALLBACK drawCallBack(LONG lLiveHandle, HDC hDC, void *pUser);

	HANDLE	m_RecFilehandle[MAX_DIVIDE_NUM];
	bool m_recNeedFormat[MAX_DIVIDE_NUM];
	CMyWaveOut	m_waveOut;
	afx_msg void OnBnClickedButtonManualAram();
	afx_msg void OnBnClickedBtnPtz3dCtrl();
	afx_msg void OnBnClickedButtonCaptureJpeg();
	afx_msg void OnBnClickedBtnMakeiframe();
	afx_msg void OnBnClickedButtonAutoScan2();
};
