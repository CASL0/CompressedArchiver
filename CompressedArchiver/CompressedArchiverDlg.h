
// CompressedArchiverDlg.h : ヘッダー ファイル
//

#pragma once


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
};
