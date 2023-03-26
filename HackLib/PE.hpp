#pragma once

#include "Win32File.hpp"
#include "SHA256.hpp"

class PEFile : protected Win32File
{
public:
	PEFile(const std::filesystem::path& path);
	virtual ~PEFile() = default;

	IMAGE_OPTIONAL_HEADER32 OptionalHeader32() const;
	IMAGE_OPTIONAL_HEADER64 OptionalHeader64() const;

	inline IMAGE_FILE_HEADER FileHeader() const
	{
		return _fileHeader;
	}

	inline size_t OptionalHeaderStart() const
	{
		return _optionalHeaderOffset;
	}

	inline size_t OptionalHeaderEnd() const
	{
		return _optionalHeaderOffset + _fileHeader.SizeOfOptionalHeader;
	}

	SHA256 Checksum() const;

private:
	IMAGE_DOS_HEADER _dosHeader = {};
	IMAGE_FILE_HEADER _fileHeader = {};
	size_t _optionalHeaderOffset = 0;
};

class Executable : PEFile
{
public:
	Executable(const std::filesystem::path& path);
	~Executable() = default;

	std::vector<std::pair<std::string, std::string>> ImportedFunctions() const;
};

class Library : PEFile
{
public:
	Library(const std::filesystem::path& path);
	~Library() = default;

	std::vector<std::string> ExportedFunctions() const;
};