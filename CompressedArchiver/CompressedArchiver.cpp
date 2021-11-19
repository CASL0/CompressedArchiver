
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
		OutputDebugString(elem.c_str());
		OutputDebugString(L"\n");
		if (auto ret = InsertToArchiveFiles(elem, compressAlgorithm); ret != ERROR_SUCCESS)
		{
			OutputDebugString(FormatErrorMessage(ret).c_str());
			m_outputCarFile.Close();
			return ret;
		}
	}
	WriteEndOfCarHeader();
	m_outputCarFile.Close();
	return ERROR_SUCCESS;
}

DWORD CCompressedArchiverApp::GetHeaderList(const std::wstring& inputCarFileName, std::list<CarHeader>& headerList) const
{
	CFile inputCarFile;
	if (CFileException ex; !inputCarFile.Open(inputCarFileName.c_str(), CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite))
	{
		OutputDebugString(L"File open error: ");
		OutputDebugString(ex.m_strFileName);
		OutputDebugString(L"\n");
		return ex.m_lOsError;
	}

	std::list<CarHeader> tmpList;
	for (;;)
	{
		CarHeader header;
		if (!ReadFileHeader(inputCarFile, &header))
		{
			break;
		}

		tmpList.push_back(header);
	}

	headerList.swap(tmpList);
	return ERROR_SUCCESS;
}

int CCompressedArchiverApp::CompressionRatio(uint32_t originalSize, uint32_t compressedSize) const
{
	if (originalSize == 0)
	{
		return 0;
	}

	return static_cast<int>((1 - static_cast<double>(compressedSize) / originalSize) * 100);
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
	header.lastWriteTime = (static_cast<uint64_t>(fileAttr.ftLastWriteTime.dwHighDateTime) << 32) | fileAttr.ftLastWriteTime.dwLowDateTime;

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
	WriteFileHeader(header);
	m_outputCarFile.Write(compressedData.data(), static_cast<UINT>(compressedData.size()));
	
	return ERROR_SUCCESS;
}

void CCompressedArchiverApp::WriteFileHeader(CarHeader& header)
{
	m_outputCarFile.Write(header.fileName.c_str(), static_cast<UINT>(header.fileName.length()));

	//ヘッダーのチェックサムはxxHash(ヘッダー全体,xxHash(ファイル名,0))
	auto headerCheckSum = GenerateHash32(std::vector<char>(header.fileName.c_str(), header.fileName.c_str() + header.fileName.length() - 1));

	std::vector<uint8_t> headerData(CAR_HEADER_BYTE);//チェックサム用のヘッダバッファ
	DWORD offset = 0;

	//圧縮アルゴリズム
	OutputDebugString(L"compression algorithm: ");
	OutputDebugString(std::to_wstring(header.compressionMethod).c_str());
	OutputDebugString(L"\n");
	PackByteAlignment(static_cast<uint32_t>(header.compressionMethod), CAR_HEADER_COMPRESSION_METHOD_FIELD_BYTE, headerData, offset);
	offset += CAR_HEADER_COMPRESSION_METHOD_FIELD_BYTE;

	//元のファイルサイズ
	OutputDebugString(L"original size: ");
	OutputDebugString(std::to_wstring(header.originalSize).c_str());
	OutputDebugString(L"\n");
	PackByteAlignment(header.originalSize, CAR_HEADER_ORIGINAL_SIZE_FIELD_BYTE, headerData, offset);
	offset += CAR_HEADER_ORIGINAL_SIZE_FIELD_BYTE;

	//圧縮後のファイルサイズ
	OutputDebugString(L"compressed size: ");
	OutputDebugString(std::to_wstring(header.compressedSize).c_str());
	OutputDebugString(L"\n");
	PackByteAlignment(header.compressedSize, CAR_HEADER_COMPRESSED_SIZE_FIELD_BYTE, headerData, offset);
	offset += CAR_HEADER_COMPRESSED_SIZE_FIELD_BYTE;

	//最終更新日時
	OutputDebugString(L"last write date: ");
	OutputDebugString(std::to_wstring(header.lastWriteTime).c_str());
	OutputDebugString(L"\n");
	PackByteAlignment(header.lastWriteTime, CAR_HEADER_LAST_WRITE_TIME_FIELD_BYTE, headerData, offset);
	offset += CAR_HEADER_LAST_WRITE_TIME_FIELD_BYTE;

	//ファイルのチェックサム
	OutputDebugString(L"file check sume: ");
	OutputDebugString(std::to_wstring(header.fileCheckSum).c_str());
	OutputDebugString(L"\n");
	PackByteAlignment(header.fileCheckSum, CAR_HEADER_FILE_CHECK_SUM_FIELD_BYTE, headerData, offset);
	offset += CAR_HEADER_FILE_CHECK_SUM_FIELD_BYTE;

	//ヘッダーのチェックサム
	header.headerCheckSum = GenerateHash32(std::vector<char>(headerData.front(), headerData.back()), headerCheckSum);
	OutputDebugString(L"header check sum: ");
	OutputDebugString(std::to_wstring(header.headerCheckSum).c_str());
	OutputDebugString(L"\n");
	m_outputCarFile.Write(headerData.data(), CAR_HEADER_BYTE);
	m_outputCarFile.Write(&header.headerCheckSum, CAR_HEADER_HEADER_CHECK_SUM_FIELD_BYTE);

}

void CCompressedArchiverApp::WriteEndOfCarHeader()
{
	m_outputCarFile.Write(&CAR_TERMINATOR, 1);
}

bool CCompressedArchiverApp::ReadFileHeader(CFile& inputCarFile, CarHeader* header) const
{
	CarHeader tmpHeader;
	for (;;)
	{
		char c = 0;
		(void)inputCarFile.Read(&c, 1);
		if (c == CAR_TERMINATOR)
		{
			break;
		}
		tmpHeader.fileName.push_back(c);
	}

	//ヘッダー終端文字の場合はfalseを返す
	if (tmpHeader.fileName.empty())
	{
		return false;
	}

	std::vector<uint8_t> headerData(CAR_HEADER_BYTE);
	(void)inputCarFile.Read(headerData.data(), CAR_HEADER_BYTE);
	size_t offset = 0;

	tmpHeader.compressionMethod = static_cast<uint8_t>(UnpackByteAlignment(headerData, CAR_HEADER_COMPRESSION_METHOD_FIELD_BYTE, offset));
	offset += CAR_HEADER_COMPRESSION_METHOD_FIELD_BYTE;

	tmpHeader.originalSize = static_cast<uint32_t>(UnpackByteAlignment(headerData, CAR_HEADER_ORIGINAL_SIZE_FIELD_BYTE, offset));
	offset += CAR_HEADER_ORIGINAL_SIZE_FIELD_BYTE;

	tmpHeader.compressedSize = static_cast<uint32_t>(UnpackByteAlignment(headerData, CAR_HEADER_COMPRESSED_SIZE_FIELD_BYTE, offset));
	offset += CAR_HEADER_COMPRESSED_SIZE_FIELD_BYTE;

	tmpHeader.lastWriteTime = UnpackByteAlignment(headerData, CAR_HEADER_LAST_WRITE_TIME_FIELD_BYTE, offset);
	offset += CAR_HEADER_LAST_WRITE_TIME_FIELD_BYTE;

	tmpHeader.fileCheckSum = static_cast<uint32_t>(UnpackByteAlignment(headerData, CAR_HEADER_FILE_CHECK_SUM_FIELD_BYTE, offset));
	offset += CAR_HEADER_FILE_CHECK_SUM_FIELD_BYTE;

	(void)inputCarFile.Read(&tmpHeader.headerCheckSum, CAR_HEADER_HEADER_CHECK_SUM_FIELD_BYTE);

	auto checkSum = GenerateHash32(std::vector<char>(headerData.front(), headerData.back()), GenerateHash32(std::vector<char>(tmpHeader.fileName.c_str(), tmpHeader.fileName.c_str() + tmpHeader.fileName.length())));
	if (checkSum != tmpHeader.headerCheckSum)
	{
		OutputDebugString(L"不一致\n");
		PostMessage(m_pMainWnd->m_hWnd, APP_MESSAGE_BROKEN_PACKAGE, reinterpret_cast<WPARAM>(Utf8ToUtf16(tmpHeader.fileName).c_str()), 0);
	}

	(void)inputCarFile.Seek(tmpHeader.compressedSize, CFile::current);
	if (header)
	{
		*header = tmpHeader;
	}

	return true;
}

void CCompressedArchiverApp::PackByteAlignment(uint64_t src, size_t numBytes, std::vector<uint8_t>& dst, size_t offset) const
{
	for (auto i = 0; i < numBytes; i++)
	{
		//下位1バイトをバッファに格納
		dst.at(offset + i) = src & 0xff;
		src >>= 8;
	}
}

uint64_t CCompressedArchiverApp::UnpackByteAlignment(const std::vector<uint8_t>& src, size_t numBytes, size_t offset) const
{
	uint64_t result = 0;
	auto shiftCount = 0;
	for (auto i = 0; i < numBytes; i++)
	{
		result |= static_cast<uint64_t>(src.at(offset + i)) << shiftCount;
		shiftCount += 8;
	}
	return result;
}

std::string CCompressedArchiverApp::Utf16ToUtf8(const std::wstring& src) const
{
	auto bufLen = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::vector<char> buffer(bufLen);
	(void)WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, buffer.data(), bufLen, nullptr, nullptr);
	return std::string(buffer.data(), bufLen);
}

std::wstring CCompressedArchiverApp::Utf8ToUtf16(const std::string& src) const
{
	auto bufLen = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, nullptr, 0);
	std::vector<wchar_t> buffer(bufLen);
	(void)MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, buffer.data(), bufLen);
	return std::wstring(buffer.data(), bufLen);
}

CFileTime CCompressedArchiverApp::Uint64ToFileTime(uint64_t fileTimeNumber) const
{
	auto low32Part = ((uint64_t)1 << 32) - 1;
	CFileTime fileTime;
	fileTime.dwLowDateTime = static_cast<DWORD>(fileTimeNumber & low32Part);
	fileTimeNumber >>= 32;
	fileTime.dwHighDateTime = static_cast<DWORD>(fileTimeNumber);
	return fileTime;
}

void CCompressedArchiverApp::ShowErrorMessage(const std::wstring& message) const
{
	(void)AfxMessageBox(message.c_str(), MB_ICONSTOP | MB_SETFOREGROUND);
}

void CCompressedArchiverApp::ShowErrorMessage(UINT resourceId, DWORD errorCode) const
{
	CString errorMessage;
	errorMessage.Format(resourceId, errorCode, FormatErrorMessage(errorCode).c_str());
	ShowErrorMessage(errorMessage.GetString());
}

void CCompressedArchiverApp::ShowErrorMessage(UINT resourceId) const
{
	CString errorMessage;
	(void)errorMessage.LoadStringW(IDS_ERROR_NO_ITEMS_TO_CREATE);
	ShowErrorMessage(errorMessage.GetString());
}