// PlayBackDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "PlayBackDlg.h"
#include <cmath>
#include "strdef.h"

// CPlayBackDlg 对话框

IMPLEMENT_DYNAMIC(CPlayBackDlg, CDialog)

CPlayBackDlg::CPlayBackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayBackDlg::IDD, pParent)
	, m_isPlay(FALSE)
	, m_playbackHandle(-1)
	, m_timeElapsed(0)
	, m_currentSpeed(NET_SDK_RPB_SPEED_1X)
	, m_playbackPos(0)
	, m_volume(0)
	, m_isOpenAudio(FALSE)
	, m_isPause(FALSE)
	, m_isDraging(FALSE)
	, m_isSavingData(FALSE)
	, m_bMultiChannelsPlaybackMode(FALSE)
{

}

CPlayBackDlg::~CPlayBackDlg()
{
}

void CPlayBackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_SLIDER_PLAYPOS, m_playbackPos);
	DDX_Slider(pDX, IDC_SLIDER_VOLUME, m_volume);
}


BEGIN_MESSAGE_MAP(CPlayBackDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPlayBackDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPlayBackDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_PLAYBACK, &CPlayBackDlg::OnBnClickedButtonPlayback)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CPlayBackDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_REW, &CPlayBackDlg::OnBnClickedButtonRew)
	ON_BN_CLICKED(IDC_BUTTON_FF, &CPlayBackDlg::OnBnClickedButtonFf)
	ON_BN_CLICKED(IDC_BUTTON_FRAME, &CPlayBackDlg::OnBnClickedButtonFrame)
	ON_WM_HSCROLL()
	ON_WM_MOVE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PLAYPOS, &CPlayBackDlg::OnNMReleasedcaptureSliderPlaypos)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_LIVE_AUDIO, &CPlayBackDlg::OnLiveAudio)
	ON_UPDATE_COMMAND_UI(ID_LIVE_AUDIO, &CPlayBackDlg::OnUpdateLiveAudio)
	ON_COMMAND(ID_LIVE_CAPTURE, &CPlayBackDlg::OnLiveCapture)
	ON_COMMAND_RANGE(ID_PLAYBACKSPEED_1_32x, ID_PLAYBACKSPEED_32X, &CPlayBackDlg::OnChangeSpeed)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PLAYBACKSPEED_1_32x, ID_PLAYBACKSPEED_32X, &CPlayBackDlg::OnUpdateSpeed)
	ON_WM_CONTEXTMENU()
	ON_STN_CLICKED(IDC_STATIC_SPEED, &CPlayBackDlg::OnStnClickedStaticSpeed)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_PLAYBACKSAVE, &CPlayBackDlg::OnBnClickedButtonPlaybacksave)
	ON_CBN_SELCHANGE(IDC_COMBO_Playback_Channels, &CPlayBackDlg::OnCbnSelchangeComboPlaybackChannels)
END_MESSAGE_MAP()


// CPlayBackDlg 消息处理程序

void CPlayBackDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CPlayBackDlg::OnBnClickedCancel()
{
	KillTimer(2020);
	if (m_playbackHandle > 0)
	{
		NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_STOPAUDIO, 0, NULL);
		NET_SDK_StopPlayBack(m_playbackHandle);
		m_playbackHandle = -1;
	}
	m_isPlay = FALSE;
	OnCancel();
}

#define MULTI_CHANNEL_REPLAY
void CPlayBackDlg::OnBnClickedButtonPlayback()
{
	if (m_isPlay && !m_isPause)
	{
		BOOL ret = NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_PAUSE, 0, NULL);

		m_isPause = TRUE;
	}
	else
	{
		if (m_playbackHandle < 0)
		{
			CComboBox* pComChannels = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_Playback_Channels));
			if (pComChannels)
			{
				m_bMultiChannelsPlaybackMode = (pComChannels->GetCurSel() > 0) ? (TRUE) : (FALSE);
			}
			else
			{
				m_bMultiChannelsPlaybackMode = FALSE;
			}
			
			LONG handle = 0;
			if (m_bMultiChannelsPlaybackMode)
			{
				LONG ch[32] = {m_channel, m_channel+1, m_channel+2, m_channel+3};
				HWND arrHwnd[32] = {0};
				arrHwnd[0] = GetDlgItem(IDC_PLAYBACK_AREA)->GetSafeHwnd();
				arrHwnd[1] = GetDlgItem(IDC_PLAYBACK_AREA2)->GetSafeHwnd();
				arrHwnd[2] = GetDlgItem(IDC_PLAYBACK_AREA3)->GetSafeHwnd();
				arrHwnd[3] = GetDlgItem(IDC_PLAYBACK_AREA4)->GetSafeHwnd();

				handle = NET_SDK_PlayBackByTime(m_userID, ch, 4, &m_startTime, &m_endTime, arrHwnd);
			}
			else
			{
				HWND wnd = GetDlgItem(IDC_PLAYBACK_AREA)->GetSafeHwnd();
			//	handle = NET_SDK_PlayBackByTime(m_userID, &m_channel, 1, &m_startTime, &m_endTime, &wnd);
				handle = NET_SDK_PlayBackByTimeEx(m_userID, &m_channel, 1, &m_startTime, &m_endTime, &wnd, TRUE);
			}

			if (handle > 0)
			{
				NET_SDK_RegisterDrawFun(handle, &CPlayBackDlg::drawCallBack, this);
				SetTimer(2020, 1000, NULL);
				m_playbackHandle = handle;
				m_isPlay = TRUE;
				m_isPause = FALSE;
				m_timeElapsed = 0;
				NET_SDK_SetPlayDataCallBack(handle, PLayCBFun, this);
			}
			else
			{
				if (NET_SDK_GetLastError() == NET_SDK_DVR_NORESOURCE)
				{
					AfxMessageBox(_T("No resource!"));
				}
				else
				{
					AfxMessageBox(_T("Faild!"));
				}

			}
		}
		else
		{
			NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_RESUME, 0, NULL);
			m_isPlay = TRUE;
			m_isPause = FALSE;
		}
		m_currentSpeed = NET_SDK_RPB_SPEED_1X;
	}
	UpdateUI();
}

void CPlayBackDlg::OnBnClickedButtonStop()
{
	KillTimer(2020);
	NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_STOPAUDIO, 0, NULL);
	NET_SDK_StopPlayBack(m_playbackHandle);
	m_playbackHandle = -1;
	m_isPlay = FALSE;
	
	GetDlgItem(IDC_PLAYBACK_AREA)->Invalidate();
	GetDlgItem(IDC_PLAYBACK_AREA2)->Invalidate();
	GetDlgItem(IDC_PLAYBACK_AREA3)->Invalidate();
	GetDlgItem(IDC_PLAYBACK_AREA4)->Invalidate();
	UpdateUI();
	m_playbackPos = 0;
	UpdateData(FALSE);
}

void CPlayBackDlg::OnBnClickedButtonRew()
{
	NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_REW, m_currentSpeed, NULL);
	UpdateUI();
}

void CPlayBackDlg::OnBnClickedButtonFf()
{
	NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_FF, m_currentSpeed, NULL);
	UpdateUI();
}

void CPlayBackDlg::OnBnClickedButtonFrame()
{
	NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_FRAME, 0, NULL);
}

void CPlayBackDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData();

	CPoint point;
	GetCursorPos(&point);
	CRect rect;
	GetDlgItem(IDC_SLIDER_VOLUME)->GetWindowRect(&rect);

	if (rect.PtInRect(point))
	{
		unsigned short volume = 0;
		double temp = (((double)m_volume) / 100) * 0xffff;
		volume = (short )temp;
		NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_AUDIOVOLUME, volume, NULL);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPlayBackDlg::UpdateUI()
{
	GetDlgItem(IDC_BUTTON_REW)->EnableWindow(m_isPlay && !m_isPause);
	GetDlgItem(IDC_BUTTON_FF)->EnableWindow(m_isPlay && !m_isPause);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_isPlay);
	GetDlgItem(IDC_BUTTON_FRAME)->EnableWindow(m_isPause);
	GetDlgItem(IDC_SLIDER_VOLUME)->EnableWindow(m_isPlay);
	
	if (m_isPlay && !m_isPause)
	{
		SetDlgItemText(IDC_BUTTON_PLAYBACK, _T("||"));
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_PLAYBACK, _T(">"));
	}
	
	CString temp;
	switch(m_currentSpeed)
	{
	case NET_SDK_RPB_SPEED_1_32X:
		temp = _T("1/32x");
		break;
	case NET_SDK_RPB_SPEED_1_16X:
		temp = _T("1/16x");
		break;
	case NET_SDK_RPB_SPEED_1_8X:
		temp = _T("1/8x");
		break;
	case NET_SDK_RPB_SPEED_1_4X:
		temp = _T("1/4x");
		break;
	case NET_SDK_RPB_SPEED_1_2X:
		temp = _T("1/2x");
		break;
	case NET_SDK_RPB_SPEED_1X:
		temp = _T("1x");
		break;
	case NET_SDK_RPB_SPEED_2X:
		temp = _T("2x");
		break;
	case NET_SDK_RPB_SPEED_4X:
		temp = _T("4x");
		break;
	case NET_SDK_RPB_SPEED_8X:
		temp = _T("8x");
		break;
	case NET_SDK_RPB_SPEED_16X:
		temp = _T("16x");
		break;
	case NET_SDK_RPB_SPEED_32X:
		temp = _T("32x");
		break;
	}
	SetDlgItemText(IDC_STATIC_SPEED, temp);

	if (m_isSavingData)
	{
		SetDlgItemText(IDC_BUTTON_PLAYBACKSAVE, _T("stop"));
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_PLAYBACKSAVE, _T("save"));
	}
}

BOOL CPlayBackDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	UpdateUI();

	m_volume = 50;

	UpdateData(FALSE);

	CComboBox* pComChannels = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_Playback_Channels));
	if (pComChannels)
	{
		pComChannels->AddString(_T("1"));
		pComChannels->AddString(_T("4"));
		pComChannels->SetCurSel(0);
	}
	GetDlgItem(IDC_PLAYBACK_AREA2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PLAYBACK_AREA3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PLAYBACK_AREA4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PLAYBACK_AREA)->MoveWindow(12, 10, 766, 535);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPlayBackDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
}

void CPlayBackDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 202)
	{
		UpdateData();
		DD_TIME t = {0};
		BOOL ret = NET_SDK_GetPlayBackOsdTime(m_playbackHandle, &t);

		if (!ret)
		{
			//KillTimer(2020);
			return;
		}

		CTime now(t.year + 1900, t.month + 1, t.mday, t.hour, t.minute, t.second);
		CTime start(m_startTime.year + 1900, m_startTime.month + 1, m_startTime.mday, m_startTime.hour,
			m_startTime.minute, m_startTime.second);
		CTime end(m_endTime.year + 1900, m_endTime.month + 1, m_endTime.mday, m_endTime.hour,
			m_endTime.minute, m_endTime.second);

		int pos = ((now.GetTime() - start.GetTime()) * 100)/ (end.GetTime() - start.GetTime());
		
		if (abs(pos - m_playbackPos) <= 5)
		{
			KillTimer(202);
			SetTimer(2020, 1000, NULL);
		}
	}
	else if (nIDEvent == 2020 && !m_isDraging) //播放进度
	{
		if (m_isPlay)
		{	
			CRect rect;
			GetDlgItem(IDC_PLAYBACK_AREA)->GetWindowRect(&rect);
			CPoint point;
			GetCursorPos(&point);
			if (rect.PtInRect(point) && (GetAsyncKeyState(VK_LBUTTON) >> 14))
			{
				return;
			}

			DD_TIME t = {0};
			BOOL ret = NET_SDK_GetPlayBackOsdTime(m_playbackHandle, &t);
			
			if (!ret)
			{
				//KillTimer(2020);
				if (m_playbackPos >= 100 || m_playbackPos < 0)
				{
					OnBnClickedButtonStop();
				}
				return;
			}

			CTime now(t.year + 1900, t.month + 1, t.mday, t.hour, t.minute, t.second);
			CTime start(m_startTime.year + 1900, m_startTime.month + 1, m_startTime.mday, m_startTime.hour,
				m_startTime.minute, m_startTime.second);
			CTime end(m_endTime.year + 1900, m_endTime.month + 1, m_endTime.mday, m_endTime.hour,
				m_endTime.minute, m_endTime.second);

			m_playbackPos = ((now.GetTime() - start.GetTime()) * 100)/ (end.GetTime() - start.GetTime());
			UpdateData(FALSE);
// 			if (m_playbackPos > 100 || m_playbackPos < 0)  //这里会导致提前关闭
// 			{
// 				OnBnClickedButtonStop();
// 			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CPlayBackDlg::OnNMReleasedcaptureSliderPlaypos(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData();

	CTime start(m_startTime.year + 1900, m_startTime.month + 1, m_startTime.mday, m_startTime.hour,
		m_startTime.minute, m_startTime.second);
	CTime end(m_endTime.year + 1900, m_endTime.month + 1, m_endTime.mday, m_endTime.hour,
		m_endTime.minute, m_endTime.second);

	LONG pos = ((end.GetTime() - start.GetTime()) * m_playbackPos) / 100 + start.GetTime();

	KillTimer(2020);
	NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_SETPOS, pos, NULL);
	SetTimer(202, 100, NULL);
	*pResult = 0;
}

void CPlayBackDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
		//CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: Add your message handler code here
	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}

}

void CPlayBackDlg::OnLiveAudio()
{
	if (m_isOpenAudio)
	{
		NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_STOPAUDIO, NET_SDK_RPB_SPEED_1X, NULL);
		m_isOpenAudio = FALSE;
	}	
	else
	{
		unsigned short volume = 0;
		double temp = (((double)m_volume) / 100) * 0xffff;
		volume = (short )temp;
		UpdateData();
		NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_STARTAUDIO, NET_SDK_RPB_SPEED_1X, NULL);
		NET_SDK_PlayBackControl(m_playbackHandle, NET_SDK_PLAYCTRL_AUDIOVOLUME, volume, NULL);
		m_isOpenAudio = TRUE;
	}
}

void CPlayBackDlg::OnUpdateLiveAudio(CCmdUI *pCmdUI)
{
	if (m_isOpenAudio)
	{
		pCmdUI->SetText(STR_CLOSE_AUDIO);
	}
	else
	{
		pCmdUI->SetText(STR_START_AUDIO);
	}
}

void CPlayBackDlg::OnLiveCapture()
{
	CFileDialog dlg(FALSE, _T("bmp"), _T("sample.bmp"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("bmp files(*.bmp)|*.bmp||"));
	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		BOOL ret = NET_SDK_PlayBackCaptureFile(m_playbackHandle, m_channel, pathName.GetBuffer());
		if (!ret)
		{
			AfxMessageBox(STR_SNAP_FAILED);
		}
	}
}

void CPlayBackDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rect;
	GetDlgItem(IDC_PLAYBACK_AREA)->GetWindowRect(&rect);\
	if (m_isPlay && rect.PtInRect(point))
	{
		CPoint p;
		GetCursorPos(&p);

		CMenu menu;
		menu.LoadMenu(IDR_MENU_POPUP);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
	}
}

void CPlayBackDlg::OnStnClickedStaticSpeed()
{
	CPoint point;
	GetCursorPos(&point);

	CMenu menu;
	menu.LoadMenu(IDR_MENU_POPUP);
	menu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CPlayBackDlg::OnChangeSpeed(UINT nID)
{
	m_currentSpeed = NET_SDK_RPB_SPEED_1_32X + nID - ID_PLAYBACKSPEED_1_32x;
	UpdateUI();
}

void CPlayBackDlg::OnUpdateSpeed( CCmdUI *pCmdUI )
{
	if (pCmdUI->m_nID == (m_currentSpeed - NET_SDK_RPB_SPEED_1_32X) + ID_PLAYBACKSPEED_1_32x)
	{
		pCmdUI->SetCheck(TRUE);
	}
}

BOOL CPlayBackDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_SLIDER_PLAYPOS)->GetSafeHwnd())
		{
			m_isDraging = TRUE;
		}
	}
	if (pMsg->message == WM_LBUTTONUP)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_SLIDER_PLAYPOS)->GetSafeHwnd())
		{
			m_isDraging = FALSE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CPlayBackDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (m_isPlay && m_isPause)
	{
		BOOL ret = NET_SDK_RefreshPlay(m_playbackHandle);
	}
	// 不为绘图消息调用 CDialog::OnPaint()
}

void CPlayBackDlg::OnBnClickedButtonPlaybacksave()
{
	if (m_isSavingData)
	{
		BOOL ret = NET_SDK_StopPlayBackSave(m_playbackHandle, m_channel);
		if (ret)
		{
			m_isSavingData = FALSE;
		}
	}
	else
	{
		CFileDialog dlg(FALSE, _T(".avi"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("avi files(*.avi)|*.avi||"));
		if (IDOK == dlg.DoModal())
		{

			BOOL ret = NET_SDK_PlayBackSaveData(m_playbackHandle, m_channel, dlg.GetPathName().GetBuffer());
			if (ret)
			{
				m_isSavingData = TRUE;
			}
			else
			{
				AfxMessageBox(_T("ERROR!"));
			}
		}
	}
	UpdateUI();
}

void CALLBACK CPlayBackDlg::drawCallBack( LONG lLiveHandle, HDC hDC, void *pUser )
{
	CPlayBackDlg *pThis = reinterpret_cast<CPlayBackDlg *>(pUser);

	TextOut(hDC, 10, 10, _T("OH YEAH! OH NO! OH MY GOD!"), _tcslen(_T("OH YEAH! OH NO! OH MY GOD!")));
}

void CALLBACK CPlayBackDlg::PLayCBFun(LONG lPlayHandle, NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer, void *pUser)
{
	//TRACE("frameType:%d, dataLen:%d\n", frameInfo.frameType, frameInfo.length);
}

void CPlayBackDlg::OnCbnSelchangeComboPlaybackChannels()
{
	CComboBox* pComChannels = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_Playback_Channels));
	if (pComChannels)
	{
		if (pComChannels->GetCurSel() > 0)
		{
			GetDlgItem(IDC_PLAYBACK_AREA)->MoveWindow(12, 10, 381, 268);
			GetDlgItem(IDC_PLAYBACK_AREA2)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_PLAYBACK_AREA3)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_PLAYBACK_AREA4)->ShowWindow(SW_SHOW);
		}
		else
		{
			GetDlgItem(IDC_PLAYBACK_AREA2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_PLAYBACK_AREA3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_PLAYBACK_AREA4)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_PLAYBACK_AREA)->MoveWindow(12, 10, 766, 535);
		}
		this->Invalidate();	
	}
}
