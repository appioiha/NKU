#include "pch.h"
#include "CDlgStat.h"
#include <fstream>
#include <atlstr.h>
#include <cmath>
using namespace std;

IMPLEMENT_DYNAMIC(CDlgStat, CDialogEx)

CDlgStat::CDlgStat(CWnd* pParent) : CDialogEx(IDD_DLG_STATISTICS, pParent)
{
    // 初始化统计数据
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

   
    // 设置默认日期范围（最近10天）
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

    // 尝试打开文件
    ifstream f(L"record.txt");
    if (!f)
    {
        // 文件不存在时提示
        // MessageBox(L"record.txt 文件不存在！");
        return;
    }

    char buf[512];
    while (f.getline(buf, 512))
    {
        CString line = CA2W(buf);
        line.Trim();  // 去除首尾空格

        // 调试：显示读取的行
        // MessageBox(line);

        int p1 = line.Find(',');
        int p2 = line.Find(',', p1 + 1);
        int p3 = line.Find(',', p2 + 1);

        if (p1 < 0 || p2 < 0 || p3 < 0)
            continue;

        Record r;
        r.type = line.Left(p1);
        r.date = line.Mid(p1 + 1, p2 - p1 - 1);
        r.cate = line.Mid(p2 + 1, p3 - p2 - 1);
        r.money = line.Mid(p3 + 1);

        // 去除可能的空格
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
    // 获取日期并统一格式
    CString sStart, sEnd;

    CTime startTime, endTime;
    m_dtStart.GetTime(startTime);
    m_dtEnd.GetTime(endTime);

    sStart = startTime.Format(L"%Y/%m/%d");
    sEnd = endTime.Format(L"%Y/%m/%d");

    // 清空列表
    m_list.DeleteAllItems();

    // 读取所有记录
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
            if (r.type == L"收入")
            {
                totalIn += m;
            }
            else if (r.type == L"支出")
            {
                totalOut += m;
                
                if (r.cate == L"衣")    cateAmount[0] += m;    // 下标0 衣（粉色）
                else if (r.cate == L"食")    cateAmount[1] += m;    // 下标1 食（蓝色）
                else if (r.cate == L"住")    cateAmount[2] += m;    // 下标2 住（黄色）
                else if (r.cate == L"行")    cateAmount[3] += m;    // 下标3 行（青色）
                else if (r.cate == L"娱乐") cateAmount[4] += m;    // 下标4 娱乐（紫色）
                else if (r.cate == L"教育") cateAmount[5] += m;    // 下标5 教育（橙色）

            }
            nItem++;
        }
    }

    // 保存到成员变量
    for (int i = 0; i < 6; i++)
        m_cateAmount[i] = cateAmount[i];
    m_totalOut = totalOut;
    m_startDate = sStart;
    m_endDate = sEnd;

    // 格式化文字
    CString sIn, sOut, sBal;
    sIn.Format(L"总收入：%.2f 元", totalIn);
    sOut.Format(L"总支出：%.2f 元", totalOut);
    sBal.Format(L"结余：%.2f 元", totalIn - totalOut);

    // 设置文字
    GetDlgItem(IDC_STATIC_INCOME)->SetWindowText(sIn);
    GetDlgItem(IDC_STATIC_OUTCOME)->SetWindowText(sOut);
    GetDlgItem(IDC_STATIC_BALANCE)->SetWindowText(sBal);

    // 强制刷新控件
    GetDlgItem(IDC_STATIC_INCOME)->RedrawWindow();
    GetDlgItem(IDC_STATIC_OUTCOME)->RedrawWindow();
    GetDlgItem(IDC_STATIC_BALANCE)->RedrawWindow();

    // 弹窗显示查询结果
    CString msg;
    msg.Format(L"查询完成！\n\n找到 %d 条记录\n总收入：%.2f 元\n总支出：%.2f 元\n结余：%.2f 元",
        nItem, totalIn, totalOut, totalIn - totalOut);
    MessageBox(msg, L"统计结果", MB_OK | MB_ICONINFORMATION);

    // 强制重绘饼图
    Invalidate();
}


void CDlgStat::OnPaint()
{
    CPaintDC dc(this);

    CRect pieWindowRect;
    m_staticPie.GetWindowRect(&pieWindowRect);
    ScreenToClient(&pieWindowRect);

    CDC memDC;
    CBitmap bitmap;
    memDC.CreateCompatibleDC(&dc);
    bitmap.CreateCompatibleBitmap(&dc, pieWindowRect.Width(), pieWindowRect.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

    memDC.FillSolidRect(0, 0, pieWindowRect.Width(), pieWindowRect.Height(), RGB(255, 255, 255));

    CRect pieClientRect(0, 0, pieWindowRect.Width(), pieWindowRect.Height());
    DrawPie(&memDC, pieClientRect, m_cateAmount);

    dc.BitBlt(pieWindowRect.left, pieWindowRect.top,
        pieWindowRect.Width(), pieWindowRect.Height(),
        &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldBitmap);
    bitmap.DeleteObject();

    // 👇 加上这一行！防止对话框其他重绘擦掉饼图
    ValidateRect(&pieWindowRect);
}



void CDlgStat::DrawPie(CDC* pDC, CRect rect, double cateAmount[6])
{
    // 获取控件实际区域
    CRect clientRect;
    m_staticPie.GetClientRect(clientRect);

    // 计算饼图绘制区域（正方形，位于控件中央）
    int pieSize = min(clientRect.Width(), clientRect.Height()) - 20;
    if (pieSize < 80) pieSize = 80;
    if (pieSize > clientRect.Width() - 20) pieSize = clientRect.Width() - 20;

    int x = clientRect.left + (clientRect.Width() - pieSize) / 2;
    int y = clientRect.top + (clientRect.Height() - pieSize) / 2;

    // 计算总支出
    double total = 0;
    for (int i = 0; i < 6; i++)
        total += cateAmount[i];

    if (total == 0)
    {
        // 没有数据时显示提示文字
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(128, 128, 128));
        CFont font;
        font.CreatePointFont(120, L"微软雅黑");
        CFont* pOldFont = pDC->SelectObject(&font);
        pDC->TextOut(clientRect.left + clientRect.Width() / 2 - 60,
            clientRect.top + clientRect.Height() / 2 - 10, L"暂无支出数据");
        pDC->SelectObject(pOldFont);
        font.DeleteObject();
        return;
    }

    // 定义颜色和名称
    COLORREF colors[] = {
        RGB(255, 99, 132),   // 衣 - 粉红
        RGB(54, 162, 235),   // 食 - 蓝色
        RGB(255, 206, 86),   // 住 - 黄色
        RGB(75, 192, 192),   // 行 - 青色
        RGB(153, 102, 255),  // 娱乐 - 紫色
        RGB(255, 159, 64)    // 教育 - 橙色
    };
    CString names[] = { L"衣", L"食", L"住", L"行", L"娱乐", L"教育" };

    const double PI = 3.14159265358979323846;
    double startAngle = -90.0; // 从12点钟方向开始

    // 绘制饼图
    for (int i = 0; i < 6; i++)
    {
        if (cateAmount[i] == 0) continue;

        double sweepAngle = (cateAmount[i] / total) * 360.0;

        // 计算弧线的起点和终点坐标
        double startRad = startAngle * PI / 180.0;
        double sweepRad = (startAngle + sweepAngle) * PI / 180.0;

        int centerX = x + pieSize / 2;
        int centerY = y + pieSize / 2;
        int radius = pieSize / 2;

        int x1 = centerX + (int)(radius * cos(startRad));
        int y1 = centerY + (int)(radius * sin(startRad));
        int x2 = centerX + (int)(radius * cos(sweepRad));
        int y2 = centerY + (int)(radius * sin(sweepRad));

        // 创建画刷并绘制扇形
        CBrush brush(colors[i]);
        CBrush* pOldBrush = pDC->SelectObject(&brush);

        pDC->Pie(x, y, x + pieSize, y + pieSize, x1, y1, x2, y2);

        pDC->SelectObject(pOldBrush);
        brush.DeleteObject();

        startAngle += sweepAngle;
    }

    // 绘制饼图边框
    CPen pen(PS_SOLID, 1, RGB(128, 128, 128));
    CPen* pOldPen = pDC->SelectObject(&pen);
    pDC->SelectStockObject(NULL_BRUSH);
    pDC->Ellipse(x, y, x + pieSize, y + pieSize);
    pDC->SelectObject(pOldPen);
    pen.DeleteObject();

    // 绘制图例（放在饼图右侧）
    int legendX = x + pieSize + 10;
    int legendY = y;

    // 如果图例超出控件右边界，则放在饼图下方
    if (legendX + 80 > clientRect.right)
    {
        legendX = x;
        legendY = y + pieSize + 10;
    }

    CFont font;
    font.CreatePointFont(90, L"微软雅黑");
    CFont* pOldFont = pDC->SelectObject(&font);

    for (int i = 0; i < 6; i++)
    {
        if (cateAmount[i] > 0)
        {
            // 绘制颜色块
            CBrush brush(colors[i]);
            CBrush* pOldBrush = pDC->SelectObject(&brush);
            pDC->Rectangle(legendX, legendY, legendX + 12, legendY + 12);
            pDC->SelectObject(pOldBrush);
            brush.DeleteObject();

            // 绘制文字（分类名和占比）
            double percent = (cateAmount[i] / total) * 100;
            CString text;
            text.Format(L"%s:%.1f%%", names[i], percent);
            pDC->SetBkMode(TRANSPARENT);
            pDC->TextOut(legendX + 16, legendY, text);

            legendY += 18;
        }
    }

    pDC->SelectObject(pOldFont);
    font.DeleteObject();
}