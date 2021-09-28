
// CompressedArchiver.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "Resource.h"		// メイン シンボル
#include <string>


// CCompressedArchiverApp:
// このクラスの実装については、CompressedArchiver.cpp を参照してください
//

class CCompressedArchiverApp : public CWinApp
{
public:
	CCompressedArchiverApp();
	std::wstring FormatErrorMessage(ULONG errorCode) const;

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()

private:
	uint32_t GenerateHash32(const std::string& buffer) const;

};

extern CCompressedArchiverApp theApp;
