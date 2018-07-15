#pragma once
#include "DVR_NET_SDK.h"

// CPlayBackDlg 对话框

class CPlayBackDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlayBackDlg)

public:
	CPlayBackDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlayBackDlg();

// 对话框数据
	enum { IDD = IDD_PLAYBACK_DIALOG };

protected:
	LONG			m_playbackHandle;
	LONG			m_userID;
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

	void UpdateUI();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	static void CALLBACK PLayCBFun(LONG lPlayHandle, NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer, void *pUser);
public:
	void setPlayBackInfo(LONG userid, DD_TIME start, DD_TIME end, LONG channel)
	{
		m_userID = userid;
		m_startTime = start;
		m_endTime = end;
		m_channel = channel;
		/* time play test
		m_startTime.hour = 10;
		m_startTime.minute = 50;
		m_startTime.second = 0;
		m_endTime.hour = 11;
		m_endTime.minute = 20;
		m_endTime.second = 0;
		*/
	
	}

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonPlayback();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonRew();
	afx_msg void OnBnClickedButtonFf();
	afx_msg void OnBnClickedButtonFrame();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMReleasedcaptureSliderPlaypos(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnLiveAudio();
	afx_msg void OnUpdateLiveAudio(CCmdUI *pCmdUI);
	afx_msg void OnLiveCapture();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnStnClickedStaticSpeed();
	afx_msg void OnChangeSpeed(UINT nID);
	afx_msg void OnUpdateSpeed(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonPlaybacksave();

	static void CALLBACK drawCallBack(LONG lLiveHandle, HDC hDC, void *pUser);

	afx_msg void OnCbnSelchangeComboPlaybackChannels();
};
