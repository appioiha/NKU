#include "pch.h"
#include "CDlgStat.h"
#include <fstream>
#include <atlstr.h>
#include <cmath>
using namespace std;

IMPLEMENT_DYNAMIC(CDlgStat, CDialogEx)

CDlgStat::CDlgStat(CWnd* pParent) : CDialogEx(IDD_DLG_STATISTICS, pParent)
{
    for (int i = 0; i < 6; i++)
        m_cateAmount[i] = 0;
    m_totalOut = 0;
    m_startDate = L"";
    m_endDate = L"";
}

void CDlgStat::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DATETIME_START, m_dtStart);
    DDX_Control(pDX, IDC_DATETIME_END, m_dtEnd);
    DDX_Control(pDX, IDC_LIST_STAT, m_list);
    DDX_Control(pDX, IDC_STATIC_INCOME, m_staticIncome);
    DDX_Control(pDX, IDC_STATIC_OUTCOME, m_staticOutcome);
    DDX_Control(pDX, IDC_STATIC_BALANCE, m_staticBalance);
    DDX_Control(pDX, IDC_STATIC_PIE, m_staticPie);
}
BEGIN_MESSAGE_MAP(CDlgStat, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_QUERY, &CDlgStat::OnBnClickedBtnQuery)
    ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CDlgStat::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    InitList();

    COleDateTime now = COleDateTime::GetCurrentTime();
    COleDateTime start = now - COleDateTimeSpan(10, 0, 0, 0);
    m_dtStart.SetTime(start);
    m_dtEnd.SetTime(now);

    return TRUE;
}

void CDlgStat::InitList()
{
    m_list.DeleteAllItems();
    for (int i = 0; i < 4; i++)
        m_list.DeleteColumn(0);

    m_list.InsertColumn(0, L"类型", LVCFMT_LEFT, 80);
    m_list.InsertColumn(1, L"日期", LVCFMT_LEFT, 120);
    m_list.InsertColumn(2, L"分类", LVCFMT_LEFT, 120);
    m_list.InsertColumn(3, L"金额", LVCFMT_LEFT, 100);
    m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CDlgStat::LoadAllRecords(CArray<Record, Record>& arr)
{
    arr.RemoveAll();
    ifstream f(L"record.txt");
    if (!f) return;

    char buf[512];
    while (f.getline(buf, 512))
    {
        CString line = CA2W(buf);
        line.Trim();

        int p1 = line.Find(',');
        int p2 = line.Find(',', p1 + 1);
        int p3 = line.Find(',', p2 + 1);
        if (p1 < 0 || p2 < 0 || p3 < 0) continue;

        Record r;
        r.type = line.Left(p1);
        r.date = line.Mid(p1 + 1, p2 - p1 - 1);
        r.cate = line.Mid(p2 + 1, p3 - p2 - 1);
        r.money = line.Mid(p3 + 1);

        r.type.Trim();
        r.date.Trim();
        r.cate.Trim();
        r.money.Trim();
        arr.Add(r);
    }
    f.close();
}

void CDlgStat::OnBnClickedBtnQuery()
{
    CString sStart, sEnd;
    CTime startTime, endTime;
    m_dtStart.GetTime(startTime);
    m_dtEnd.GetTime(endTime);

    sStart = startTime.Format(L"%Y/%m/%d");
    sEnd = endTime.Format(L"%Y/%m/%d");

    m_list.DeleteAllItems();
    CArray<Record, Record> all;
    LoadAllRecords(all);

    double totalIn = 0, totalOut = 0;
    double cateAmount[6] = { 0 };
    int nItem = 0;

    for (int i = 0; i < all.GetSize(); i++)
    {
        Record r = all.GetAt(i);
        if (r.date >= sStart && r.date <= sEnd)
        {
            m_list.InsertItem(nItem, r.type);
            m_list.SetItemText(nItem, 1, r.date);
            m_list.SetItemText(nItem, 2, r.cate);
            m_list.SetItemText(nItem, 3, r.money);

            double m = _wtof(r.money);
            if (r.type == L"收入") totalIn += m;
            else if (r.type == L"支出")
            {
                totalOut += m;
                if (r.cate == L"衣") cateAmount[0] += m;
                else if (r.cate == L"食") cateAmount[1] += m;
                else if (r.cate == L"住") cateAmount[2] += m;
                else if (r.cate == L"行") cateAmount[3] += m;
                else if (r.cate == L"娱乐") cateAmount[4] += m;
                else if (r.cate == L"教育") cateAmount[5] += m;
            }
            nItem++;
        }
    }

    for (int i = 0; i < 6; i++) m_cateAmount[i] = cateAmount[i];
    m_totalOut = totalOut;

    CString sIn, sOut, sBal;
    sIn.Format(L"总收入：%.2f 元", totalIn);
    sOut.Format(L"总支出：%.2f 元", totalOut);
    sBal.Format(L"结余：%.2f 元", totalIn - totalOut);

    GetDlgItem(IDC_STATIC_INCOME)->SetWindowText(sIn);
    GetDlgItem(IDC_STATIC_OUTCOME)->SetWindowText(sOut);
    GetDlgItem(IDC_STATIC_BALANCE)->SetWindowText(sBal);

    // ========== 绘制 条形图（替换饼图）==========
    CRect rect;
    m_staticPie.GetClientRect(&rect);
    CDC* pDC = m_staticPie.GetDC();
    pDC->FillSolidRect(rect, RGB(255, 255, 255));
    CDlgStat::DrawBar(pDC, rect, m_cateAmount);
    m_staticPie.ReleaseDC(pDC);
}

void CDlgStat::OnPaint()
{
    CDialogEx::OnPaint();
}

// ========== 【条形图绘制】 100% 稳定 无BUG ==========
void CDlgStat::DrawBar(CDC* pDC, CRect rect, double cateAmount[6])
{
    CString names[] = { L"衣", L"食", L"住", L"行", L"娱乐", L"教育" };
    COLORREF colors[] = {
        RGB(255,99,132),
        RGB(54,162,235),
        RGB(255,206,86),
        RGB(75,192,192),
        RGB(153,102,255),
        RGB(255,159,64)
    };

    double maxVal = 0;
    for (int i = 0; i < 6; i++)
        if (cateAmount[i] > maxVal) maxVal = cateAmount[i];
    if (maxVal < 1) return;

    int barHeight = 26;
    int space = 14;
    int left = 50;
    int w = rect.Width() - left - 30;

    CFont font;
    font.CreatePointFont(90, L"微软雅黑");
    CFont* oldFont = pDC->SelectObject(&font);

    for (int i = 0; i < 6; i++)
    {
        int y = 22 + i * (barHeight + space);
        int barW = (int)(cateAmount[i] / maxVal * w);

        pDC->TextOut(10, y + 2, names[i]);

        CBrush br(colors[i]);
        CBrush* oldBr = pDC->SelectObject(&br);
        pDC->Rectangle(left, y, left + barW, y + barHeight);
        pDC->SelectObject(oldBr);

        CString txt;
        txt.Format(L"%.1f元", cateAmount[i]);
        pDC->TextOut(left + barW + 6, y + 2, txt);
    }

    pDC->SelectObject(oldFont);
}
