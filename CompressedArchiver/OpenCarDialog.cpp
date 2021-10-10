// OpenCarDialog.cpp : 実装ファイル
//

#include "pch.h"
#include "CompressedArchiver.h"
#include "OpenCarDialog.h"
#include "afxdialogex.h"
#include <map>
#include <sstream>

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
	DDX_Control(pDX, IDC_EDIT_OPENED_CAR, m_editCarFileName);
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

	auto listColum = 0;
	(void)m_fileList.SetExtendedStyle(m_fileList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_NAME);
	(void)m_fileList.InsertColumn(listColum++, resourceStr, LVCFMT_LEFT, 200);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_LAST_MOD);
	(void)m_fileList.InsertColumn(listColum++, resourceStr, LVCFMT_LEFT, 150);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_ORIGINAL_SIZE);
	(void)m_fileList.InsertColumn(listColum++, resourceStr, LVCFMT_LEFT, 150);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_COMPRESSED_SIZE);
	(void)m_fileList.InsertColumn(listColum++, resourceStr, LVCFMT_LEFT, 160);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_RATIO);
	(void)m_fileList.InsertColumn(listColum++, resourceStr, LVCFMT_LEFT, 100);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_METHOD);
	(void)m_fileList.InsertColumn(listColum++, resourceStr, LVCFMT_LEFT, 130);

	(void)resourceStr.LoadStringW(IDS_LIST_HEADER_CHECK_SUM);
	(void)m_fileList.InsertColumn(listColum++, resourceStr, LVCFMT_LEFT, 100);

	CRect rect;
	GetWindowRect(&rect);
	(void)SetWindowPos(nullptr, rect.left, rect.top, 1100, 600, SWP_SHOWWINDOW);
	
	return TRUE;
}

void OpenCarDialog::AddItemToList(const CCompressedArchiverApp::CarHeader& header)
{
	static const std::map<COMPRESS_ALGORITHM, std::wstring> algorithmMap =
	{
		{COMPRESS_ALGORITHM::MSZIP, L"ZIP"},
		{COMPRESS_ALGORITHM::XPRESS, L"XPRESS"},
		{COMPRESS_ALGORITHM::XPRESS_HUFF, L"XPRESS+ハフマン"},
		{COMPRESS_ALGORITHM::LZMS, L"LZMS"},
		{COMPRESS_ALGORITHM::LZ4, L"LZ4"},
		{COMPRESS_ALGORITHM::ZSTD, L"Zstandard"},
	};

	auto numItems = m_fileList.GetItemCount();

	(void)m_fileList.InsertItem(numItems, theApp.Utf8ToUtf16(header.fileName).c_str());
	auto column = 1;
	auto fileTime = theApp.Uint64ToFileTime(header.lastWriteTime);
	CTime systemTime(fileTime);
	auto lastWriteTime(systemTime.Format(L"%Y/%m/%d %H:%M:%S"));
	(void)m_fileList.SetItemText(numItems, column++, lastWriteTime.GetString());
	(void)m_fileList.SetItemText(numItems, column++, std::to_wstring(header.originalSize).c_str());
	(void)m_fileList.SetItemText(numItems, column++, std::to_wstring(header.compressedSize).c_str());
	(void)m_fileList.SetItemText(numItems, column++, std::to_wstring(theApp.CompressionRatio(header.originalSize, header.compressedSize)).c_str());
	(void)m_fileList.SetItemText(numItems, column++, algorithmMap.at(static_cast<COMPRESS_ALGORITHM>(header.compressionMethod)).c_str());

	std::wstringstream ss;
	ss << L"0x";
	ss << std::hex << header.fileCheckSum;
	(void)m_fileList.SetItemText(numItems, column++, ss.str().c_str());

}

BEGIN_MESSAGE_MAP(OpenCarDialog, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_OPENED_CAR, &OpenCarDialog::OnEnChangeEditOpenedCar)
END_MESSAGE_MAP()


// OpenCarDialog メッセージ ハンドラー


void OpenCarDialog::OnEnChangeEditOpenedCar()
{
	CString carFileName;
	m_editCarFileName.GetWindowTextW(carFileName);
	OutputDebugString(L"CAR: ");
	OutputDebugString(carFileName.GetString());
	OutputDebugString(L"\n");

	std::list<CCompressedArchiverApp::CarHeader> headerList;
	if (auto ret = theApp.GetHeaderList(carFileName.GetString(), headerList); ret != ERROR_SUCCESS)
	{
		auto errorMessage = theApp.FormatErrorMessage(ret);
		OutputDebugString(errorMessage.c_str());
		(void)AfxMessageBox(errorMessage.c_str(), MB_ICONSTOP);
		return;
	}

	(void)m_fileList.DeleteAllItems();
	for (const auto& elem : headerList)
	{
		AddItemToList(elem);
		OutputDebugStringA(elem.fileName.c_str());
		OutputDebugString(L"\n");
	}
}
