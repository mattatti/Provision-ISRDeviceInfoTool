// MyWaveOut.cpp: implementation of the CMyWaveOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyWaveOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyWaveOut::CMyWaveOut()
{
	m_iBufIndex=0;
	m_hWaveOut=NULL;
}

BOOL CMyWaveOut::Start(WAVEFORMATEX wavf)
{
	m_SafeLock.Lock();
	m_WaveFormat = wavf;
	if(MMSYSERR_NOERROR != waveOutOpen(&m_hWaveOut,WAVE_MAPPER,&wavf,(DWORD)waveOutProc,(DWORD)this,CALLBACK_FUNCTION))
	{
		m_hWaveOut=NULL;
		m_SafeLock.Unlock();
		return FALSE;
	}
	for(int i=0; i<MAX_BUF; i++)
	{
		m_WaveHeader[i].lpData=new char[AUDIO_BUF_LEN];
		m_WaveHeader[i].dwBufferLength=AUDIO_BUF_LEN;
		m_WaveHeader[i].dwBytesRecorded=0;
		m_WaveHeader[i].dwLoops=1;
		m_WaveHeader[i].dwUser=0;
		m_WaveHeader[i].dwFlags =WHDR_DONE ;
		waveOutPrepareHeader(m_hWaveOut,&m_WaveHeader[i],sizeof(WAVEHDR));
	}
	m_SafeLock.Unlock();
	return TRUE;
}

CMyWaveOut::~CMyWaveOut()
{
	Stop();
}

void CALLBACK CMyWaveOut::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	//m_SafeLock.Lock();
	switch(uMsg)
	{
	case WOM_DONE:
		{
			WAVEHDR *pHeader=(WAVEHDR*)dwParam1;
			pHeader->dwUser = 0;
		}
		break;
	}
	//m_SafeLock.Unlock();
}

void CMyWaveOut::WriteBuf(BYTE *pByte, DWORD len)
{
	m_SafeLock.Lock();
	if((m_hWaveOut==NULL)||(len>m_WaveFormat.nAvgBytesPerSec)) 
	{
		m_SafeLock.Unlock();
		return;
	}
	if(m_WaveHeader[m_iBufIndex].dwUser==0)
	{
		memcpy(m_WaveHeader[m_iBufIndex].lpData,pByte,len);
		m_WaveHeader[m_iBufIndex].dwBufferLength =len;
		m_WaveHeader[m_iBufIndex].dwUser=1;
		waveOutWrite (m_hWaveOut,&(m_WaveHeader[m_iBufIndex]),sizeof( WAVEHDR));
		m_iBufIndex++;
		if(m_iBufIndex>=MAX_BUF)
			m_iBufIndex=0;
	}
	m_SafeLock.Unlock();
}

/**********************************************************************
* 函数名称：  SetVolume
* 功能描述：  设置音频的音量
* 输入参数：  dwVolume:音量值
* 输出参数：  无
* 返 回 值：  无 
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
***********************************************************************/
BOOL CMyWaveOut::SetVolume(DWORD dwVolume)
{
//	m_SafeLock.Lock();
	if ( (0x00000000 <= dwVolume) && (dwVolume <= 0xFFFFFFFF) )
	{
		if(m_hWaveOut != NULL)
		{
			waveOutSetVolume(m_hWaveOut, dwVolume);
			return TRUE;
		}
	}
	return FALSE;
//	m_SafeLock.Unlock();
}//SetVolume end.


DWORD CMyWaveOut::GetVoume()
{
	DWORD dwVolume;
	if(m_hWaveOut != NULL)
	{
		if(MMSYSERR_NOERROR == waveOutGetVolume(m_hWaveOut, &dwVolume))
		{
			return dwVolume;
		}
	}

	return 0;
}


/**********************************************************************
* 函数名称：  SetPlaybackRate
* 功能描述：  设置音频播放速度
* 输入参数：  dwRate:速率
* 输出参数：  无
* 返 回 值：  无 
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
***********************************************************************/
void CMyWaveOut::SetPlaybackRate(DWORD dwRate)
{
}//SetPlaybackRate end.


/**********************************************************************
* 函数名称：  Stop
* 功能描述：  停止播放音频
* 输入参数：  无
* 输出参数：  无
* 返 回 值：  无 
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
***********************************************************************/
void CMyWaveOut::Stop()
{
	m_SafeLock.Lock();
	if(m_hWaveOut)
	{
		waveOutReset(m_hWaveOut);
		for(int i=0;i<MAX_BUF;i++)
		{
			waveOutUnprepareHeader(m_hWaveOut,&m_WaveHeader[i],sizeof(WAVEHDR));
			delete [] m_WaveHeader[i].lpData;
			m_WaveHeader[i].lpData = NULL;
		}
		waveOutClose(m_hWaveOut);
		m_hWaveOut = NULL;
	}
	m_SafeLock.Unlock();

}//Stop end.
