#pragma once
#include "Resource.h"
#include "afxdtctl.h"
#include "afxwin.h"
#include <afxtempl.h>

struct Record
{
    CString type;
    CString date;
    CString cate;
    CString money;
};

class CDlgStat : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgStat)

public:
    CDlgStat(CWnd* pParent = nullptr);
    enum { IDD = IDD_DLG_STATISTICS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

public:
    CDateTimeCtrl m_dtStart;
    CDateTimeCtrl m_dtEnd;
    CListCtrl m_list;
    CStatic m_staticIncome;
    CStatic m_staticOutcome;
    CStatic m_staticBalance;
    CStatic m_staticPie;

    void InitList();
    void LoadAllRecords(CArray<Record, Record>& arr);
    void Query();
    void DrawPie(CPaintDC& dc, int cateCount[6]);

    afx_msg void OnBnClickedBtnQuery();
    afx_msg void OnPaint();
};