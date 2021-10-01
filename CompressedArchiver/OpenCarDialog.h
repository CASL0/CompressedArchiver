#pragma once


// OpenCarDialog ダイアログ

class OpenCarDialog : public CDialogEx
{
	DECLARE_DYNAMIC(OpenCarDialog)

public:
	OpenCarDialog(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~OpenCarDialog();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPEN_CAR_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()

private:
	BOOL OnInitDialog();
	CListCtrl m_fileList;
};
