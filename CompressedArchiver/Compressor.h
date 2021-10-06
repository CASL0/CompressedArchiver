#pragma once
#include <map>
#include <vector>

enum class COMPRESS_ALGORITHM
{
	MSZIP = 0,
	XPRESS,
	XPRESS_HUFF,
	LZMS,
	LZ4,
};

class Compressor
{
private:
	COMPRESS_ALGORITHM m_algorithm;
	static const std::map<COMPRESS_ALGORITHM, DWORD> compressAlgorithmMap;

public:
	Compressor(COMPRESS_ALGORITHM algorithm = COMPRESS_ALGORITHM::MSZIP) :m_algorithm(algorithm) {};
	unsigned long Compress(const std::vector<char>& src, std::vector<char>& dst) const;
	unsigned long Decompress(const std::vector<char>& src, std::vector<char>& dst) const;

private:
	unsigned long CompressByWin32(const std::vector<char>& src, std::vector<char>& dst) const;
	unsigned long DecompressByWin32(const std::vector<char>& src, std::vector<char>& dst) const;
	unsigned long CompressByLz4(const std::vector<char>& src, std::vector<char>& dst) const;
	unsigned long DecompressByLz4(const std::vector<char>& src, std::vector<char>& dst) const;
};