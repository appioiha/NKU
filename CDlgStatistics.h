#pragma once
#include "afxdialogex.h"


// CDlgStatistics 对话框

class CDlgStatistics : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStatistics)

public:
	CDlgStatistics(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgStatistics();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_STATISTICS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStaticPie();
};
