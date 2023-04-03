#pragma once

#include "Logger.hpp"
#include "SHA256.hpp"
#include "Win32File.hpp"

// https://en.wikibooks.org/wiki/X86_Disassembly/Windows_Executable_Files
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format

namespace MZ // https://wiki.osdev.org/MZ
{
	struct Header
	{
		uint16_t Signature = 0u;
		uint16_t ExtraBytes = 0u;
		uint16_t Pages = 0u;
		uint16_t RelocationCount = 0u;
		uint16_t ParagraphHeaderSize = 0u;
		uint16_t MinimumParagraphs = 0u;
		uint16_t MaximumParagraphs = 0u;
		uint16_t InitialSS = 0u;
		uint16_t InitialSP = 0u;
		uint16_t Checksum = 0u;
		uint16_t InitialIP = 0u;
		uint16_t InitialCS = 0u;
		uint16_t RelocationTableOffset = 0u;
		uint16_t OverlayNumber = 0u;

		static constexpr uint16_t ExpectedSignature = 0x5A4Du;
	};

	struct HeaderExtension // Windows specific
	{
		uint8_t Reserved1[8] = {};
		uint16_t OEMIdentifier = 0u;
		uint16_t OEMInfo = 0u;
		uint8_t Reserved2[20] = { };
		uint32_t COFFHeaderStart = 0u;
	};
}

namespace COFF // https://wiki.osdev.org/COFF
{
	struct Header
	{
		uint32_t Signature = 0u;
		uint16_t Machine = 0u;
		uint16_t NumberOfSections = 0u;
		uint32_t TimeDateStamp = 0u;
		uint32_t PointerToSymbolTable = 0u;
		uint32_t NumberOfSymbols = 0u;
		uint16_t SizeOfOptionalHeader = 0u;
		uint16_t Characteristics = 0u;

		static constexpr uint32_t ExpectedSignature = 0x00004550u;

		static constexpr uint16_t MachineI386 = 0x014Cu;
		static constexpr uint16_t MachineAMD64 = 0x8664u;

		static constexpr uint16_t ExecutableFlag = 0x0002u;
		static constexpr uint16_t LibraryFlag = 0x2000u;
	};

	struct OptionalHeader
	{
		uint16_t Signature = 0u;
		uint8_t MajorLinkerVersion = 0u;
		uint8_t MinorLinkerVersion = 0u;
		uint32_t SizeOfCode = 0u;
		uint32_t SizeOfInitializedData = 0u;
		uint32_t SizeOfUninitializedData = 0u;
		uint32_t AddressOfEntryPoint = 0u;
		uint32_t BaseOfCode = 0u;
		uint32_t BaseOfData = 0u;

		static constexpr uint32_t ExpectedSignaturePE32 = 0x10Bu;
		static constexpr uint32_t ExpectedSignaturePE32Plus = 0x20Bu;
	};

	template<typename T, class = typename std::enable_if<std::is_unsigned<T>::value>::type>
	struct OptionalHeaderExtension // Windows specific
	{
		T ImageBase = 0u;
		uint32_t SectionAlignment = 0u;
		uint32_t FileAlignment = 0u;
		uint16_t MajorOperatingSystemVersion = 0u;
		uint16_t MinorOperatingSystemVersion = 0u;
		uint16_t MajorImageVersion = 0u;
		uint16_t MinorImageVersion = 0u;
		uint16_t MajorSubsystemVersion = 0u;
		uint16_t MinorSubsystemVersion = 0u;
		uint32_t Win32VersionValue = 0u;
		uint32_t SizeOfImage = 0u;
		uint32_t SizeOfHeaders = 0u;
		uint32_t CheckSum = 0u;
		uint16_t Subsystem = 0u;
		uint16_t DllCharacteristics = 0u;
		T SizeOfStackReserve = 0u;
		T SizeOfStackCommit = 0u;
		T SizeOfHeapReserve = 0u;
		T SizeOfHeapCommit = 0u;
		uint32_t LoaderFlags = 0u;
		uint32_t NumberOfDataDirectories = 0u;
	};

	using OptionalHeaderExtensionX86 = OptionalHeaderExtension<uint32_t>;
	using OptionalHeaderExtensionX64 = OptionalHeaderExtension<uint64_t>;

	struct DataDirectory
	{
		uint32_t VirtualAddress = 0u;
		uint32_t Size = 0u;
	};

	enum DataDirectoryType : size_t
	{
		ExportTable = 0u,
		ImportTable,
		ResourceTable,
		ExceptionTable,
		CertificateTable,
		BaseRelocationTable,
		DebugInfo,
		ArchitectureData,
		GlobalPointer,
		ThreadLocalStorageTable,
		LoadConfigurationTable,
		BoundImportTable,
		ImportAddressTable,
		DelayImportDescriptor,
		CLRHeader,
		Reserved
	};

	struct SectionHeader
	{
		char Name[8] = {};
		uint32_t VirtualSize = 0u;
		uint32_t VirtualAddress = 0u;
		uint32_t SizeOfRawData = 0u;
		uint32_t PointerToRawData = 0u;
		uint32_t PointerToRelocations = 0u;
		uint32_t PointerToLinenumbers = 0u;
		uint16_t NumberOfRelocations = 0u;
		uint16_t NumberOfLinenumbers = 0u;
		uint32_t Characteristics = 0u;
	};
}

namespace PE // https://wiki.osdev.org/PE
{
	struct ImportDescriptor
	{
		uint32_t OriginalFirstThunk = 0u;
		uint32_t TimeDateStamp = 0u;
		uint32_t ForwarderChain = 0u;
		uint32_t Name = 0u;
		uint32_t FirstThunk = 0u;
	};

	struct ExportDirectory
	{
		uint32_t Characteristics = 0u;
		uint32_t TimeDateStamp = 0u;
		uint16_t MajorVersion = 0u;
		uint16_t MinorVersion = 0u;
		uint32_t Name = 0u;
		uint32_t Base = 0u;
		uint32_t NumberOfFunctions = 0u;
		uint32_t NumberOfNames = 0u;
		uint32_t AddressOfFunctions = 0u;
		uint32_t AddressOfNames = 0u;
		uint32_t AddressOfNameOrdinals = 0u;
	};
}

// https://learn.microsoft.com/en-us/previous-versions/ms809762(v=msdn.10)?redirectedfrom=MSDN
class PEFile : protected Win32File
{
public:
	PEFile(const std::filesystem::path& path);
	virtual ~PEFile();

	SHA256 Checksum() const;

protected:
	MZ::Header _mzHeader;
	MZ::HeaderExtension _mzExtension;

	COFF::Header _coffHeader;
	COFF::OptionalHeader _coffOptionalHeader;
	
	std::vector<COFF::DataDirectory> _dataDirectories;
	std::vector<COFF::SectionHeader> _sectionHeaders;

	void* _optionalHeaderExtension = nullptr;

private:
	template <typename T>
	size_t ReadCOFFOptionalHeaderExtension()
	{
		using HT = COFF::OptionalHeaderExtension<T>;

		_optionalHeaderExtension = new HT();

		[[maybe_unused]]
		size_t bytesRead = Read(_optionalHeaderExtension, sizeof(HT));
		_ASSERT(bytesRead == sizeof(HT));

		auto ohe = reinterpret_cast<HT*>(_optionalHeaderExtension);

		LogDebug << *ohe;

		return ohe->NumberOfDataDirectories;
	}
};

class Executable : public PEFile
{
public:
	Executable(const std::filesystem::path& path);
	~Executable() = default;

	std::vector<std::pair<std::string, std::string>> ImportedFunctions() const;
};

class Library : public PEFile
{
public:
	Library(const std::filesystem::path& path);
	~Library() = default;

	std::vector<std::string> ExportedFunctions() const;
};