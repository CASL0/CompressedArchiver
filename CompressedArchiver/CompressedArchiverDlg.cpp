
// CompressedArchiverDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "CompressedArchiver.h"
#include "CompressedArchiverDlg.h"
#include "afxdialogex.h"
#include "CreateCarDialog.h"

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
}

BEGIN_MESSAGE_MAP(CCompressedArchiverDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CREATE_CAR, &CCompressedArchiverDlg::OnBnClickedButtonCreateCar)
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
	(void)loadStr.LoadStringW(IDS_BUTTON_CREATE_CAR);
	GetDlgItem(IDC_BUTTON_CREATE_CAR)->SetWindowTextW(loadStr);

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



void CCompressedArchiverDlg::OnBnClickedButtonCreateCar()
{
	CreateCarDialog dlg(this);
	(void)dlg.DoModal();
}
