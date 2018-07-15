// DeviceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "DeviceDlg.h"
#include <string>
#include "resource.h"
using namespace std;

// CDeviceDlg 对话框

IMPLEMENT_DYNAMIC(CDeviceDlg, CDialog)

CDeviceDlg::CDeviceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeviceDlg::IDD, pParent)
{

}

CDeviceDlg::~CDeviceDlg()
{
}

void CDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICE, m_deviceList);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
}


BEGIN_MESSAGE_MAP(CDeviceDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CDeviceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDeviceDlg::OnBnClickedCancel)
	ON_LBN_DBLCLK(IDC_LIST_DEVICE, &CDeviceDlg::OnLbnDblclkListDevice)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PRODUCT_SUB_ID, &CDeviceDlg::OnBnClickedProductSubId)
END_MESSAGE_MAP()


// CDeviceDlg 消息处理程序

void CDeviceDlg::OnBnClickedOk()
{
	m_deviceList.ResetContent();
	CreateThread(NULL, 0, &CDeviceDlg::DiscoverThread, this, 0, NULL);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	m_deviceList.ShowWindow(SW_HIDE);
	m_progress.ShowWindow(SW_SHOW);
	m_curProgress = 0;
	SetTimer(1010, 200, NULL);
}

void CDeviceDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CDeviceDlg::OnLbnDblclkListDevice()
{
	int index = m_deviceList.GetCurSel();
	

	m_Port = m_devs[index].netPort;
	m_IP = m_devs[index].strIP;

	OnOK();
}

void CDeviceDlg::OnTimer(UINT_PTR nIDEvent)
{
	double total = DISCOVER_WAIT_SEC * 1000;
	if (nIDEvent == 1010)
	{
		m_curProgress += (20000.0 ) / total;
		m_progress.SetPos(m_curProgress);
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CDeviceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_progress.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

DWORD WINAPI CDeviceDlg::DiscoverThread( LPVOID param )
{
	CDeviceDlg *pThis = reinterpret_cast<CDeviceDlg *>(param);

	int num = 0;
	if (num = NET_SDK_DiscoverDevice(pThis->m_devs, 100, DISCOVER_WAIT_SEC))
	{
		for (int i = 0; i < num; i++)
		{
			std::string strDeviceType;
			if ( pThis->m_devs[i].deviceType == NET_SDK_DVR)
			{
				strDeviceType = "DVR";
			}
			else if ( pThis->m_devs[i].deviceType == NET_SDK_DVS)
			{
				strDeviceType = "DVS";
			}
			else if (pThis->m_devs[i].deviceType == NET_SDK_IPCAMERA)
			{
				strDeviceType = "IPC";
			}
			else if (pThis->m_devs[i].deviceType == NET_SDK_NVR)
			{
				strDeviceType = "NVR";
			}
			else if (pThis->m_devs[i].deviceType == NET_SDK_DECODER)
			{
				strDeviceType = "DECODER";
			}
			else
			{
				strDeviceType = "other";
			}

			std::string strIP(pThis->m_devs[i].strIP);
			
			strIP = strIP + "__" + strDeviceType;
			pThis->m_deviceList.InsertString(i, strIP.c_str());

			TRACE("ip=%s,mac= %02d:%02d:%02d:%02d:%02d:%02d \n", strIP.c_str(), pThis->m_devs[i].byMac[0], pThis->m_devs[i].byMac[1], pThis->m_devs[i].byMac[2], pThis->m_devs[i].byMac[3],
			pThis->m_devs[i].byMac[4], pThis->m_devs[i].byMac[5]);

		}
	}

	pThis->GetDlgItem(IDOK)->EnableWindow(TRUE);
	pThis->m_deviceList.ShowWindow(SW_SHOW);
	pThis->m_progress.ShowWindow(SW_HIDE);
	pThis->m_curProgress = 0;
	pThis->KillTimer(1010);
	return 0;
}

afx_msg void CDeviceDlg::OnBnClickedProductSubId()
{
	int index = m_deviceList.GetCurSel();
	if (index < 0)
	{
		return ;
	}
	
	DWORD customID = NET_SDK_GetProductSubID(m_devs[index].strIP, m_devs[index].netPort);
	
	CString temp;
	if (customID >= 0 )
	{
		temp.Format("product sub ID %d", customID);
	}
	else
	{
		temp.Format("Failed!");
	}

	AfxMessageBox(temp);
}
