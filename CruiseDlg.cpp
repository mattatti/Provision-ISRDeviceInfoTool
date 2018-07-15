// CruiseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "CruiseDlg.h"
#include "GridCellCombo.h"
#include "DVR_NET_SDK.h"
#include "strdef.h"

// CCruiseDlg 对话框

TCHAR *TimeOptions[] = {_T("5"), _T("10"), _T("15"), _T("20"), _T("30"), _T("60")};

IMPLEMENT_DYNAMIC(CCruiseDlg, CDialog)

CCruiseDlg::CCruiseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCruiseDlg::IDD, pParent)
	, m_cruiseNum(0)
	, m_data(NULL)
{

}

CCruiseDlg::~CCruiseDlg()
{
	if (m_data)
	{
		delete[] m_data;
		m_data = NULL;
	}
}

void CCruiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_CUSTOM_CRUISE, m_grid);
}


BEGIN_MESSAGE_MAP(CCruiseDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCruiseDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCruiseDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CCruiseDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCruiseDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_DELETEALL, &CCruiseDlg::OnBnClickedButtonDeleteall)
END_MESSAGE_MAP()


// CCruiseDlg 消息处理程序

void CCruiseDlg::OnBnClickedOk()
{
	if (m_data)
	{
		delete[] m_data;
		m_data = NULL;
	}

	m_data = new BYTE[sizeof(DD_CRUISE_POINT_INFO) * m_cruiseNum];
	ZeroMemory(m_data, sizeof(DD_CRUISE_POINT_INFO) * m_cruiseNum);
	CGridCellCombo *combo = NULL;
	DD_CRUISE_POINT_INFO *cpi = (DD_CRUISE_POINT_INFO *)m_data;
	CString temp;

	for (int i = 0; i < m_cruiseNum; i++)
	{
		temp = m_grid.GetItemText(i + 1, 0);
		temp = temp.Right(temp.GetLength() - _tcslen(_T("preset ")));
		cpi[i].presetIndex = _tcstol(temp, NULL, 10);
		cpi[i].dwellTime = _tcstol(m_grid.GetItemText(i + 1, 1), NULL, 10);
		cpi[i].dwellSpeed = _tcstol(m_grid.GetItemText(i + 1, 2), NULL, 10);
	}

	OnOK();
}

void CCruiseDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

BOOL CCruiseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_grid.SetColumnCount(3);
	m_grid.SetRowCount(1);
	m_grid.SetItemText(0, 0, STR_PRESET);
	m_grid.SetItemText(0, 1, STR_TIME);
	m_grid.SetItemText(0, 2, STR_SPEED);
	
	CRect GRect;
	m_grid.GetWindowRect(&GRect);               // 得到控件的宽度
	m_grid.SetColumnWidth(0,GRect.Width()/3 - 1); // 设定没列的宽度
	m_grid.SetColumnWidth(1,GRect.Width()/3 - 1);
	m_grid.SetColumnWidth(2,GRect.Width()/3 - 1);
	
	m_grid.SetItemState(0,0,GVIS_READONLY);
	m_grid.SetItemState(0,1,GVIS_READONLY);
	m_grid.SetItemState(0,2,GVIS_READONLY);

	m_grid.SetListMode(TRUE);
	m_grid.SetSingleRowSelection(TRUE);
	m_grid.SetFixedRowCount(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCruiseDlg::OnBnClickedButtonAdd()
{
	if (m_cruiseNum >= 16)
	{
		AfxMessageBox(STR_FULL_16);
		return ;
	}
	m_cruiseNum++;
	m_grid.InsertRow(_T(""));
	m_grid.SetCellType(m_cruiseNum, 0, RUNTIME_CLASS(CGridCellCombo));
	m_grid.SetCellType(m_cruiseNum, 1, RUNTIME_CLASS(CGridCellCombo));
	m_grid.SetCellType(m_cruiseNum, 2, RUNTIME_CLASS(CGridCellCombo));
	m_grid.SetItemText(m_cruiseNum, 0, _T("preset 1"));
	m_grid.SetItemText(m_cruiseNum, 1, _T("5"));
	m_grid.SetItemText(m_cruiseNum, 2, _T("1"));
	CGridCellCombo *combo = NULL;
	CStringArray options;
	CString temp;

	combo = reinterpret_cast<CGridCellCombo *>(m_grid.GetCell(m_cruiseNum, 0));
	for (int i = 0; i < 128; i++)
	{
		temp.Format(_T("preset %d"), i + 1);
		options.Add(temp);
	}
	combo->SetOptions(options);
	combo->SetStyle(combo->GetStyle() | CBS_DROPDOWNLIST);
	options.RemoveAll();

	combo = reinterpret_cast<CGridCellCombo *>(m_grid.GetCell(m_cruiseNum, 1));
	for (int i = 0; i < 6; i++)
	{
		temp = TimeOptions[i];
		options.Add(temp);
	}
	combo->SetOptions(options);
	combo->SetStyle(combo->GetStyle() | CBS_DROPDOWNLIST);
	options.RemoveAll();
	
	combo = reinterpret_cast<CGridCellCombo *>(m_grid.GetCell(m_cruiseNum, 2));
	for (int i = 0; i < 8; i++)
	{
		temp.Format(_T("%d"), i + 1);
		options.Add(temp);
	}
	combo->SetOptions(options);
	combo->SetStyle(combo->GetStyle() | CBS_DROPDOWNLIST);

	m_grid.RedrawRow(m_cruiseNum);
}

void CCruiseDlg::OnBnClickedButtonDelete()
{
	CCellRange range = m_grid.GetSelectedCellRange();
	for (int i = range.GetMinRow(); i <= range.GetMaxRow(); i++)
	{
		if (i != 0)
		{
			m_grid.DeleteRow(i);
			m_cruiseNum--;
		}
	}
	
	m_grid.Refresh();
}

void CCruiseDlg::OnBnClickedButtonDeleteall()
{
	while(m_cruiseNum-- > 0)
	{
		m_grid.DeleteRow(1);
	}
	m_cruiseNum = 0;
	m_grid.Refresh();
}

void CCruiseDlg::GetCruiseInfo( void *data )
{
	memcpy(data, m_data, sizeof(DD_CRUISE_POINT_INFO) * m_cruiseNum);
}
