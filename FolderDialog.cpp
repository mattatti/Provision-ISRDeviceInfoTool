
/* 
DESCRIPTION:
	CFolderDialog  - Folder Selection Dialog Class

NOTES:
	Copyright(C) Armen Hakobyan, 2002
	mailto:armenh@cit.am
	
VERSION HISTORY:
	24 Mar 2002 - First release
*/

#include "stdafx.h"
#include "FolderDialog.h"
#include "driveinfo.h"

#include "resource.h"

#ifndef BFFM_VALIDATEFAILED
#ifndef UNICODE
#define BFFM_VALIDATEFAILED 3
#else
#define BFFM_VALIDATEFAILED 4	
#endif
#endif //!BFFM_VALIDATEFAILED

// CFolderDialog

IMPLEMENT_DYNAMIC(CFolderDialog, CDialog)
CFolderDialog::CFolderDialog(LPCTSTR lpszTitle /*NULL*/,
							 LPCTSTR lpszFolderName /*NULL*/,
							 CWnd* pParentWnd /*NULL*/,
							 UINT uFlags /*BIF_RETURNONLYFSDIRS*/)
			 : CCommonDialog(pParentWnd)
			 , m_hWnd(NULL)
{
	m_szSelectedPath[0]	= '\0';
	m_szFolderPath[0]	= '\0';
	m_szFolderDisplayName[0]= '\0';
	
	if(lpszFolderName != NULL && lstrlen(lpszFolderName))
	{
		lstrcpy(m_szSelectedPath, lpszFolderName);
	}
	else
	{
		lstrcpy(m_szSelectedPath, _T(" "));
	}
	
	// Fill
	::ZeroMemory(&m_bi, sizeof(BROWSEINFO)); 
	m_bi.hwndOwner = pParentWnd->GetSafeHwnd();
	m_bi.pidlRoot = NULL;
	m_bi.pszDisplayName = m_szFolderDisplayName;
	m_bi.lpszTitle = m_szSelectedPath;
	m_bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	m_bi.lpfn = BrowseCallbackProc;
	m_bi.lParam = (LPARAM)this;
}

CFolderDialog::~CFolderDialog(void)
{
}

BEGIN_MESSAGE_MAP(CFolderDialog, CCommonDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// CFolderDialog message handlers

int CFolderDialog::DoModal(void)
{
	ASSERT_VALID(this);	
	ASSERT(m_bi.lpfn != NULL);

	CoInitialize(NULL);
		
	m_bi.hwndOwner = PreModal();	
	int nRet   = -1;
	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder(&m_bi);

	if(pItemIDList != NULL)
	{
		if(::SHGetPathFromIDList(pItemIDList, m_szFolderPath))
		{
			IMalloc* pMalloc = NULL;
			if(SUCCEEDED(::SHGetMalloc(&pMalloc)))
			{
				pMalloc->Free(pItemIDList);
				pMalloc->Release();
			}
			nRet = IDOK;
		}
		else
		{
			nRet = IDCANCEL;
		}
	}

	PostModal();

	CoUninitialize();

	return nRet;	
}

// Overridables:

void CFolderDialog::OnInitialized(void)
{	
	HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);
	while(NULL != hChild)
	{
		TCHAR szClass[256];
		GetClassName(hChild, szClass, 255);

		if (strcmp(szClass, "Edit") == 0)
		{
			m_hWndEdit = hChild;
		}
		else if(strcmp(szClass, "Static") == 0)
		{
			::GetWindowText(hChild, m_szFolderPath, MAX_PATH);
			if(strlen(m_szFolderPath) > 0)
			{
				m_hWndTitle = hChild;
			}				
		}

		hChild = ::GetNextWindow(hChild, GW_HWNDNEXT);
	}

	if(lstrlen(m_szSelectedPath))
		SetSelection(m_szSelectedPath);
}

int CFolderDialog::OnValidateFailed(LPCTSTR /*lpstrFolderPath*/)
{
	int nRet = IDYES;//AfxMessageBox(_T("The path entered is not valid! Continue ?"), 
					//MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
	
	// Return 1 = continue, 0 = EndDialog
	return((nRet == IDYES) ? 0 : 1);
}

void CFolderDialog::OnSelChanged(LPITEMIDLIST pItemIDList)
{
	CString sStatusText, sPath;

	SHGetPathFromIDList(pItemIDList, sPath.GetBuffer(MAX_PATH));
	sPath.ReleaseBuffer();

	DWORD dwAttrib = GetFileAttributes(sPath);
	BOOL bEnable = (dwAttrib != 0xffffffff);

	if (!bEnable)
		sStatusText = "< Please select a valid folder >";

	if (bEnable && (m_bi.ulFlags & BIF_BROWSENONETWORK))
	{
		bEnable = !CDriveInfo::IsRemotePath(sPath);
		sStatusText = bEnable ? "" : "< Please select a folder on a local drive >";
	}

	if (bEnable && (m_bi.ulFlags & BIF_BROWSEFIXEDONLY))
	{
		bEnable = CDriveInfo::IsFixedPath(sPath);
		sStatusText = bEnable ? "" : "< Please select a folder on a non-removeable drive >";
	}

	if (bEnable && (m_bi.ulFlags & BIF_BROWSENOREADONLY))
	{
		bEnable = !CDriveInfo::IsReadonlyPath(sPath);
		sStatusText = bEnable ? "" : "< Please select a non-readonly folder >";
	}

	if (bEnable && (m_bi.ulFlags & BIF_BROWSENOROOTDIR))
	{
		bEnable = sPath.GetLength() > 3;
		sStatusText = bEnable ? "" : "< Please select a non-root folder >";
	}

	EnableOK(bEnable);
	SetStatusText(sStatusText);

	::SetWindowText(m_hWndTitle, sPath);
}

// Callback function used with the SHBrowseForFolder function. 

INT CALLBACK CFolderDialog::BrowseCallbackProc(HWND hWnd, 
							UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	CFolderDialog* pThis = (CFolderDialog*)(lpData);
	pThis->m_hWnd = hWnd;
	int nRet = 0;

	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		pThis->OnInitialized();
		break;
	case BFFM_SELCHANGED:
		pThis->OnSelChanged((LPITEMIDLIST)lParam);
		break;
	case BFFM_VALIDATEFAILED:
		nRet = pThis->OnValidateFailed((LPCTSTR)lParam);
		break;
	}

	pThis->m_hWnd = NULL;

	return nRet;	
}

void CFolderDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CCommonDialog::OnPaint()
#if 0
	CDC* pMemDC = new CDC;
	pMemDC->CreateCompatibleDC(&dc);

	//dc.FillRect(&rect, &m_bkBrush);
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP_BK);
	BITMAP bmpInfo;
	bitmap.GetObject(sizeof(bmpInfo), &bmpInfo);

	pMemDC->SelectObject(bitmap);
	CRect rect;
	GetClientRect(&rect);

	dc.StretchBlt(0, 45, rect.Width(), rect.Height(), pMemDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);
	delete pMemDC;
#else
	//CRect rect;
	//GetClientRect(&rect);
	//dc.FillRect(&rect, &m_bkBrush);

	//rect.top = 45;
//	DrawGradient(&dc,rect,RGB(255, 255, 255),RGB(201, 209, 254),FALSE,TRUE);
#endif
}

BOOL CFolderDialog::OnInitDialog()
{
	CCommonDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	VERIFY(m_bkBrush.CreateSolidBrush(RGB(255, 255, 255)));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFolderDialog::OnDestroy()
{
	CCommonDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_bkBrush.DeleteObject();
}

HBRUSH CFolderDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CCommonDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	/*if (CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SetBkMode(TRANSPARENT);

		return m_bkBrush;
	}*/

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
