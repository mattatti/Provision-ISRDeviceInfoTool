// MyWaveOut.h: interface for the CMyWaveOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYWAVEOUT_H__F7B4B1FB_785A_433F_92E1_68AF9414A9C0__INCLUDED_)
#define AFX_MYWAVEOUT_H__F7B4B1FB_785A_433F_92E1_68AF9414A9C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "mmsystem.h"
#include "Afxmt.h"

const int MAX_BUF = 30;
const int AUDIO_BUF_LEN = 100*1024;
class CMyWaveOut  
{
public:
	void WriteBuf(BYTE *pByte,DWORD len);
	BOOL SetVolume(DWORD dwVolume);
	DWORD GetVoume();
	void SetPlaybackRate(DWORD dwRate);
	void Stop();
	BOOL Start(WAVEFORMATEX wavf);

	CMyWaveOut();
	virtual ~CMyWaveOut();
private:
	WAVEHDR m_WaveHeader[MAX_BUF];
	HWAVEOUT m_hWaveOut;
	int m_iBufIndex;

	WAVEFORMATEX m_WaveFormat;
	static void CALLBACK waveOutProc(HWAVEOUT hwo,UINT uMsg, DWORD dwInstance,DWORD dwParam1,DWORD dwParam2);


	CCriticalSection   m_SafeLock;
};

#endif // !defined(AFX_MYWAVEOUT_H__F7B4B1FB_785A_433F_92E1_68AF9414A9C0__INCLUDED_)
