
// CompressedArchiverDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "CompressedArchiver.h"
#include "CompressedArchiverDlg.h"
#include "afxdialogex.h"
#include "CreateCarDialog.h"
#include "OpenCarDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCompressedArchiverDlg ダイアログ



CCompressedArchiverDlg::CCompressedArchiverDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_COMPRESSEDARCHIVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCompressedArchiverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_FUNCTIONS, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CCompressedArchiverDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(CCompressedArchiverApp::APP_MESSAGE_BROKEN_PACKAGE, CCompressedArchiverDlg::OnDetectBroken)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_FUNCTIONS, &CCompressedArchiverDlg::OnTcnSelchangeTabFunctions)
END_MESSAGE_MAP()


// CCompressedArchiverDlg メッセージ ハンドラー

BOOL CCompressedArchiverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	CString loadStr;
	(void)loadStr.LoadStringW(AFX_IDS_APP_TITLE);
	SetWindowText(loadStr);

	EnableDynamicLayout(TRUE);
	(void)m_pDynamicLayout->Create(this);
	(void)m_pDynamicLayout->AddItem(IDC_TAB_FUNCTIONS, CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontalAndVertical(100, 100));

	CRect rect;
	GetWindowRect(&rect);
	(void)SetWindowPos(nullptr, rect.left, rect.top, 1100, 600, SWP_SHOWWINDOW);

	auto numTabs = 0;
	(void)loadStr.LoadStringW(IDS_TAB_CREATE_CAR);
	(void)m_tabCtrl.InsertItem(numTabs++, loadStr);

	(void)loadStr.LoadStringW(IDS_TAB_OPEN_CAR);
	(void)m_tabCtrl.InsertItem(numTabs++, loadStr);

	CRect clientRect;
	m_tabCtrl.GetClientRect(&clientRect);
	clientRect.top += 22;

	(void)m_createCarDlg.Create(IDD_CREATE_CAR_DIALOG, &m_tabCtrl);
	m_createCarDlg.MoveWindow(&clientRect);

	(void)m_openCarDlg.Create(IDD_OPEN_CAR_DIALOG, &m_tabCtrl);
	m_openCarDlg.MoveWindow(&clientRect);

	(void)m_tabCtrl.SetCurSel(0);
	OnTcnSelchangeTabFunctions(nullptr, nullptr);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CCompressedArchiverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CCompressedArchiverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CCompressedArchiverDlg::OnDetectBroken(WPARAM wParam, LPARAM lParam)
{
	auto brokenPackage = reinterpret_cast<wchar_t*>(wParam);
	if (brokenPackage)
	{
		OutputDebugString(L"ファイルが破損しています：");
		OutputDebugString(brokenPackage);
		OutputDebugString(L"\n");
	}
	(void)AfxMessageBox(IDS_WARN_BROKEN_PACKAGE);
	return ERROR_SUCCESS;
}

void CCompressedArchiverDlg::OnTcnSelchangeTabFunctions(NMHDR* pNMHDR, LRESULT* pResult)
{
	auto curSel = m_tabCtrl.GetCurSel();
	if (curSel == 0)
	{
		(void)m_createCarDlg.ShowWindow(SW_SHOW);
		(void)m_openCarDlg.ShowWindow(SW_HIDE);
	}
	else if (curSel == 1)
	{
		(void)m_createCarDlg.ShowWindow(SW_HIDE);
		(void)m_openCarDlg.ShowWindow(SW_SHOW);
	}
}
