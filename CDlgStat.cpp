#include "pch.h"
#include "CDlgStat.h"
#include <fstream>
#include <atlstr.h>
#include <cmath>
using namespace std;

IMPLEMENT_DYNAMIC(CDlgStat, CDialogEx)

CDlgStat::CDlgStat(CWnd* pParent) : CDialogEx(IDD_DLG_STATISTICS, pParent) {}

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
        int p1 = line.Find(',');
        int p2 = line.Find(',', p1 + 1);
        int p3 = line.Find(',', p2 + 1);
        if (p1 < 0 || p2 < 0 || p3 < 0) continue;

        Record r;
        r.type = line.Left(p1);
        r.date = line.Mid(p1 + 1, p2 - p1 - 1);
        r.cate = line.Mid(p2 + 1, p3 - p2 - 1);
        r.money = line.Mid(p3 + 1);
        arr.Add(r);
    }
    f.close();
}

void CDlgStat::OnBnClickedBtnQuery()
{
    CString sStart, sEnd;
    m_dtStart.GetWindowText(sStart);
    m_dtEnd.GetWindowText(sEnd);

    CArray<Record, Record> all;
    LoadAllRecords(all);
    m_list.DeleteAllItems();

    double totalIn = 0, totalOut = 0;
    int cateCount[6] = { 0 }; // 衣、食、住、行、娱乐、教育

    for (int i = 0; i < all.GetSize(); i++)
    {
        Record r = all.GetAt(i);
        if (r.date >= sStart && r.date <= sEnd)
        {
            int n = m_list.GetItemCount();
            m_list.InsertItem(n, r.type);
            m_list.SetItemText(n, 1, r.date);
            m_list.SetItemText(n, 2, r.cate);
            m_list.SetItemText(n, 3, r.money);

            double m = _wtof(r.money);
            if (r.type == L"收入")
                totalIn += m;
            else
            {
                totalOut += m;
                if (r.cate == L"衣") cateCount[0]++;
                else if (r.cate == L"食") cateCount[1]++;
                else if (r.cate == L"住") cateCount[2]++;
                else if (r.cate == L"行") cateCount[3]++;
                else if (r.cate == L"娱乐") cateCount[4]++;
                else if (r.cate == L"教育") cateCount[5]++;
            }
        }
    }

    CString sIn, sOut, sBal;
    sIn.Format(L"总收入：%.2lf", totalIn);
    sOut.Format(L"总支出：%.2lf", totalOut);
    sBal.Format(L"结余：%.2lf", totalIn - totalOut);

    m_staticIncome.SetWindowText(sIn);
    m_staticOutcome.SetWindowText(sOut);
    m_staticBalance.SetWindowText(sBal);

    Invalidate();
}

void CDlgStat::OnPaint()
{
    CPaintDC dc(this);

    CArray<Record, Record> all;
    LoadAllRecords(all);
    CString s0, s1;
    m_dtStart.GetWindowText(s0);
    m_dtEnd.GetWindowText(s1);

    int cateCount[6] = { 0 };
    for (int i = 0; i < all.GetSize(); i++)
    {
        Record r = all.GetAt(i);
        if (r.date >= s0 && r.date <= s1 && r.type == L"支出")
        {
            if (r.cate == L"衣") cateCount[0]++;
            else if (r.cate == L"食") cateCount[1]++;
            else if (r.cate == L"住") cateCount[2]++;
            else if (r.cate == L"行") cateCount[3]++;
            else if (r.cate == L"娱乐") cateCount[4]++;
            else if (r.cate == L"教育") cateCount[5]++;
        }
    }
    DrawPie(dc, cateCount);
}

void CDlgStat::DrawPie(CPaintDC& dc, int cateCount[6])
{
    CRect r;
    m_staticPie.GetClientRect(&r);
    int x = r.left + (r.Width() - 160) / 2;
    int y = r.top + (r.Height() - 160) / 2;

    int total = 0;
    for (int i = 0; i < 6; i++)
        total += cateCount[i];
    if (total == 0) return;

    COLORREF colors[] = { RGB(255,0,0), RGB(0,255,0), RGB(0,0,255),
                          RGB(255,255,0), RGB(255,0,255), RGB(0,255,255) };
    CString names[] = { L"衣", L"食", L"住", L"行", L"娱乐", L"教育" };

    const double PI = 3.14159265358979323846;
    double ang = -90.0; // 从12点钟方向开始画

    for (int i = 0; i < 6; i++)
    {
        if (cateCount[i] == 0) continue;

        double sweep = (double)cateCount[i] / total * 360.0;
        double angRad = ang * PI / 180.0;
        double sweepRad = sweep * PI / 180.0;

        int x1 = x + 80 + (int)(80 * cos(angRad));
        int y1 = y + 80 + (int)(80 * sin(angRad));
        int x2 = x + 80 + (int)(80 * cos(angRad + sweepRad));
        int y2 = y + 80 + (int)(80 * sin(angRad + sweepRad));

        dc.SelectStockObject(DC_BRUSH);
        dc.SetDCBrushColor(colors[i]);
        dc.Pie(x, y, x + 160, y + 160, x1, y1, x2, y2);

        ang += sweep;
    }

    // 绘制图例
    int ty = y + 180;
    for (int i = 0; i < 6; i++)
    {
        dc.SelectStockObject(DC_BRUSH);
        dc.SetDCBrushColor(colors[i]);
        dc.Rectangle(x, ty, x + 20, ty + 15);
        dc.SetTextColor(RGB(0, 0, 0));
        dc.TextOut(x + 25, ty, names[i]);
        ty += 20;
    }
}