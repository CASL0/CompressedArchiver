
// CompressedArchiver.cpp : アプリケーションのクラス動作を定義します。
//

#include "pch.h"
#include "framework.h"
#include "CompressedArchiver.h"
#include "CompressedArchiverDlg.h"
#include <memory>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCompressedArchiverApp

BEGIN_MESSAGE_MAP(CCompressedArchiverApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCompressedArchiverApp の構築

CCompressedArchiverApp::CCompressedArchiverApp()
{
	// 再起動マネージャーをサポートします
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CCompressedArchiverApp オブジェクト

CCompressedArchiverApp theApp;


// CCompressedArchiverApp の初期化

BOOL CCompressedArchiverApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls = { 0 };
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	auto shellManager = std::make_unique<CShellManager>();

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	CCompressedArchiverDlg dlg;
	m_pMainWnd = &dlg;
	(void)dlg.DoModal();

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

std::wstring CCompressedArchiverApp::FormatErrorMessage(ULONG errorCode) const
{
	constexpr DWORD BUFFERLENGTH = 1024;
	std::vector<wchar_t> buf(BUFFERLENGTH);
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, static_cast<DWORD>(errorCode), 0, buf.data(), BUFFERLENGTH - 1, 0);
	return std::wstring(buf.data()) + L"\n";
}