#pragma once
#include "CDlgAdd.h"
#include "CDlgStatistics.h"
#include "CDlgStat.h"  // 只保留这个统计页面

// CAccountBookDlg 对话框
class CAccountBookDlg : public CDialogEx
{
	// 构造
public:
	CAccountBookDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACCOUNTBOOK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	HICON m_hIcon;

	// 页面容器与子页面
	CStatic m_frame;
	CDlgAdd m_dlgAdd;
	CDlgStat m_dlgStat;
	CWnd* m_pCurrentDlg;

public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnStat();

	DECLARE_MESSAGE_MAP()
};