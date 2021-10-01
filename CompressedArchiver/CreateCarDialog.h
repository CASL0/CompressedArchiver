#pragma once
#include <string>

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
	DWORD GetFileSizeAndLastWriteTime(const std::wstring& filePath, uint64_t& fileSize, std::wstring& lastWriteTime) const;
	void AddItemToList(const std::wstring& filePath, const std::wstring& fileSize, const std::wstring& lastWriteTime);
	bool CheckDoubled(const std::wstring& filePath) const;

	CListCtrl m_itemList;
	CComboBox m_comboAlgorithms;
public:
	afx_msg void OnBnClickedButtonAddList();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonCreate();
};
