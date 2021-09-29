
// CompressedArchiver.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "Resource.h"		// メイン シンボル
#include "Compressor.h"
#include <string>
#include <list>


// CCompressedArchiverApp:
// このクラスの実装については、CompressedArchiver.cpp を参照してください
//

class CCompressedArchiverApp : public CWinApp
{
private:
	typedef struct tagCarHeader 
	{
		std::string fileName;
		uint8_t compressionMethod = 0;
		uint32_t originalSize = 0;
		uint32_t compressedSize = 0;
		uint64_t lastWriteTime = 0;
		uint32_t fileCheckSum = 0;
		uint32_t headerCheckSum = 0;
	} CarHeader;
	CFile m_outputCarFile;

	static const DWORD CAR_HEADER_COMPRESSION_METHOD_FIELD_BYTE = 1;
	static const DWORD CAR_HEADER_ORIGINAL_SIZE_FIELD_BYTE = 4;
	static const DWORD CAR_HEADER_COMPRESSED_SIZE_FIELD_BYTE = 4;
	static const DWORD CAR_HEADER_LAST_WRITE_TIME_FIELD_BYTE = 8;
	static const DWORD CAR_HEADER_FILE_CHECK_SUME_FIELD_BYTE = 4;
	static const DWORD CAR_HEADER_HEADERL_CHECK_SUME_FIELD_BYTE = 4;
	static const DWORD CAR_HEADER_BYTE = 
										CAR_HEADER_COMPRESSION_METHOD_FIELD_BYTE
										+ CAR_HEADER_ORIGINAL_SIZE_FIELD_BYTE
										+ CAR_HEADER_COMPRESSED_SIZE_FIELD_BYTE
										+ CAR_HEADER_LAST_WRITE_TIME_FIELD_BYTE
										+ CAR_HEADER_FILE_CHECK_SUME_FIELD_BYTE
										+ CAR_HEADER_HEADERL_CHECK_SUME_FIELD_BYTE;

public:
	CCompressedArchiverApp();
	std::wstring FormatErrorMessage(ULONG errorCode) const;
	DWORD Archive(const std::list<std::wstring>& fileList, COMPRESS_ALGORITHM compressAlgorithm, const std::wstring& carFileName);

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()

private:
	uint32_t GenerateHash32(const std::vector<char>& buffer, uint32_t seed = 0) const;
	DWORD InsertToArchiveFiles(const std::wstring& inputFileName, COMPRESS_ALGORITHM compressAlgorithm);
	void WriteFileHeader(CarHeader& header);
	std::string Utf16ToUtf8(const std::wstring& src) const;

};

extern CCompressedArchiverApp theApp;
