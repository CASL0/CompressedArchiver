#include "pch.h"
#include "Compressor.h"
#include "lz4.h"
#include "zstd.h"
#include <compressapi.h>
#include <memory>
#include <string>

//?Q?l?Fhttps://docs.microsoft.com/en-us/windows/win32/cmpapi/-compression-portal
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
	switch (m_algorithm)
	{
	case COMPRESS_ALGORITHM::MSZIP:
	case COMPRESS_ALGORITHM::XPRESS:
	case COMPRESS_ALGORITHM::XPRESS_HUFF:
	case COMPRESS_ALGORITHM::LZMS:
		return CompressByWin32(src, dst);
	case COMPRESS_ALGORITHM::LZ4:
		return CompressByLz4(src, dst);
	case COMPRESS_ALGORITHM::ZSTD:
		return CompressByZstd(src, dst);
	default:
		break;
	}
	return ERROR_SUCCESS;
}

unsigned long Compressor::Compress(const std::vector<char>& src, std::vector<char>& dst, size_t dstSize) const
{
	dst.resize(dstSize);
	return Compress(src, dst);
}

unsigned long Compressor::Decompress(const std::vector<char>& src, std::vector<char>& dst) const
{
	OutputDebugString(L"algorithm: ");
	OutputDebugString(std::to_wstring(static_cast<int>(m_algorithm)).c_str());
	OutputDebugString(L"\n");
	switch (m_algorithm)
	{
	case COMPRESS_ALGORITHM::MSZIP:
	case COMPRESS_ALGORITHM::XPRESS:
	case COMPRESS_ALGORITHM::XPRESS_HUFF:
	case COMPRESS_ALGORITHM::LZMS:
		return DecompressByWin32(src, dst);
	case COMPRESS_ALGORITHM::LZ4:
		return DecompressByLz4(src, dst);
	case COMPRESS_ALGORITHM::ZSTD:
		return DecompressByZstd(src, dst);
	default:
		break;
	}
	return ERROR_SUCCESS;

}

unsigned long Compressor::Decompress(const std::vector<char>& src, std::vector<char>& dst, size_t dstSize) const
{
	dst.resize(dstSize);
	return Decompress(src, dst);
}

unsigned long Compressor::CompressByWin32(const std::vector<char>& src, std::vector<char>& dst) const
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

unsigned long Compressor::DecompressByWin32(const std::vector<char>& src, std::vector<char>& dst) const
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

unsigned long Compressor::CompressByLz4(const std::vector<char>& src, std::vector<char>& dst) const
{
	auto dstCapacity = LZ4_compressBound(static_cast<int>(src.size()));
	dst.resize(dstCapacity);
	auto dstSize = LZ4_compress_default(src.data(), dst.data(), static_cast<int>(src.size()), dstCapacity);
	dst.resize(dstSize);
	return ERROR_SUCCESS;
}

unsigned long Compressor::DecompressByLz4(const std::vector<char>& src, std::vector<char>& dst) const
{

	//?????L???p?V?e?B?̗??_?l?Q?l?Fhttps://stackoverflow.com/questions/42114762/how-to-know-when-the-output-buffer-is-too-small-when-decompressing-with-lz4
	int dstCapacity = 255 * static_cast<int>(src.size()) - 2526;
	dst.resize(dstCapacity);
	auto dstSize = LZ4_decompress_safe(src.data(), dst.data(), static_cast<int>(src.size()), dstCapacity);
	dst.resize(dstSize);
	return ERROR_SUCCESS;
}

unsigned long Compressor::CompressByZstd(const std::vector<char>& src, std::vector<char>& dst) const
{
	auto dstCapacity = ZSTD_compressBound(src.size());
	dst.resize(dstCapacity);
	auto dstSize = ZSTD_compress(dst.data(), dstCapacity, src.data(), src.size(), ZSTD_CLEVEL_DEFAULT);
	dst.resize(dstSize);
	return ERROR_SUCCESS;
}

unsigned long Compressor::DecompressByZstd(const std::vector<char>& src, std::vector<char>& dst) const
{
	//dst??alloc?ς?
	auto dstSize = ZSTD_decompress(dst.data(), dst.size(), src.data(), src.size());
	dst.resize(dstSize);
	return ERROR_SUCCESS;
}

const std::map<COMPRESS_ALGORITHM, DWORD> Compressor::compressAlgorithmMap =
{
	{COMPRESS_ALGORITHM::MSZIP, COMPRESS_ALGORITHM_MSZIP},
	{COMPRESS_ALGORITHM::XPRESS, COMPRESS_ALGORITHM_XPRESS},
	{COMPRESS_ALGORITHM::XPRESS_HUFF, COMPRESS_ALGORITHM_XPRESS_HUFF},
	{COMPRESS_ALGORITHM::LZMS, COMPRESS_ALGORITHM_LZMS},
};