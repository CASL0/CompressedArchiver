// CreateCarDialog.cpp : 実装ファイル
//

#include "pch.h"
#include "CompressedArchiver.h"
#include "CreateCarDialog.h"
#include "afxdialogex.h"


// CreateCarDialog ダイアログ

IMPLEMENT_DYNAMIC(CreateCarDialog, CDialogEx)

CreateCarDialog::CreateCarDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CREATE_CAR_DIALOG, pParent)
{

}

CreateCarDialog::~CreateCarDialog()
{
}

void CreateCarDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ITEM_LIST, m_itemList);
	DDX_Control(pDX, IDC_COMBO_ALGORITHM, m_comboAlgorithms);
}

BOOL CreateCarDialog::OnInitDialog()
{
	(void)CDialogEx::OnInitDialog();
	EnableDynamicLayout(TRUE);
	(void)m_pDynamicLayout->Create(this);
	(void)m_pDynamicLayout->AddItem(IDC_ITEM_LIST, CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontalAndVertical(100, 100));
	(void)m_pDynamicLayout->AddItem(IDC_BUTTON_ADD_LIST, CMFCDynamicLayout::MoveHorizontalAndVertical(100, 100), CMFCDynamicLayout::SizeNone());
	(void)m_pDynamicLayout->AddItem(IDC_GROUP_CAR_SETTING, CMFCDynamicLayout::MoveVertical(100), CMFCDynamicLayout::SizeHorizontal(100));
	(void)m_pDynamicLayout->AddItem(IDC_LABEL_CAR_NAME, CMFCDynamicLayout::MoveVertical(100), CMFCDynamicLayout::SizeNone());
	(void)m_pDynamicLayout->AddItem(IDC_EDIT_CARFILE, CMFCDynamicLayout::MoveVertical(100), CMFCDynamicLayout::SizeHorizontal(100));
	(void)m_pDynamicLayout->AddItem(IDC_COMBO_ALGORITHM, CMFCDynamicLayout::MoveHorizontalAndVertical(100, 100), CMFCDynamicLayout::SizeNone());
	(void)m_pDynamicLayout->AddItem(IDC_BUTTON_CREATE, CMFCDynamicLayout::MoveHorizontalAndVertical(100, 100), CMFCDynamicLayout::SizeNone());

	CString resourceStr;
	(void)resourceStr.LoadStringW(IDS_TITLE_CREATE_CAR);
	SetWindowText(resourceStr);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_NAME);
	(void)m_itemList.InsertColumn(0, resourceStr, LVCFMT_LEFT, 300);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_SIZE);
	(void)m_itemList.InsertColumn(1, resourceStr, LVCFMT_LEFT, 100);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_LAST_MOD);
	(void)m_itemList.InsertColumn(2, resourceStr, LVCFMT_LEFT, 100);

	(void)resourceStr.LoadStringW(IDS_BUTTON_ADD_LIST);
	(void)GetDlgItem(IDC_BUTTON_ADD_LIST)->SetWindowTextW(resourceStr);

	(void)resourceStr.LoadStringW(IDS_GROUP_CAR_SETTING);
	(void)GetDlgItem(IDC_GROUP_CAR_SETTING)->SetWindowTextW(resourceStr);

	(void)resourceStr.LoadStringW(IDS_CAR_NAME);
	(void)GetDlgItem(IDC_LABEL_CAR_NAME)->SetWindowTextW(resourceStr);

	(void)resourceStr.LoadStringW(IDS_BUTTON_CREATE);
	(void)GetDlgItem(IDC_BUTTON_CREATE)->SetWindowTextW(resourceStr);

	SetupComboBox();

	return TRUE;
}

void CreateCarDialog::SetupComboBox()
{
	m_comboAlgorithms.SetExtendedUI();
	auto idx = 0;

	CString resourceStr;
	(void)resourceStr.LoadStringW(IDS_COMBO_ALGORITHM_MSZIP);
	(void)m_comboAlgorithms.InsertString(idx++, resourceStr);

	(void)resourceStr.LoadStringW(IDS_COMBO_ALGORITHM_XPRESS);
	(void)m_comboAlgorithms.InsertString(idx++, resourceStr);

	(void)resourceStr.LoadStringW(IDS_COMBO_ALGORITHM_XPRESS_HUFFMAN);
	(void)m_comboAlgorithms.InsertString(idx++, resourceStr);

	(void)resourceStr.LoadStringW(IDS_COMBO_ALGORITHM_LZMS);
	(void)m_comboAlgorithms.InsertString(idx++, resourceStr);

	m_comboAlgorithms.SetCurSel(0);

}

BEGIN_MESSAGE_MAP(CreateCarDialog, CDialogEx)
END_MESSAGE_MAP()


// CreateCarDialog メッセージ ハンドラー
