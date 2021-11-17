
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
public:
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
	static const DWORD CAR_HEADER_FILE_CHECK_SUM_FIELD_BYTE = 4;
	static const DWORD CAR_HEADER_HEADER_CHECK_SUM_FIELD_BYTE = 4;
	static const DWORD CAR_HEADER_BYTE =
										CAR_HEADER_COMPRESSION_METHOD_FIELD_BYTE
										+ CAR_HEADER_ORIGINAL_SIZE_FIELD_BYTE
										+ CAR_HEADER_COMPRESSED_SIZE_FIELD_BYTE
										+ CAR_HEADER_LAST_WRITE_TIME_FIELD_BYTE
										+ CAR_HEADER_FILE_CHECK_SUM_FIELD_BYTE;
	static const char CAR_TERMINATOR = 0;

	static const UINT APP_MESSAGE_BROKEN_PACKAGE = WM_APP + 1;
	static const UINT APP_MESSAGE_FAILURE_COMPRESS = APP_MESSAGE_BROKEN_PACKAGE + 1;

public:
	CCompressedArchiverApp();
	std::wstring FormatErrorMessage(ULONG errorCode) const;
	DWORD Archive(const std::list<std::wstring>& fileList, COMPRESS_ALGORITHM compressAlgorithm, const std::wstring& carFileName);
	DWORD GetHeaderList(const std::wstring& inputCarFileName, std::list<CarHeader>& headerList) const;
	int CompressionRatio(uint32_t originalSize, uint32_t compressedSize) const;
	std::string Utf16ToUtf8(const std::wstring& src) const;
	std::wstring Utf8ToUtf16(const std::string& src) const;
	CFileTime Uint64ToFileTime(uint64_t fileTimeNumber) const;
	void ShowErrorMessage(const std::wstring& message) const;
	void ShowErrorMessage(UINT resourceId, DWORD errorCode) const;

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()

private:
	/*
	* @brief xxHashのハッシュ値を生成する
	* @param[in] buffer ハッシュ化したいバイナリ
	* @param[in] seed ハッシュのシード
	* @return ハッシュ値
	*/
	uint32_t GenerateHash32(const std::vector<char>& buffer, uint32_t seed = 0) const;

	/*
	* @breif ファイルから読み込み、圧縮後にヘッダーとともに書き込む
	* @param[in] inputFileName 読み込むファイル名
	* @param[in] compressAlgorithm 圧縮アルゴリズム
	* @return GetLastErrorの戻り値
	*/
	DWORD InsertToArchiveFiles(const std::wstring& inputFileName, COMPRESS_ALGORITHM compressAlgorithm);

	/*
	* @brief ヘッダーを書き込む
	* @param[in] header 書き込むヘッダー
	*/
	void WriteFileHeader(CarHeader& header);

	/*
	* @brief ヘッダーの終端文字を出力する
	*/
	void WriteEndOfCarHeader();

	/*
	* @brief ヘッダーを読み込む
	* @param[in,out] inputCarFile 読み込み中のCARファイルハンドル
	* @param[out] header 読み込んだヘッダー 
	* @return ファイルを読み込んだ-->true  終端-->false
	*/
	bool ReadFileHeader(CFile& inputCarFile, CarHeader* header) const;

	/*
	* @brief 1バイトずつバッファーに整列する
	* @param[in] src バイト列
	* @param[in] numBytes バイト数
	* @param[out] dst 整列後のバイト列を格納するバッファー
	* @param[in] 格納したいバッファーの先頭へのオフセット
	*/
	void PackByteAlignment(uint64_t src, size_t numBytes, std::vector<uint8_t>& dst, size_t offset) const;

	/*
	* @param[in] src バイト列
	* @param[in] numBytes 取り出すバイト列
	* @param[in] offset 読み取る先頭へのオフセット
	* @return バッファから取り出したバイト列
	*/
	uint64_t UnpackByteAlignment(const std::vector<uint8_t>& src, size_t numBytes, size_t offset) const;
};

extern CCompressedArchiverApp theApp;
