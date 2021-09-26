#pragma once


// CreateCarDialog ダイアログ

class CreateCarDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CreateCarDialog)

public:
	CreateCarDialog(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CreateCarDialog();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_CAR_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
private:
	BOOL OnInitDialog();
	void SetupComboBox();

	CListCtrl m_itemList;
	CComboBox m_comboAlgorithms;
};
