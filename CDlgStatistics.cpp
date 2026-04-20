// CDlgStatistics.cpp: 实现文件
//

#include "pch.h"
#include "AccountBook.h"
#include "afxdialogex.h"
#include "CDlgStatistics.h"


// CDlgStatistics 对话框

IMPLEMENT_DYNAMIC(CDlgStatistics, CDialogEx)

CDlgStatistics::CDlgStatistics(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_STATISTICS, pParent)
{

}

CDlgStatistics::~CDlgStatistics()
{
}

void CDlgStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStatistics, CDialogEx)
	ON_STN_CLICKED(IDC_STATIC_PIE, &CDlgStatistics::OnStnClickedStaticPie)
END_MESSAGE_MAP()


// CDlgStatistics 消息处理程序


void CDlgStatistics::OnStnClickedStaticPie()
{
	// TODO: 在此添加控件通知处理程序代码
}
