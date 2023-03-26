#include "PE.hpp"
#include "Exceptions.hpp"
#include "Win32File.hpp"
#include "Pointer.hpp"
#include "Logger.hpp"

inline std::ostream& operator << (std::ostream& os, const IMAGE_DOS_HEADER& idh)
{
	os << "\nIMAGE_DOS_HEADER:" << std::endl;
	os << std::format("e_magic:    0x{:04X}", idh.e_magic) << std::endl;
	os << std::format("e_cblp:     0x{:04X}", idh.e_cblp) << std::endl;
	os << std::format("e_cp:       0x{:04X}", idh.e_cp) << std::endl;
	os << std::format("e_crlc:     0x{:04X}", idh.e_crlc) << std::endl;
	os << std::format("e_cparhdr:  0x{:04X}", idh.e_cparhdr) << std::endl;
	os << std::format("e_minalloc: 0x{:04X}", idh.e_minalloc) << std::endl;
	os << std::format("e_maxalloc: 0x{:04X}", idh.e_maxalloc) << std::endl;
	os << std::format("e_csum:     0x{:04X}", idh.e_csum) << std::endl;
	os << std::format("e_ip:       0x{:04X}", idh.e_ip) << std::endl;
	os << std::format("e_cs:       0x{:04X}", idh.e_cs) << std::endl;
	os << std::format("e_lfarlc:   0x{:04X}", idh.e_lfarlc) << std::endl;
	os << std::format("e_ovno:     0x{:04X}", idh.e_ovno) << std::endl;
	os << std::format("e_res[0]    0x{:04X}", idh.e_res[0]) << std::endl;
	os << std::format("e_res[1]:   0x{:04X}", idh.e_res[1]) << std::endl;
	os << std::format("e_res[2]:   0x{:04X}", idh.e_res[2]) << std::endl;
	os << std::format("e_res[3]:   0x{:04X}", idh.e_res[3]) << std::endl;
	os << std::format("e_oemid:    0x{:04X}", idh.e_oemid) << std::endl;
	os << std::format("e_oeminfo:  0x{:04X}", idh.e_oeminfo) << std::endl;
	os << std::format("e_res2[0]   0x{:04X}", idh.e_res2[0]) << std::endl;
	os << std::format("e_res2[1]   0x{:04X}", idh.e_res2[1]) << std::endl;
	os << std::format("e_res2[2]   0x{:04X}", idh.e_res2[2]) << std::endl;
	os << std::format("e_res2[3]   0x{:04X}", idh.e_res2[3]) << std::endl;
	os << std::format("e_res2[4]   0x{:04X}", idh.e_res2[4]) << std::endl;
	os << std::format("e_res2[5]   0x{:04X}", idh.e_res2[5]) << std::endl;
	os << std::format("e_res2[6]   0x{:04X}", idh.e_res2[6]) << std::endl;
	os << std::format("e_res2[7]   0x{:04X}", idh.e_res2[7]) << std::endl;
	os << std::format("e_lfanew    0x{:04X}", idh.e_lfanew);

	return os;
}

inline std::ostream& operator << (std::ostream& os, const IMAGE_FILE_HEADER& ifh)
{
	os << "\nIMAGE_FILE_HEADER:" << std::endl;
	os << std::format("Machine:              0x{:04X}", ifh.Machine) << std::endl;
	os << std::format("NumberOfSections:     0x{:04X}", ifh.NumberOfSections) << std::endl;
	os << std::format("TimeDateStamp:        0x{:08X}", ifh.TimeDateStamp) << std::endl;
	os << std::format("PointerToSymbolTable: 0x{:08X}", ifh.PointerToSymbolTable) << std::endl;
	os << std::format("NumberOfSymbols:      0x{:08X}", ifh.NumberOfSymbols) << std::endl;
	os << std::format("SizeOfOptionalHeader: 0x{:04X}", ifh.SizeOfOptionalHeader) << std::endl;
	os << std::format("Characteristics:      0x{:04X}", ifh.Characteristics) << std::endl;

	return os;
}

inline std::ostream& operator << (std::ostream& os, const IMAGE_DATA_DIRECTORY& idd)
{
	os << "\nIMAGE_DATA_DIRECTORY:" << std::endl;
	os << std::format("VirtualAddress: 0x{:08X}", idd.VirtualAddress) << std::endl;
	os << std::format("Size:           0x{:08X}", idd.Size) << std::endl;
	
	return os;
}

inline std::ostream& operator << (std::ostream& os, const IMAGE_OPTIONAL_HEADER32& ioh)
{
	os << "\nIMAGE_OPTIONAL_HEADER32:" << std::endl;
	os << std::format("Magic:                            0x{:04X}", ioh.Magic) << std::endl;
	os << std::format("MajorLinkerVersion:               0x{:02X}", ioh.MajorLinkerVersion) << std::endl;
	os << std::format("MinorLinkerVersion:               0x{:02X}", ioh.MinorLinkerVersion) << std::endl;
	os << std::format("SizeOfCode:                       0x{:08X}", ioh.SizeOfCode) << std::endl;
	os << std::format("SizeOfInitializedData:            0x{:08X}", ioh.SizeOfInitializedData) << std::endl;
	os << std::format("SizeOfUninitializedData:          0x{:08X}", ioh.SizeOfUninitializedData) << std::endl;
	os << std::format("AddressOfEntryPoint:              0x{:08X}", ioh.AddressOfEntryPoint) << std::endl;
	os << std::format("BaseOfCode:                       0x{:08X}", ioh.BaseOfCode) << std::endl;
	os << std::format("BaseOfData:                       0x{:08X}", ioh.BaseOfData) << std::endl;
	os << std::format("ImageBase:                        0x{:08X}", ioh.ImageBase) << std::endl;
	os << std::format("SectionAlignment:                 0x{:08X}", ioh.SectionAlignment) << std::endl;
	os << std::format("FileAlignment:                    0x{:08X}", ioh.FileAlignment) << std::endl;
	os << std::format("MajorOperatingSystemVersion:      0x{:04X}", ioh.MajorOperatingSystemVersion) << std::endl;
	os << std::format("MinorOperatingSystemVersion:      0x{:04X}", ioh.MinorOperatingSystemVersion) << std::endl;
	os << std::format("MajorImageVersion:                0x{:04X}", ioh.MajorImageVersion) << std::endl;
	os << std::format("MinorImageVersion:                0x{:04X}", ioh.MinorImageVersion) << std::endl;
	os << std::format("MajorSubsystemVersion:            0x{:04X}", ioh.MajorSubsystemVersion) << std::endl;
	os << std::format("MinorSubsystemVersion:            0x{:04X}", ioh.MinorSubsystemVersion) << std::endl;
	os << std::format("Win32VersionValue:                0x{:08X}", ioh.Win32VersionValue) << std::endl;
	os << std::format("SizeOfImage:                      0x{:08X}", ioh.SizeOfImage) << std::endl;
	os << std::format("SizeOfHeaders:                    0x{:08X}", ioh.SizeOfHeaders) << std::endl;
	os << std::format("CheckSum:                         0x{:08X}", ioh.CheckSum) << std::endl;
	os << std::format("Subsystem:                        0x{:04X}", ioh.Subsystem) << std::endl;
	os << std::format("DllCharacteristics:               0x{:04X}", ioh.DllCharacteristics) << std::endl;
	os << std::format("SizeOfStackReserve:               0x{:08X}", ioh.SizeOfStackReserve) << std::endl;
	os << std::format("SizeOfStackCommit:                0x{:08X}", ioh.SizeOfStackCommit) << std::endl;
	os << std::format("SizeOfHeapReserve:                0x{:08X}", ioh.SizeOfHeapReserve) << std::endl;
	os << std::format("SizeOfHeapCommit:                 0x{:08X}", ioh.SizeOfHeapCommit) << std::endl;
	os << std::format("LoaderFlags:                      0x{:08X}", ioh.LoaderFlags) << std::endl;
	os << std::format("NumberOfRvaAndSizes:              0x{:08X}", ioh.NumberOfRvaAndSizes) << std::endl;

	size_t i = 0;

	for (const IMAGE_DATA_DIRECTORY& idd : ioh.DataDirectory)
	{
		os << std::format("DataDirectory[{:02}].VirtualAddress: 0x{:08X}", i, idd.VirtualAddress) << std::endl;
		os << std::format("DataDirectory[{:02}].Size:           0x{:08X}", i, idd.Size) << std::endl;
		++i;
	}

	return os;
}

inline std::ostream& operator << (std::ostream& os, const IMAGE_OPTIONAL_HEADER64& ioh)
{
	os << "\nIMAGE_OPTIONAL_HEADER64:" << std::endl;
	os << std::format("Magic:                            0x{:04X}", ioh.Magic) << std::endl;
	os << std::format("MajorLinkerVersion:               0x{:02X}", ioh.MajorLinkerVersion) << std::endl;
	os << std::format("MinorLinkerVersion:               0x{:02X}", ioh.MinorLinkerVersion) << std::endl;
	os << std::format("SizeOfCode:                       0x{:08X}", ioh.SizeOfCode) << std::endl;
	os << std::format("SizeOfInitializedData:            0x{:08X}", ioh.SizeOfInitializedData) << std::endl;
	os << std::format("SizeOfUninitializedData:          0x{:08X}", ioh.SizeOfUninitializedData) << std::endl;
	os << std::format("AddressOfEntryPoint:              0x{:08X}", ioh.AddressOfEntryPoint) << std::endl;
	os << std::format("BaseOfCode:                       0x{:08X}", ioh.BaseOfCode) << std::endl;
	os << std::format("ImageBase:                        0x{:16X}", ioh.ImageBase) << std::endl;
	os << std::format("SectionAlignment:                 0x{:08X}", ioh.SectionAlignment) << std::endl;
	os << std::format("FileAlignment:                    0x{:08X}", ioh.FileAlignment) << std::endl;
	os << std::format("MajorOperatingSystemVersion:      0x{:04X}", ioh.MajorOperatingSystemVersion) << std::endl;
	os << std::format("MinorOperatingSystemVersion:      0x{:04X}", ioh.MinorOperatingSystemVersion) << std::endl;
	os << std::format("MajorImageVersion:                0x{:04X}", ioh.MajorImageVersion) << std::endl;
	os << std::format("MinorImageVersion:                0x{:04X}", ioh.MinorImageVersion) << std::endl;
	os << std::format("MajorSubsystemVersion:            0x{:04X}", ioh.MajorSubsystemVersion) << std::endl;
	os << std::format("MinorSubsystemVersion:            0x{:04X}", ioh.MinorSubsystemVersion) << std::endl;
	os << std::format("Win32VersionValue:                0x{:08X}", ioh.Win32VersionValue) << std::endl;
	os << std::format("SizeOfImage:                      0x{:08X}", ioh.SizeOfImage) << std::endl;
	os << std::format("SizeOfHeaders:                    0x{:08X}", ioh.SizeOfHeaders) << std::endl;
	os << std::format("CheckSum:                         0x{:08X}", ioh.CheckSum) << std::endl;
	os << std::format("Subsystem:                        0x{:04X}", ioh.Subsystem) << std::endl;
	os << std::format("DllCharacteristics:               0x{:04X}", ioh.DllCharacteristics) << std::endl;
	os << std::format("SizeOfStackReserve:               0x{:16X}", ioh.SizeOfStackReserve) << std::endl;
	os << std::format("SizeOfStackCommit:                0x{:16X}", ioh.SizeOfStackCommit) << std::endl;
	os << std::format("SizeOfHeapReserve:                0x{:16X}", ioh.SizeOfHeapReserve) << std::endl;
	os << std::format("SizeOfHeapCommit:                 0x{:16X}", ioh.SizeOfHeapCommit) << std::endl;
	os << std::format("LoaderFlags:                      0x{:08X}", ioh.LoaderFlags) << std::endl;
	os << std::format("NumberOfRvaAndSizes:              0x{:08X}", ioh.NumberOfRvaAndSizes) << std::endl;

	size_t i = 0;

	for (const IMAGE_DATA_DIRECTORY& idd : ioh.DataDirectory)
	{
		os << std::format("DataDirectory[{:02}].VirtualAddress: 0x{:08X}", i, idd.VirtualAddress) << std::endl;
		os << std::format("DataDirectory[{:02}].Size:           0x{:08X}", i, idd.Size) << std::endl;
		++i;
	}

	return os;
}

inline std::ostream& operator << (std::ostream& os, const IMAGE_SECTION_HEADER& iid)
{
	os << "\nIMAGE_SECTION_HEADER:" << std::endl;

	char name[IMAGE_SIZEOF_SHORT_NAME];
	std::memcpy(name, iid.Name, IMAGE_SIZEOF_SHORT_NAME);

	os << std::format("Name:                 {}", name) << std::endl;
	os << std::format("Misc.PhysicalAddress: 0x{:08X}", iid.Misc.PhysicalAddress) << std::endl;
	os << std::format("Misc.VirtualSize:     0x{:08X}", iid.Misc.VirtualSize) << std::endl;
	os << std::format("VirtualAddress:       0x{:08X}", iid.VirtualAddress) << std::endl;
	os << std::format("SizeOfRawData:        0x{:08X}", iid.SizeOfRawData) << std::endl;
	os << std::format("PointerToRawData:     0x{:08X}", iid.PointerToRawData) << std::endl;
	os << std::format("PointerToRelocations: 0x{:08X}", iid.PointerToRelocations) << std::endl;
	os << std::format("PointerToLinenumbers: 0x{:08X}", iid.PointerToLinenumbers) << std::endl;
	os << std::format("NumberOfRelocations:  0x{:04X}", iid.NumberOfRelocations) << std::endl;
	os << std::format("NumberOfLinenumbers:  0x{:04X}", iid.NumberOfLinenumbers) << std::endl;
	os << std::format("Characteristics:      0x{:08X}", iid.Characteristics) << std::endl;

	return os;
}

inline std::ostream& operator << (std::ostream& os, const IMAGE_IMPORT_DESCRIPTOR& iid)
{
	os << "\nIMAGE_IMPORT_DESCRIPTOR:" << std::endl;
	os << std::format("Characteristics:    0x{:08X}", iid.Characteristics) << std::endl;
	os << std::format("OriginalFirstThunk: 0x{:08X}", iid.OriginalFirstThunk) << std::endl;
	os << std::format("TimeDateStamp:      0x{:08X}", iid.TimeDateStamp) << std::endl;
	os << std::format("ForwarderChain:     0x{:08X}", iid.ForwarderChain) << std::endl;
	os << std::format("Name:               0x{:08X}", iid.Name) << std::endl;
	os << std::format("FirstThunk:         0x{:08X}", iid.FirstThunk) << std::endl;

	return os;
}

inline std::ostream& operator << (std::ostream& os, const IMAGE_THUNK_DATA32& itd)
{
	os << "\nIMAGE_THUNK_DATA32:" << std::endl;
	os << std::format("ForwarderString: 0x{:08X}", itd.u1.ForwarderString) << std::endl;
	os << std::format("Function:        0x{:08X}", itd.u1.Function) << std::endl;
	os << std::format("Ordinal:         0x{:08X}", itd.u1.Ordinal) << std::endl;
	os << std::format("AddressOfData:   0x{:08X}", itd.u1.AddressOfData) << std::endl;

	return os;
}

inline std::ostream& operator << (std::ostream& os, const IMAGE_THUNK_DATA64& itd)
{
	os << "\nIMAGE_THUNK_DATA64:" << std::endl;
	os << std::format("ForwarderString: 0x{:16X}", itd.u1.ForwarderString) << std::endl;
	os << std::format("Function:        0x{:16X}", itd.u1.Function) << std::endl;
	os << std::format("Ordinal:         0x{:16X}", itd.u1.Ordinal) << std::endl;
	os << std::format("AddressOfData:   0x{:16X}", itd.u1.AddressOfData) << std::endl;

	return os;
}

inline  std::ostream& operator << (std::ostream& os, const IMAGE_EXPORT_DIRECTORY& ied)
{
	os << "\nIMAGE_EXPORT_DIRECTORY:" << std::endl;
	os << std::format("Characteristics:       0x{:08X}", ied.Characteristics) << std::endl;
	os << std::format("TimeDateStamp:         0x{:08X}", ied.TimeDateStamp) << std::endl;
	os << std::format("MajorVersion:          0x{:04X}", ied.MajorVersion) << std::endl;
	os << std::format("MinorVersion:          0x{:04X}", ied.MinorVersion) << std::endl;
	os << std::format("Name:                  0x{:08X}", ied.Name) << std::endl;
	os << std::format("NumberOfFunctions:     0x{:08X}", ied.NumberOfFunctions) << std::endl;
	os << std::format("NumberOfNames:         0x{:08X}", ied.NumberOfNames) << std::endl;
	os << std::format("AddressOfFunctions:    0x{:08X}", ied.AddressOfFunctions) << std::endl;
	os << std::format("AddressOfNames:        0x{:08X}", ied.AddressOfNames) << std::endl;
	os << std::format("AddressOfNameOrdinals: 0x{:08X}", ied.AddressOfNameOrdinals) << std::endl;

	return os;
}

PEFile::PEFile(const std::filesystem::path& path) :
	Win32File(path),
	_dosHeader(ReadAt<IMAGE_DOS_HEADER>(0))
{
	LogDebug << _dosHeader;

	if (_dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		throw LogicException("Invalid DOS header");
	}

	uint32_t coffOffset = _dosHeader.e_lfanew;

	if (ReadAt<uint32_t>(coffOffset) != 0x00004550)
	{
		throw LogicException("Invalid COFF signature");
	}

	_fileHeader = ReadAt<IMAGE_FILE_HEADER>(coffOffset + sizeof(uint32_t));
	LogDebug << _fileHeader;

	_optionalHeaderOffset = CurrentPosition();
}

IMAGE_OPTIONAL_HEADER32 PEFile::OptionalHeader32() const
{
	auto header = ReadAt<IMAGE_OPTIONAL_HEADER32>(_optionalHeaderOffset);
	LogDebug << header;

	if (header.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
	{
		throw LogicException("Invalid magic");
	}

	return header;
}

IMAGE_OPTIONAL_HEADER64 PEFile::OptionalHeader64() const
{
	auto header = ReadAt<IMAGE_OPTIONAL_HEADER64>(_optionalHeaderOffset);
	LogDebug << header;

	if (header.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		throw LogicException("Invalid magic");
	}

	return header;
}

SHA256 PEFile::Checksum() const
{
	auto self = dynamic_cast<const Win32File*>(this);
	return SHA256(*self);
}

Executable::Executable(const std::filesystem::path& path) :
	PEFile(path)
{
}

// https://learn.microsoft.com/en-us/previous-versions/ms809762(v=msdn.10)?redirectedfrom=MSDN
// TODO: this function is too long to read
std::vector<std::pair<std::string, std::string>> Executable::ImportedFunctions() const
{
	std::vector<std::pair<std::string, std::string>> result;

	IMAGE_FILE_HEADER fh = FileHeader();

	if (!(fh.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE))
	{
		throw LogicException("Not an executable");
	}

	if (fh.Machine == IMAGE_FILE_MACHINE_I386)
	{
		IMAGE_OPTIONAL_HEADER32 ioh = OptionalHeader32();

		size_t imageSectionOffset = OptionalHeaderEnd();

		IMAGE_DATA_DIRECTORY idd = ioh.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

		LogDebug << idd;

		IMAGE_SECTION_HEADER ish;
		IMAGE_SECTION_HEADER importSection = {};

		for (size_t i = 0; i < fh.NumberOfSections; ++i)
		{
			ish = ReadAt<IMAGE_SECTION_HEADER>(imageSectionOffset);

			LogDebug << ish;

			if (idd.VirtualAddress >= ish.VirtualAddress && idd.VirtualAddress < ish.VirtualAddress + ish.Misc.VirtualSize)
			{
				LogDebug << "Found correct ISH";
				importSection = ish;
			}

			imageSectionOffset += sizeof(IMAGE_SECTION_HEADER);
		}

		LogDebug << importSection;

		size_t importOffset = importSection.PointerToRawData + (idd.VirtualAddress - importSection.VirtualAddress);

		IMAGE_IMPORT_DESCRIPTOR iid = {};

		do
		{
			LogDebug << std::format("importOffset: 0x{:08X}", importOffset);
			iid = ReadAt<IMAGE_IMPORT_DESCRIPTOR>(importOffset);

			if (!iid.Name)
			{
				break;
			}

			LogDebug << iid;

			size_t libraryNameOffset = importSection.PointerToRawData + (iid.Name - importSection.VirtualAddress);

			char libraryNameBuffer[MAX_PATH] = {};
			ReadAt(libraryNameBuffer, MAX_PATH, libraryNameOffset);


			size_t thunkOffset = iid.OriginalFirstThunk == 0 ? iid.FirstThunk : iid.OriginalFirstThunk;
			thunkOffset -= importSection.VirtualAddress;
			thunkOffset += importSection.PointerToRawData;

			IMAGE_THUNK_DATA32 itd = { };

			do
			{
				LogDebug << std::format("thunkOffset: 0x{:08X}", thunkOffset);
				itd = ReadAt<IMAGE_THUNK_DATA32>(thunkOffset);

				if (!itd.u1.AddressOfData)
				{
					break;
				}

				char functionNameBuffer[MAX_PATH] = {};

				size_t functionNameOffset = importSection.PointerToRawData + (itd.u1.AddressOfData - importSection.VirtualAddress + 2);

				ReadAt(functionNameBuffer, MAX_PATH, functionNameOffset);

				LogDebug << "Found:" << functionNameBuffer;

				result.emplace_back(libraryNameBuffer, functionNameBuffer);

				thunkOffset += sizeof(IMAGE_THUNK_DATA32);
				
			} while (itd.u1.AddressOfData);

			importOffset += sizeof(IMAGE_IMPORT_DESCRIPTOR);
		} while (iid.Name);

	}

	return result;
}

Library::Library(const std::filesystem::path& path) :
	PEFile(path)
{
}

// TODO: this function is too long to read
std::vector<std::string> Library::ExportedFunctions() const
{
	std::vector<std::string> result;

	IMAGE_FILE_HEADER fh = FileHeader();

	if (!(fh.Characteristics & IMAGE_FILE_DLL))
	{
		throw LogicException("Not a library");
	}

	if (fh.Machine == IMAGE_FILE_MACHINE_I386)
	{
		IMAGE_OPTIONAL_HEADER32 ioh = OptionalHeader32();

		size_t imageSectionOffset = OptionalHeaderEnd();

		IMAGE_DATA_DIRECTORY idd = ioh.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

		LogDebug << idd;

		IMAGE_SECTION_HEADER ish;
		IMAGE_SECTION_HEADER exportSection = {};

		for (size_t i = 0; i < fh.NumberOfSections; ++i)
		{
			ish = ReadAt<IMAGE_SECTION_HEADER>(imageSectionOffset);

			LogDebug << ish;

			if (idd.VirtualAddress >= ish.VirtualAddress && idd.VirtualAddress < ish.VirtualAddress + ish.Misc.VirtualSize)
			{
				LogDebug << "Found correct ISH";
				exportSection = ish;
			}

			imageSectionOffset += sizeof(IMAGE_SECTION_HEADER);
		}

		size_t exportOffset = exportSection.PointerToRawData + (idd.VirtualAddress - exportSection.VirtualAddress);
		LogDebug << std::format("exportOffset: 0x{:08X}", exportOffset);

		auto ied = ReadAt<IMAGE_EXPORT_DIRECTORY>(exportOffset);

		LogDebug << ied;

		size_t functionNamesOffset = exportSection.PointerToRawData + (ied.AddressOfNames - exportSection.VirtualAddress);
		LogDebug << std::format("functionNamesOffset: 0x{:08X}", functionNamesOffset);

		for (size_t i = 0; i < ied.NumberOfNames; ++i)
		{
			size_t functionNameOffset = ReadAt<uint32_t>(functionNamesOffset);
			LogDebug << std::format("functionNameOffset: 0x{:08X}", functionNameOffset);

			functionNameOffset = exportSection.PointerToRawData + (functionNameOffset - exportSection.VirtualAddress);
			LogDebug << std::format("functionNameOffset: 0x{:08X}", functionNameOffset);

			char functionNameBuffer[MAX_PATH] = {};

			ReadAt(functionNameBuffer, MAX_PATH, functionNameOffset);

			LogDebug << "Found:" << functionNameBuffer;

			result.emplace_back(functionNameBuffer);

			functionNamesOffset += sizeof(uint32_t);
		}

	}

	return result;
}