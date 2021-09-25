#include "pch.h"
#include "Compressor.h"
#include <compressapi.h>
#include <memory>

//éQçlÅFhttps://docs.microsoft.com/en-us/windows/win32/cmpapi/-compression-portal
/*
* MSZIP (COMPRESS_ALGORITHM_MSZIP)
	* Uses more resources than COMPRESS_ALGORITHM_XPRESS_HUFF
	* Generates a compressed block similar to RFC 1951.
	* Medium to high compression ratio
	* Medium compression speed and high decompression speed
	* Medium memory requirement

* XPRESS (COMPRESS_ALGORITHM_XPRESS)
	* Very fast with low resource requirements
	* Medium compression ratio
	* High compression and decompression speeds
	* Low memory requirement
	* Supports the COMPRESS_INFORMATION_CLASS_LEVEL option available in the COMPRESS_INFORMATION_CLASS enumeration. 
	  The default value is (DWORD)0. For some data, the value (DWORD)1 can improve the compression ratio with a slightly 
	  slower compression speed.

* XPRESS with Huffman encoding (COMPRESS_ALGORITHM_XPRESS_HUFF)
	* Compression ratio is higher than COMPRESS_ALGORITHM_XPRESS
	* Medium compression ratio
	* Medium to high compression and decompression speeds
	* Low memory requirement
	* Supports the COMPRESS_INFORMATION_CLASS_LEVEL option in the COMPRESS_INFORMATION_CLASS enumeration. 
	  The default value is (DWORD)0. For some data, the value (DWORD)1 can improve the compression ratio with a slightly 
	  slower compression speed.

* LZMS (COMPRESS_ALGORITHM_LZMS)
	* Good algorithm if the size of the data to be compressed is over 2MB.
	* High compression ratio
	* Low compression speed and high decompression speed
	* Medium to high memory requirement
	* Supports the COMPRESS_INFORMATION_CLASS_BLOCK_SIZE option in the COMPRESS_INFORMATION_CLASS enumeration.
	  A minimum size of 1 MB is suggested to get a better compression ratio.
*/

unsigned long Compressor::Compress(const std::vector<char>& src, std::vector<char>& dst) const
{
	COMPRESSOR_HANDLE tmpHandle = nullptr;
	if (!CreateCompressor(compressAlgorithmMap.at(m_algorithm), nullptr, &tmpHandle))
	{
		OutputDebugString(L"CreateCompressor failed\n");
		return GetLastError();
	}
	std::unique_ptr<std::remove_pointer<COMPRESSOR_HANDLE>::type, decltype(&CloseCompressor)> compressorHandle{ tmpHandle, CloseCompressor };

	SIZE_T compressBufferSize = 0;
	if (!::Compress(compressorHandle.get(), reinterpret_cast<LPCVOID>(src.data()), src.size(), nullptr, 0, &compressBufferSize))
	{
		if (auto errorCode = GetLastError(); errorCode != ERROR_INSUFFICIENT_BUFFER)
		{
			OutputDebugString(L"Compress failed\n");
			return errorCode;
		}
	}

	std::vector<char> compressData(compressBufferSize);
	SIZE_T compressDataSize = 0;
	if (!::Compress(compressorHandle.get(), reinterpret_cast<LPCVOID>(src.data()), src.size(), compressData.data(), compressBufferSize, &compressDataSize))
	{
		OutputDebugString(L"Compress failed\n");
		return GetLastError();
	}

	compressData.resize(compressDataSize);
	dst.swap(compressData);
	return ERROR_SUCCESS;
}

unsigned long Compressor::Decompress(const std::vector<char>& src, std::vector<char>& dst) const
{

	DECOMPRESSOR_HANDLE tmpHandle = nullptr;
	if (!CreateDecompressor(compressAlgorithmMap.at(m_algorithm), nullptr, &tmpHandle))
	{
		OutputDebugString(L"CreateDecompressor failed\n");
		return GetLastError();
	}

	std::unique_ptr<std::remove_pointer<DECOMPRESSOR_HANDLE>::type, decltype(&CloseDecompressor)> decompressorHandle{ tmpHandle, CloseDecompressor };
	SIZE_T decompressBufferSize = 0;
	if (!::Decompress(decompressorHandle.get(), reinterpret_cast<LPCVOID>(src.data()), src.size(), nullptr, 0, &decompressBufferSize))
	{
		if (auto errorCode = GetLastError(); errorCode != ERROR_INSUFFICIENT_BUFFER)
		{
			OutputDebugString(L"Decompress failed\n");
			return errorCode;
		}
	}

	std::vector<char> decompressData(decompressBufferSize);
	SIZE_T decompressDataSize = 0;
	if (!::Decompress(decompressorHandle.get(), reinterpret_cast<LPCVOID>(src.data()), src.size(), decompressData.data(), decompressBufferSize, &decompressDataSize))
	{
		OutputDebugString(L"Decompress failed\n");
		return GetLastError();
	}

	decompressData.resize(decompressDataSize);
	dst.swap(decompressData);
	return ERROR_SUCCESS;
}

const std::map<COMPRESS_ALGORITHM, DWORD> Compressor::compressAlgorithmMap =
{
	{COMPRESS_ALGORITHM::MSZIP, COMPRESS_ALGORITHM_MSZIP},
	{COMPRESS_ALGORITHM::XPRESS, COMPRESS_ALGORITHM_XPRESS},
	{COMPRESS_ALGORITHM::XPRESS_HUFF, COMPRESS_ALGORITHM_XPRESS_HUFF},
	{COMPRESS_ALGORITHM::LZMS, COMPRESS_ALGORITHM_LZMS},
};