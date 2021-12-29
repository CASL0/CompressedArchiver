
// CompressedArchiverDlg.h : ヘッダー ファイル
//

#pragma once
#include "CreateCarDialog.h"
#include "OpenCarDialog.h"

// CCompressedArchiverDlg ダイアログ
class CCompressedArchiverDlg : public CDialogEx
{
// コンストラクション
public:
	CCompressedArchiverDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMPRESSEDARCHIVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCreateCar();
	afx_msg void OnBnClickedOpenArchive();
private:
	LRESULT OnDetectBroken(WPARAM wParam, LPARAM lParam);
	CTabCtrl m_tabCtrl;
	CreateCarDialog m_createCarDlg{ &m_tabCtrl };
	OpenCarDialog m_openCarDlg{ &m_tabCtrl };
public:
	BOOL PreTranslateMessage(MSG* msg);
	afx_msg void OnTcnSelchangeTabFunctions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIdok();
};
