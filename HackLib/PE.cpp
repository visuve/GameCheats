#include "PE.hpp"
#include "Exceptions.hpp"
#include "Logger.hpp"
#include "Pointer.hpp"
#include "Win32File.hpp"

inline std::ostream& operator << (std::ostream& os, const MZ::Header& mh)
{
	os << "\n\nMZ header:\n";

	os << std::format("\tSignature:             0x{:04X}\n",mh.Signature);
	os << std::format("\tExtraBytes:            0x{:04X}\n", mh.ExtraBytes);
	os << std::format("\tPages:                 0x{:04X}\n", mh.Pages);
	os << std::format("\tRelocationCount:       0x{:04X}\n", mh.RelocationCount);
	os << std::format("\tParagraphHeaderSize:   0x{:04X}\n", mh.ParagraphHeaderSize);
	os << std::format("\tMinimumParagraphs:     0x{:04X}\n", mh.MinimumParagraphs);
	os << std::format("\tMaximumParagraphs:     0x{:04X}\n", mh.MaximumParagraphs);
	os << std::format("\tInitialSS:             0x{:04X}\n", mh.InitialSS);
	os << std::format("\tInitialSP:             0x{:04X}\n", mh.InitialSP);
	os << std::format("\tChecksum:              0x{:04X}\n", mh.Checksum);
	os << std::format("\tInitialIP:             0x{:04X}\n", mh.InitialIP);
	os << std::format("\tInitialCS:             0x{:04X}\n", mh.InitialCS);
	os << std::format("\tRelocationTableOffset: 0x{:04X}\n", mh.InitialCS);
	os << std::format("\tOverlayNumber:         0x{:04X}", mh.OverlayNumber);

	return os;
}

inline std::ostream& operator << (std::ostream& os, const MZ::HeaderExtension& mhe)
{
	os << "\n\nMZ header extension:\n";

	os << "\tReserved1:      ";

	for (uint8_t x : mhe.Reserved1)
	{
		os << std::format(" 0x{:02X}", x);
	}

	os << std::format("\n\tOEMIdentifier:   0x{:04X}\n", mhe.OEMIdentifier);
	os << std::format("\tOEMInfo:         0x{:04X}\n", mhe.OEMInfo);

	os << "\tReserved2:      ";

	for (uint8_t x : mhe.Reserved2)
	{
		os << std::format(" 0x{:02X}", x);
	}

	os << std::format("\n\tCOFFHeaderStart: 0x{:08X}\n", mhe.COFFHeaderStart);

	return os;
}

inline std::ostream& operator << (std::ostream& os, const COFF::Header& ch)
{
	os << "\n\nCOFF header:\n";

	os << std::format("\tArchitecture:         0x{:04X}\n", static_cast<uint16_t>(ch.Architecture));
	os << std::format("\tNumberOfSections:     0x{:04X}\n", ch.NumberOfSections);
	os << std::format("\tTimeDateStamp:        0x{:08X}\n", ch.TimeDateStamp);
	os << std::format("\tPointerToSymbolTable: 0x{:08X}\n", ch.PointerToSymbolTable);
	os << std::format("\tNumberOfSymbols:      0x{:08X}\n", ch.NumberOfSymbols);
	os << std::format("\tSizeOfOptionalHeader: 0x{:04X}\n", ch.SizeOfOptionalHeader);
	os << std::format("\tFlags:                0x{:04X}", ch.Flags);

	return os;
}

inline std::ostream& operator << (std::ostream& os, const COFF::OptionalHeader& ioh)
{
	os << "\n\nCOFF optional header:\n";

	os << std::format("\tSignature:               0x{:04X}\n", ioh.Signature);
	os << std::format("\tMajorLinkerVersion:      0x{:02X}\n", ioh.MajorLinkerVersion);
	os << std::format("\tMinorLinkerVersion:      0x{:02X}\n", ioh.MinorLinkerVersion);
	os << std::format("\tSizeOfCode:              0x{:08X}\n", ioh.SizeOfCode);
	os << std::format("\tSizeOfInitializedData:   0x{:08X}\n", ioh.SizeOfInitializedData);
	os << std::format("\tSizeOfUninitializedData: 0x{:08X}\n", ioh.SizeOfUninitializedData);
	os << std::format("\tAddressOfEntryPoint:     0x{:08X}\n", ioh.AddressOfEntryPoint);
	os << std::format("\tBaseOfCode:              0x{:08X}\n", ioh.BaseOfCode);
	os << std::format("\tBaseOfData:              0x{:08X}", ioh.BaseOfData);

	return os;
}

inline std::ostream& operator << (std::ostream& os, const COFF::OptionalHeaderExtension<uint32_t>& ioh)
{
	os << "\n\nCOFF optional header extension:\n";

	os << std::format("\tImageBase:                   0x{:08X}\n", ioh.ImageBase);
	os << std::format("\tSectionAlignment:            0x{:08X}\n", ioh.SectionAlignment);
	os << std::format("\tFileAlignment:               0x{:08X}\n", ioh.FileAlignment);
	os << std::format("\tMajorOperatingSystemVersion: 0x{:04X}\n", ioh.MajorOperatingSystemVersion);
	os << std::format("\tMinorOperatingSystemVersion: 0x{:04X}\n", ioh.MinorOperatingSystemVersion);
	os << std::format("\tMajorImageVersion:           0x{:04X}\n", ioh.MajorImageVersion);
	os << std::format("\tMinorImageVersion:           0x{:04X}\n", ioh.MinorImageVersion);
	os << std::format("\tMajorSubsystemVersion:       0x{:04X}\n", ioh.MajorSubsystemVersion);
	os << std::format("\tMinorSubsystemVersion:       0x{:04X}\n", ioh.MinorSubsystemVersion);
	os << std::format("\tWin32VersionValue:           0x{:08X}\n", ioh.Win32VersionValue);
	os << std::format("\tSizeOfImage:                 0x{:08X}\n", ioh.SizeOfImage);
	os << std::format("\tSizeOfHeaders:               0x{:08X}\n", ioh.SizeOfHeaders);
	os << std::format("\tCheckSum:                    0x{:08X}\n", ioh.CheckSum);
	os << std::format("\tSubsystem:                   0x{:04X}\n", ioh.Subsystem);
	os << std::format("\tDllCharacteristics:          0x{:04X}\n", ioh.DllCharacteristics);
	os << std::format("\tSizeOfStackReserve:          0x{:08X}\n", ioh.SizeOfStackReserve);
	os << std::format("\tSizeOfStackCommit:           0x{:08X}\n", ioh.SizeOfStackCommit);
	os << std::format("\tSizeOfHeapReserve:           0x{:08X}\n", ioh.SizeOfHeapReserve);
	os << std::format("\tSizeOfHeapCommit:            0x{:08X}\n", ioh.SizeOfHeapCommit);
	os << std::format("\tLoaderFlags:                 0x{:08X}\n", ioh.LoaderFlags);
	os << std::format("\tNumberOfDataDirectories :    0x{:08X}", ioh.NumberOfDataDirectories);

	return os;
}

inline std::ostream& operator << (std::ostream& os, const COFF::OptionalHeaderExtension<uint64_t>& ioh)
{
	os << "\n\nCOFF optional header extension:\n";

	os << std::format("\tImageBase:                   0x{:16X}\n", ioh.ImageBase);
	os << std::format("\tSectionAlignment:            0x{:08X}\n", ioh.SectionAlignment);
	os << std::format("\tFileAlignment:               0x{:08X}\n", ioh.FileAlignment);
	os << std::format("\tMajorOperatingSystemVersion: 0x{:04X}\n", ioh.MajorOperatingSystemVersion);
	os << std::format("\tMinorOperatingSystemVersion: 0x{:04X}\n", ioh.MinorOperatingSystemVersion);
	os << std::format("\tMajorImageVersion:           0x{:04X}\n", ioh.MajorImageVersion);
	os << std::format("\tMinorImageVersion:           0x{:04X}\n", ioh.MinorImageVersion);
	os << std::format("\tMajorSubsystemVersion:       0x{:04X}\n", ioh.MajorSubsystemVersion);
	os << std::format("\tMinorSubsystemVersion:       0x{:04X}\n", ioh.MinorSubsystemVersion);
	os << std::format("\tWin32VersionValue:           0x{:08X}\n", ioh.Win32VersionValue);
	os << std::format("\tSizeOfImage:                 0x{:08X}\n", ioh.SizeOfImage);
	os << std::format("\tSizeOfHeaders:               0x{:08X}\n", ioh.SizeOfHeaders);
	os << std::format("\tCheckSum:                    0x{:08X}\n", ioh.CheckSum);
	os << std::format("\tSubsystem:                   0x{:04X}\n", ioh.Subsystem);
	os << std::format("\tDllCharacteristics:          0x{:04X}\n", ioh.DllCharacteristics);
	os << std::format("\tSizeOfStackReserve:          0x{:16X}\n", ioh.SizeOfStackReserve);
	os << std::format("\tSizeOfStackCommit:           0x{:16X}\n", ioh.SizeOfStackCommit);
	os << std::format("\tSizeOfHeapReserve:           0x{:16X}\n", ioh.SizeOfHeapReserve);
	os << std::format("\tSizeOfHeapCommit:            0x{:16X}\n", ioh.SizeOfHeapCommit);
	os << std::format("\tLoaderFlags:                 0x{:08X}\n", ioh.LoaderFlags);
	os << std::format("\tNumberOfDataDirectories:     0x{:08X}", ioh.NumberOfDataDirectories);

	return os;
}

inline std::ostream& operator << (std::ostream& os, const COFF::DataDirectory& dd)
{
	os << "\n\nCOFF data directory:\n";

	os << std::format("\tVirtualAddress: 0x{:04X}\n", dd.VirtualAddress);
	os << std::format("\tSize:           0x{:04X}", dd.Size);

	return os;
}

inline std::ostream& operator << (std::ostream& os, const COFF::SectionHeader& sh)
{
	os << "\n\nCOFF section header:\n";

	os << '\t' << sh.Name << '\n';
	os << std::format("\tVirtualSize:          0x{:08X}\n", sh.VirtualSize);
	os << std::format("\tVirtualAddress:       0x{:08X}\n", sh.VirtualAddress);
	os << std::format("\tSizeOfRawData:        0x{:08X}\n", sh.SizeOfRawData);
	os << std::format("\tPointerToRawData:     0x{:08X}\n", sh.PointerToRawData);
	os << std::format("\tPointerToRelocations: 0x{:08X}\n", sh.PointerToRelocations);
	os << std::format("\tPointerToLinenumbers: 0x{:08X}\n", sh.PointerToLinenumbers);
	os << std::format("\tNumberOfRelocations:  0x{:04X}\n", sh.NumberOfRelocations);
	os << std::format("\tNumberOfLinenumbers:  0x{:04X}\n", sh.NumberOfLinenumbers);
	os << std::format("\tCharacteristics:      0x{:08X}", sh.Characteristics);

	return os;
}


inline std::ostream& operator << (std::ostream& os, const PE::ImportDescriptor& id)
{
	os << "\n\nPE import descriptor:\n";

	os << std::format("\tOriginalFirstThunk: 0x{:08X}\n", id.OriginalFirstThunk);
	os << std::format("\tTimeDateStamp:      0x{:08X}\n", id.TimeDateStamp);
	os << std::format("\tForwarderChain:     0x{:08X}\n", id.ForwarderChain);
	os << std::format("\tName:               0x{:08X}\n", id.Name);
	os << std::format("\tFirstThunk:         0x{:08X}", id.FirstThunk);

	return os;
}

inline std::ostream& operator << (std::ostream& os, const PE::ExportDirectory& ed)
{
	os << "\n\nPE export directory:\n";

	os << std::format("\tCharacteristics:       0x{:08X}\n", ed.Characteristics);
	os << std::format("\tTimeDateStamp:         0x{:08X}\n", ed.TimeDateStamp);
	os << std::format("\tMajorVersion:          0x{:04X}\n", ed.MajorVersion);
	os << std::format("\tMinorVersion:          0x{:04X}\n", ed.MinorVersion);
	os << std::format("\tName:                  0x{:08X}\n", ed.Name);
	os << std::format("\tBase:                  0x{:08X}\n", ed.Base);
	os << std::format("\tNumberOfFunctions:     0x{:08X}\n", ed.NumberOfFunctions);
	os << std::format("\tNumberOfNames:         0x{:08X}\n", ed.NumberOfNames);
	os << std::format("\tAddressOfFunctions:    0x{:08X}\n", ed.AddressOfFunctions);
	os << std::format("\tAddressOfNames:        0x{:08X}\n", ed.AddressOfNames);
	os << std::format("\tAddressOfNameOrdinals: 0x{:08X}\n", ed.AddressOfNameOrdinals);

	return os;
}

template <typename T>
inline T VirtualAdressToFileOffset(const COFF::SectionHeader& sh, T virtualAddress)
{
	return sh.PointerToRawData + (virtualAddress - sh.VirtualAddress);
}

PEFile::PEFile(const std::filesystem::path& path) :
	Win32File(path),
	_mzHeader(Read<MZ::Header>())
{
	LogDebug << _mzHeader;

	if (_mzHeader.Signature != MZ::Header::ExpectedSignature)
	{
		throw ArgumentException("Invalid MZ header signature");
	}

	_mzExtension = Read<MZ::HeaderExtension>();
	LogDebug << _mzExtension;

	_coffHeader = ReadAt<COFF::Header>(_mzExtension.COFFHeaderStart);
	LogDebug << _coffHeader;

	if (_coffHeader.Signature != COFF::Header::ExpectedSignature)
	{
		throw ArgumentException("Invalid COFF header signature");
	}

	if (!_coffHeader.SizeOfOptionalHeader)
	{
		throw ArgumentException("No optional header");
	}

	_coffOptionalHeader = Read<COFF::OptionalHeader>();
	LogDebug << _coffOptionalHeader;

	size_t numberOfDataDirectories = 0;

	switch (_coffHeader.Architecture)
	{
		case COFF::ArchitectureType::I386:
			if (_coffOptionalHeader.Signature != COFF::OptionalHeader::ExpectedSignaturePE32)
			{
				throw ArgumentException("Architecture and PE format mismatch!");
			}

			numberOfDataDirectories = ReadCOFFOptionalHeaderExtension<uint32_t>();
			_addressSize = 4;
			break;
		case COFF::ArchitectureType::AMD64:
			if (_coffOptionalHeader.Signature != COFF::OptionalHeader::ExpectedSignaturePE32Plus)
			{
				throw ArgumentException("Architecture and PE format mismatch!");
			}

			numberOfDataDirectories = ReadCOFFOptionalHeaderExtension<uint64_t>();
			_addressSize = 8;
			break;

		default:
			throw ArgumentException("Unsupported architecture");
	}

	if (!numberOfDataDirectories)
	{
		throw ArgumentException("No data directories");
	}

	for (size_t i = 0; i < numberOfDataDirectories; ++i)
	{
		auto dd = Read<COFF::DataDirectory>();

		LogDebug << dd;

		_dataDirectories.emplace_back(dd);
	}

	if (!_coffHeader.NumberOfSections)
	{
		throw ArgumentException("No sections");
	}

	for (size_t i = 0; i < _coffHeader.NumberOfSections; ++i)
	{
		auto sh = Read<COFF::SectionHeader>();

		LogDebug << sh;

		_sectionHeaders.emplace_back(sh);
	}
}

PEFile::~PEFile()
{
	if (_optionalHeaderExtension)
	{
		delete _optionalHeaderExtension;
	}
}

SHA256 PEFile::Checksum() const
{
	auto self = dynamic_cast<const Win32File*>(this);
	return SHA256(*self);
}

COFF::SectionHeader PEFile::FindSectionHeader(const COFF::DataDirectory& dd) const
{
	for (const COFF::SectionHeader& sh : _sectionHeaders)
	{
		if (dd.VirtualAddress >= sh.VirtualAddress && dd.VirtualAddress < sh.VirtualAddress + sh.VirtualSize)
		{
			return sh;
		}
	}

	throw RangeException("Not found");
}

Executable::Executable(const std::filesystem::path& path) :
	PEFile(path)
{
	if (!(_coffHeader.Flags & COFF::Flag::Executable))
	{
		throw ArgumentException("Not an executable");
	}
}

std::vector<std::pair<std::string, std::string>> Executable::ImportedFunctions() const
{
	std::vector<std::pair<std::string, std::string>> result;
	size_t originalPosition = CurrentPosition();

	auto dd = _dataDirectories[COFF::DataDirectoryType::ImportTable];
	COFF::SectionHeader importSection = FindSectionHeader(dd);

	LogDebug << importSection;

	PE::ImportDescriptor iid;

	for (size_t importOffset = VirtualAdressToFileOffset(importSection, dd.VirtualAddress); 
		true; // 4ever loop
		importOffset += sizeof(PE::ImportDescriptor))
	{
		LogDebug << std::format("importOffset: 0x{:08X}", importOffset);
		iid = ReadAt<PE::ImportDescriptor>(importOffset);

		if (!iid.Name)
		{
			break;
		}

		LogDebug << iid;

		size_t libraryNameOffset = VirtualAdressToFileOffset(importSection, iid.Name);

		std::string libraryName = ReadAtUntil(libraryNameOffset, '\0');

		LogDebug << "Found:" << libraryName;

		for (size_t thunkOffset = VirtualAdressToFileOffset(importSection,
			iid.OriginalFirstThunk == 0 ? iid.FirstThunk : iid.OriginalFirstThunk);
			true; // 4ever loop
			thunkOffset += _addressSize)
		{
			LogDebug << std::format("thunkOffset: 0x{:08X}", thunkOffset);

			uint64_t thunk = 0; // large enough to hold 32 or 64 bit

			if (ReadAt(&thunk, _addressSize, thunkOffset) != _addressSize)
			{
				throw ArgumentException("Failed to read thunk");
			}

			if (!thunk)
			{
				break;
			}

			uint64_t functionNameOffset = VirtualAdressToFileOffset(importSection, thunk) + 2;

			std::string functionName = ReadAtUntil(static_cast<size_t>(functionNameOffset), '\0');

			LogDebug << "Found:" << functionName;

			result.emplace_back(libraryName, functionName);
		}
	}

	SetPosition(originalPosition);
	return result;
}

Library::Library(const std::filesystem::path& path) :
	PEFile(path)
{
	if (!(_coffHeader.Flags & COFF::Flag::DynamicLinkLibrary))
	{
		throw ArgumentException("Not a library");
	}
}

std::vector<std::string> Library::ExportedFunctions() const
{
	std::vector<std::string> result;
	size_t originalPosition = CurrentPosition();

	auto dd = _dataDirectories[COFF::DataDirectoryType::ExportTable];
	COFF::SectionHeader exportSection = FindSectionHeader(dd);

	size_t exportOffset = VirtualAdressToFileOffset(exportSection, dd.VirtualAddress);
	LogDebug << std::format("exportOffset: 0x{:08X}", exportOffset);

	auto ed = ReadAt<PE::ExportDirectory>(exportOffset);

	LogDebug << ed;

	size_t functionNamesOffset = VirtualAdressToFileOffset(exportSection, ed.AddressOfNames);
	LogDebug << std::format("functionNamesOffset: 0x{:08X}", functionNamesOffset);

	for (size_t i = 0; i < ed.NumberOfNames; ++i)
	{
		uint64_t functionNameOffset = 0;  // large enough to hold 32 or 64 bit
		
		if (ReadAt(&functionNameOffset, _addressSize, static_cast<size_t>(functionNamesOffset)) != _addressSize)
		{
			throw ArgumentException("Failed to read function name offset");
		}

		LogDebug << std::format("functionNameOffset: 0x{:016X}", functionNameOffset);

		functionNameOffset = VirtualAdressToFileOffset(exportSection, functionNameOffset);
		LogDebug << std::format("functionNameOffset: 0x{:016X}", functionNameOffset);

		std::string functionName = ReadAtUntil(static_cast<size_t>(functionNameOffset), '\0');

		LogDebug << "Found:" << functionName;

		result.emplace_back(functionName);

		functionNamesOffset += _addressSize;
	}

	SetPosition(originalPosition);
	return result;
}