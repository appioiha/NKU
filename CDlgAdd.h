#pragma once
#include "afxdtctl.h"
#include "afxwin.h"

class CDlgAdd : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgAdd)

public:
    CDlgAdd(CWnd* pParent = nullptr);
    virtual ~CDlgAdd();

    enum { IDD = IDD_DLG_ADD };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

public:
    CButton m_radioIn;
    CButton m_radioOut;
    CDateTimeCtrl m_date;
    CEdit m_money;
    CListCtrl m_list;

    // 支出分类按钮控件变量
    CButton m_radioCloth;
    CButton m_radioFood;
    CButton m_radioHome;
    CButton m_radioTravel;
    CButton m_radioPlay;
    CButton m_radioEdu;

    void InitList();
    void LoadRecord();
    void SaveRecord();
    void DeleteRecord();
    CString GetType();
    void EnableExpenseCategories(BOOL bEnable);  // 启用/禁用支出分类按钮

    afx_msg void OnBnClickedBtnSave();
    afx_msg void OnBnClickedBtnDelete();
    afx_msg void OnRadioInOut();  // 收入/支出切换响应
};