// OpenCarDialog.cpp : 実装ファイル
//

#include "pch.h"
#include "CompressedArchiver.h"
#include "OpenCarDialog.h"
#include "afxdialogex.h"


// OpenCarDialog ダイアログ

IMPLEMENT_DYNAMIC(OpenCarDialog, CDialogEx)

OpenCarDialog::OpenCarDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPEN_CAR_DIALOG, pParent)
{

}

OpenCarDialog::~OpenCarDialog()
{
}

void OpenCarDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST_IN_CAR, m_fileList);
}

BOOL OpenCarDialog::OnInitDialog()
{
	(void)CDialogEx::OnInitDialog();
	EnableDynamicLayout(TRUE);
	(void)m_pDynamicLayout->Create(this);
	(void)m_pDynamicLayout->AddItem(IDC_FILE_LIST_IN_CAR, CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontalAndVertical(100, 100));
	(void)m_pDynamicLayout->AddItem(IDC_EDIT_OPENED_CAR, CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontal(100));
	(void)m_pDynamicLayout->AddItem(IDC_LABEL_ARCHIVE, CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeNone());

	CString resourceStr;
	(void)resourceStr.LoadStringW(IDS_TITLE_OPEN_CAR);
	SetWindowText(resourceStr);

	(void)resourceStr.LoadStringW(IDS_ARCHIVE_NAME);
	GetDlgItem(IDC_LABEL_ARCHIVE)->SetWindowTextW(resourceStr);

	(void)m_fileList.SetExtendedStyle(m_fileList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_NAME);
	(void)m_fileList.InsertColumn(0, resourceStr, LVCFMT_LEFT, 300);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_SIZE);
	(void)m_fileList.InsertColumn(1, resourceStr, LVCFMT_LEFT, 100);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_LAST_MOD);
	(void)m_fileList.InsertColumn(2, resourceStr, LVCFMT_LEFT, 100);


	
	return TRUE;
}

BEGIN_MESSAGE_MAP(OpenCarDialog, CDialogEx)
END_MESSAGE_MAP()


// OpenCarDialog メッセージ ハンドラー
