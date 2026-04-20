#include "pch.h"
#include "AccountBook.h"
#include "CDlgAdd.h"
#include <fstream>
#include <atlstr.h>
using namespace std;

IMPLEMENT_DYNAMIC(CDlgAdd, CDialogEx)

CDlgAdd::CDlgAdd(CWnd* pParent)
    : CDialogEx(IDD_DLG_ADD, pParent)
{
}

CDlgAdd::~CDlgAdd()
{
}

void CDlgAdd::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RADIO_IN, m_radioIn);
    DDX_Control(pDX, IDC_RADIO_OUT, m_radioOut);
    DDX_Control(pDX, IDC_DATETIME, m_date);
    DDX_Control(pDX, IDC_EDIT_MONEY, m_money);
    DDX_Control(pDX, IDC_LIST_RECORD, m_list);

    // 绑定支出分类按钮
    DDX_Control(pDX, IDC_RADIO_CLOTH, m_radioCloth);
    DDX_Control(pDX, IDC_RADIO_FOOD, m_radioFood);
    DDX_Control(pDX, IDC_RADIO_HOME, m_radioHome);
    DDX_Control(pDX, IDC_RADIO_TRAVEL, m_radioTravel);
    DDX_Control(pDX, IDC_RADIO_PLAY, m_radioPlay);
    DDX_Control(pDX, IDC_RADIO_EDU, m_radioEdu);
}

BEGIN_MESSAGE_MAP(CDlgAdd, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgAdd::OnBnClickedBtnSave)
    ON_BN_CLICKED(IDC_BTN_DELETE, &CDlgAdd::OnBnClickedBtnDelete)
    ON_BN_CLICKED(IDC_RADIO_IN, &CDlgAdd::OnRadioInOut)
    ON_BN_CLICKED(IDC_RADIO_OUT, &CDlgAdd::OnRadioInOut)
END_MESSAGE_MAP()

BOOL CDlgAdd::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 默认选中收入
    m_radioIn.SetCheck(BST_CHECKED);
    m_radioOut.SetCheck(BST_UNCHECKED);

    // 收入时禁用支出分类按钮
    EnableExpenseCategories(FALSE);

    // 初始化列表表头
    InitList();

    // 加载已有记录
    LoadRecord();

    return TRUE;
}

void CDlgAdd::InitList()
{
    // 先清空所有列
    m_list.DeleteAllItems();

    // 删除所有列（需要多次删除直到全部删除）
    while (m_list.DeleteColumn(0)) {}

    // 重新插入表头，设置列宽
    m_list.InsertColumn(0, L"类型", LVCFMT_LEFT, 80);
    m_list.InsertColumn(1, L"日期", LVCFMT_LEFT, 120);
    m_list.InsertColumn(2, L"分类", LVCFMT_LEFT, 120);
    m_list.InsertColumn(3, L"金额", LVCFMT_LEFT, 100);

    // 让列表显示表头，支持整行选择和网格线
    m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CDlgAdd::EnableExpenseCategories(BOOL bEnable)
{
    m_radioCloth.EnableWindow(bEnable);
    m_radioFood.EnableWindow(bEnable);
    m_radioHome.EnableWindow(bEnable);
    m_radioTravel.EnableWindow(bEnable);
    m_radioPlay.EnableWindow(bEnable);
    m_radioEdu.EnableWindow(bEnable);
}

void CDlgAdd::OnRadioInOut()
{
    BOOL bIsIncome = (m_radioIn.GetCheck() == BST_CHECKED);

    // 收入时禁用支出分类按钮，支出时启用
    EnableExpenseCategories(!bIsIncome);

    // 切换时清空之前选中的分类
    m_radioCloth.SetCheck(BST_UNCHECKED);
    m_radioFood.SetCheck(BST_UNCHECKED);
    m_radioHome.SetCheck(BST_UNCHECKED);
    m_radioTravel.SetCheck(BST_UNCHECKED);
    m_radioPlay.SetCheck(BST_UNCHECKED);
    m_radioEdu.SetCheck(BST_UNCHECKED);
}

CString CDlgAdd::GetType()
{
    // 如果是收入，返回空（收入不显示具体分类）
    if (m_radioIn.GetCheck() == BST_CHECKED)
        return L"";

    // 支出：获取选中的分类
    if (m_radioCloth.GetCheck() == BST_CHECKED)
        return L"衣";
    if (m_radioFood.GetCheck() == BST_CHECKED)
        return L"食";
    if (m_radioHome.GetCheck() == BST_CHECKED)
        return L"住";
    if (m_radioTravel.GetCheck() == BST_CHECKED)
        return L"行";
    if (m_radioPlay.GetCheck() == BST_CHECKED)
        return L"娱乐";
    if (m_radioEdu.GetCheck() == BST_CHECKED)
        return L"教育";

    return L"";
}

void CDlgAdd::OnBnClickedBtnSave()
{
    CString type, date, money, cate;

    // 1. 获取收入/支出
    if (m_radioIn.GetCheck() == BST_CHECKED)
        type = L"收入";
    else
        type = L"支出";

    // 2. 获取日期（格式化为 YYYY/MM/DD）
    CTime time;
    m_date.GetTime(time);
    date = time.Format(L"%Y/%m/%d");

    // 3. 获取金额（非空校验）
    m_money.GetWindowText(money);
    money.Trim();
    if (money.IsEmpty())
    {
        MessageBox(L"请输入金额！", L"提示", MB_OK | MB_ICONWARNING);
        return;
    }

    // 4. 获取分类
    cate = GetType();

    // 支出时必须选择分类
    if (type == L"支出" && cate.IsEmpty())
    {
        MessageBox(L"请选择支出分类！", L"提示", MB_OK | MB_ICONWARNING);
        return;
    }

    // 如果是收入，分类显示为"--"
    if (type == L"收入" && cate.IsEmpty())
    {
        cate = L"--";
    }

    // 5. 插入列表
    int n = m_list.GetItemCount();
    m_list.InsertItem(n, type);
    m_list.SetItemText(n, 1, date);
    m_list.SetItemText(n, 2, cate);
    m_list.SetItemText(n, 3, money);

    // 自动滚动到最新记录
    m_list.EnsureVisible(n, FALSE);
    // 刷新列表显示
    m_list.UpdateWindow();

    // 6. 写入文件（追加模式）
    ofstream file(L"record.txt", ios::app);
    if (file.is_open())
    {
        CStringA typeA(type), dateA(date), cateA(cate), moneyA(money);
        file << typeA << "," << dateA << "," << cateA << "," << moneyA << endl;
        file.close();
    }
    else
    {
        MessageBox(L"无法保存记录！", L"错误", MB_OK | MB_ICONERROR);
        return;
    }

    // 7. 清空金额输入框和分类选择
    m_money.SetWindowText(L"");

    // 清空支出分类的选中状态
    m_radioCloth.SetCheck(BST_UNCHECKED);
    m_radioFood.SetCheck(BST_UNCHECKED);
    m_radioHome.SetCheck(BST_UNCHECKED);
    m_radioTravel.SetCheck(BST_UNCHECKED);
    m_radioPlay.SetCheck(BST_UNCHECKED);
    m_radioEdu.SetCheck(BST_UNCHECKED);

    // 让金额输入框重新获得焦点
    m_money.SetFocus();
}

void CDlgAdd::OnBnClickedBtnDelete()
{
    int nSel = m_list.GetSelectionMark();
    if (nSel < 0)
    {
        MessageBox(L"请先选中要删除的记录！", L"提示", MB_OK | MB_ICONWARNING);
        return;
    }

    // 确认删除
    if (MessageBox(L"确定要删除选中的记录吗？", L"确认", MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;

    // 从列表删除
    m_list.DeleteItem(nSel);

    // 重新写入文件
    SaveRecord();
}

void CDlgAdd::SaveRecord()
{
    ofstream file(L"record.txt");
    if (file.is_open())
    {
        for (int i = 0; i < m_list.GetItemCount(); i++)
        {
            CString t = m_list.GetItemText(i, 0);
            CString d = m_list.GetItemText(i, 1);
            CString c = m_list.GetItemText(i, 2);
            CString m = m_list.GetItemText(i, 3);
            CStringA tA(t), dA(d), cA(c), mA(m);
            file << tA << "," << dA << "," << cA << "," << mA << endl;
        }
        file.close();
    }
}

void CDlgAdd::LoadRecord()
{
    m_list.DeleteAllItems();
    ifstream file(L"record.txt");
    if (!file.is_open()) return;

    char buf[512];
    while (file.getline(buf, 512))
    {
        CString line = CA2W(buf);
        int p1 = line.Find(',');
        int p2 = line.Find(',', p1 + 1);
        int p3 = line.Find(',', p2 + 1);

        if (p1 < 0 || p2 < 0 || p3 < 0) continue;

        CString t = line.Left(p1);
        CString d = line.Mid(p1 + 1, p2 - p1 - 1);
        CString c = line.Mid(p2 + 1, p3 - p2 - 1);
        CString m = line.Mid(p3 + 1);

        // 如果是旧数据中收入分类显示为"收入"，改为"--"
        if (t == L"收入" && c == L"收入")
        {
            c = L"--";
        }

        int n = m_list.GetItemCount();
        m_list.InsertItem(n, t);
        m_list.SetItemText(n, 1, d);
        m_list.SetItemText(n, 2, c);
        m_list.SetItemText(n, 3, m);
    }
    file.close();
}