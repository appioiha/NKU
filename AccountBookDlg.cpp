#include "pch.h"
#include "framework.h"
#include "AccountBook.h"
#include "AccountBookDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CAccountBookDlg::CAccountBookDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ACCOUNTBOOK_DIALOG, pParent)
	, m_pCurrentDlg(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAccountBookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAME, m_frame);
}

BEGIN_MESSAGE_MAP(CAccountBookDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ADD, &CAccountBookDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_STAT, &CAccountBookDlg::OnBnClickedBtnStat)
END_MESSAGE_MAP()

BOOL CAccountBookDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CRect rect;
	m_frame.GetWindowRect(rect);
	ScreenToClient(rect);

	// ========== 初始化记账页面 ==========
	m_dlgAdd.Create(IDD_DLG_ADD, &m_frame);
	m_dlgAdd.SetWindowPos(nullptr, 0, 0, rect.Width(), rect.Height(), SWP_NOZORDER);
	m_dlgAdd.ShowWindow(SW_SHOW);
	m_pCurrentDlg = &m_dlgAdd;

	// ========== 初始化统计页面 ==========
	m_dlgStat.Create(IDD_DLG_STATISTICS, &m_frame);
	m_dlgStat.SetWindowPos(nullptr, 0, 0, rect.Width(), rect.Height(), SWP_NOZORDER);
	m_dlgStat.ShowWindow(SW_HIDE);

	return TRUE;
}

void CAccountBookDlg::OnBnClickedBtnAdd()
{
	// 切换到记账页面
	m_pCurrentDlg->ShowWindow(SW_HIDE);
	m_dlgAdd.ShowWindow(SW_SHOW);
	m_pCurrentDlg = &m_dlgAdd;
}

void CAccountBookDlg::OnBnClickedBtnStat()
{
	// 切换到统计页面
	m_pCurrentDlg->ShowWindow(SW_HIDE);
	m_dlgStat.ShowWindow(SW_SHOW);
	m_pCurrentDlg = &m_dlgStat;
}

// 以下是系统自带函数，不动
void CAccountBookDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void CAccountBookDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CAccountBookDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}