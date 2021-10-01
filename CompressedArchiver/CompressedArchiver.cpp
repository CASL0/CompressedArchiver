
// CompressedArchiver.cpp : アプリケーションのクラス動作を定義します。
//

#include "pch.h"
#include "framework.h"
#include "CompressedArchiver.h"
#include "CompressedArchiverDlg.h"
#include "xxhash.h"
#include <memory>
#include <vector>
#include <random>

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

uint32_t CCompressedArchiverApp::GenerateHash32(const std::vector<char>& buffer, uint32_t seed) const
{
	return XXH32(buffer.data(), buffer.size(), seed);
}

DWORD CCompressedArchiverApp::Archive(const std::list<std::wstring>& fileList, COMPRESS_ALGORITHM compressAlgorithm, const std::wstring& carFileName)
{
	if (CFileException ex; !m_outputCarFile.Open(carFileName.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyWrite, &ex))
	{
		OutputDebugString(L"File open error: ");
		OutputDebugString(ex.m_strFileName);
		OutputDebugString(L"\n");
		return ex.m_lOsError;
	}

	for (const auto& elem : fileList)
	{
		if (auto ret = InsertToArchiveFiles(elem, compressAlgorithm); ret != ERROR_SUCCESS)
		{
			OutputDebugString(FormatErrorMessage(ret).c_str());
			m_outputCarFile.Close();
			return ret;
		}
	}

	m_outputCarFile.Close();
	return ERROR_SUCCESS;
}

DWORD CCompressedArchiverApp::InsertToArchiveFiles(const std::wstring& inputFileName, COMPRESS_ALGORITHM compressAlgorithm)
{
	CFile inputFile;
	if (CFileException ex; !inputFile.Open(inputFileName.c_str(), CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite))
	{
		OutputDebugString(L"File open error: ");
		OutputDebugString(ex.m_strFileName);
		OutputDebugString(L"\n");
		return ex.m_lOsError;
	}

	CarHeader header;
	header.fileName = Utf16ToUtf8(inputFile.GetFileName().GetString());
	header.compressionMethod = static_cast<uint8_t>(compressAlgorithm);
	header.originalSize = static_cast<uint32_t>(inputFile.GetLength());

	WIN32_FILE_ATTRIBUTE_DATA fileAttr = { 0 };
	if (!GetFileAttributesEx(inputFileName.c_str(), GetFileExInfoStandard, &fileAttr))
	{
		OutputDebugString(L"GetFileAttributesEx failed\n");
		return GetLastError();
	}
	header.lastWriteTime = fileAttr.ftLastWriteTime.dwHighDateTime | fileAttr.ftLastWriteTime.dwLowDateTime;

	//ヘッダーデータは後で書き込むためヘッダーの先頭位置を保存
	//圧縮後のサイズとCRCは後で書き込む
	auto savedPosition = m_outputCarFile.GetPosition();
	WriteFileHeader(header);

	std::vector<char> inputBuffer(header.originalSize);
	auto bufLen = inputFile.Read(inputBuffer.data(), header.originalSize);
	inputBuffer.resize(bufLen);
	std::vector<char> compressedData;
	Compressor compressor(compressAlgorithm);
	if (auto ret = compressor.Compress(inputBuffer, compressedData); ret != ERROR_SUCCESS)
	{
		OutputDebugString(L"圧縮処理に失敗\n");
		return ret;
	}

	header.compressedSize = static_cast<uint32_t>(compressedData.size());
	header.fileCheckSum = GenerateHash32(inputBuffer);
	m_outputCarFile.Write(compressedData.data(), static_cast<UINT>(compressedData.size()));
	m_outputCarFile.Seek(savedPosition, CFile::begin());
	WriteFileHeader(header);
	m_outputCarFile.SeekToEnd();
	
	return ERROR_SUCCESS;
}

void CCompressedArchiverApp::WriteFileHeader(CarHeader& header)
{
	m_outputCarFile.Write(header.fileName.c_str(), static_cast<UINT>(header.fileName.length()));
	uint8_t zero = 0;
	m_outputCarFile.Write(&zero, 1);

	//ヘッダーのチェックサムはxxHash(ヘッダー全体,xxHash(ファイル名,0))
	auto headerCheckSum = GenerateHash32(std::vector<char>(header.fileName.c_str(), header.fileName.c_str() + header.fileName.length()));

	std::vector<uint8_t> headerData(CAR_HEADER_BYTE);//チェックサム用のヘッダバッファ
	DWORD offset = 0;

	//圧縮アルゴリズム
	m_outputCarFile.Write(&header.compressionMethod, CAR_HEADER_COMPRESSION_METHOD_FIELD_BYTE);
	std::memcpy(headerData.data(), &header.compressionMethod, CAR_HEADER_COMPRESSION_METHOD_FIELD_BYTE);
	offset += CAR_HEADER_COMPRESSION_METHOD_FIELD_BYTE;

	//元のファイルサイズ
	m_outputCarFile.Write(&header.originalSize, CAR_HEADER_ORIGINAL_SIZE_FIELD_BYTE);
	std::memcpy(headerData.data() + offset , &header.originalSize, CAR_HEADER_ORIGINAL_SIZE_FIELD_BYTE);
	offset += CAR_HEADER_ORIGINAL_SIZE_FIELD_BYTE;

	//圧縮後のファイルサイズ
	m_outputCarFile.Write(&header.compressedSize, CAR_HEADER_COMPRESSED_SIZE_FIELD_BYTE);
	std::memcpy(headerData.data() + offset , &header.compressedSize, CAR_HEADER_COMPRESSED_SIZE_FIELD_BYTE);
	offset += CAR_HEADER_COMPRESSED_SIZE_FIELD_BYTE;

	//最終更新日時
	m_outputCarFile.Write(&header.lastWriteTime, CAR_HEADER_LAST_WRITE_TIME_FIELD_BYTE);
	std::memcpy(headerData.data() + offset, &header.lastWriteTime, CAR_HEADER_LAST_WRITE_TIME_FIELD_BYTE);
	offset += CAR_HEADER_LAST_WRITE_TIME_FIELD_BYTE;

	//ファイルのチェックサム
	m_outputCarFile.Write(&header.fileCheckSum, CAR_HEADER_FILE_CHECK_SUME_FIELD_BYTE);
	std::memcpy(headerData.data() + offset, &header.fileCheckSum, CAR_HEADER_FILE_CHECK_SUME_FIELD_BYTE);
	offset += CAR_HEADER_FILE_CHECK_SUME_FIELD_BYTE;

	//ヘッダーのチェックサム
	header.headerCheckSum = GenerateHash32(std::vector<char>(headerData.front(), headerData.back()), headerCheckSum);
	m_outputCarFile.Write(&header.headerCheckSum, CAR_HEADER_HEADERL_CHECK_SUME_FIELD_BYTE);

}

std::string CCompressedArchiverApp::Utf16ToUtf8(const std::wstring& src) const
{
	auto bufLen = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::vector<char> buffer(bufLen);
	(void)WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, buffer.data(), bufLen, nullptr, nullptr);
	return std::string(buffer.data(), bufLen);
}