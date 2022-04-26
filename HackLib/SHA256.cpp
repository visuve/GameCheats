#include "Exceptions.hpp"
#include "SHA256.hpp"

SHA256::SHA256(const std::filesystem::path& path, std::ostream* progressOutput) :
	_file(CreateFileW(path.wstring().c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr))
{
	if (!_file)
	{
		throw Win32Exception("CreateFile");
	}

	// Create provider
	{
		NTSTATUS status = BCryptOpenAlgorithmProvider(
			&_algorithmHandle,
			BCRYPT_SHA256_ALGORITHM,
			nullptr,
			0);

		if (status != 0)
		{
			throw Win32Exception("BCryptOpenAlgorithmProvider", status);
		}
	}

	// Create hash object
	{
		_hashObject.resize(PropertySize(BCRYPT_OBJECT_LENGTH));

		NTSTATUS status = BCryptCreateHash(
			_algorithmHandle,
			&_hashHandle,
			_hashObject.data(),
			static_cast<ULONG>(_hashObject.size()),
			nullptr,
			0,
			0);

		if (status != 0 || !_hashHandle)
		{
			throw Win32Exception("BCryptCreateHash", status);
		}
	}

	// Prepare hash data
	_hashData.resize(PropertySize(BCRYPT_HASH_LENGTH));;

	ProcessFile(progressOutput);
}

SHA256::~SHA256()
{
	if (_algorithmHandle)
	{
		BCryptCloseAlgorithmProvider(_algorithmHandle, 0);
	}

	if (_hashHandle)
	{
		BCryptDestroyHash(_hashHandle);
	}
}

size_t SHA256::PropertySize(std::wstring_view property)
{
	DWORD object = 0;
	DWORD bytesWritten = 0;

	NTSTATUS status = BCryptGetProperty(
		_algorithmHandle,
		property.data(),
		reinterpret_cast<PUCHAR>(&object),
		sizeof(DWORD),
		&bytesWritten,
		0);

	if (status != 0 || !_algorithmHandle || !object || bytesWritten != sizeof(DWORD))
	{
		throw Win32Exception("BCryptGetProperty", status);
	}

	return object;
}

bool SHA256::operator == (std::string_view expected) const
{
	if (expected.length() != 64)
	{
		throw LogicException("SHA-256 hashes are 64 bytes!");
	}

	std::string actual = Value();

	if (_strnicmp(actual.c_str(), expected.data(), 64) != 0)
	{
		std::cerr << "Expected " << expected << ", got " << actual << std::endl;
		return false;
	}

	return true;
}

std::string SHA256::Value() const
{
	std::stringstream stream;

	stream.setf(std::ios::hex, std::ios::basefield);
	stream.setf(std::ios::uppercase);
	stream.fill('0');

	for (uint8_t x : _hashData)
	{
		stream << std::setw(2) << +x;
	}

	return stream.str();
}

void SHA256::ProcessFile(std::ostream* progressOutput)
{
	std::vector<uint8_t> buffer(0x4000);

	LARGE_INTEGER fileSize = {};

	if (!GetFileSizeEx(_file.Get(), &fileSize))
	{
		throw Win32Exception("GetFileSizeEx");
	}

	LARGE_INTEGER bytesLeft = fileSize;

	while (bytesLeft.QuadPart > 0)
	{
		DWORD bytesRead = 0;

		if (!ReadFile(_file.Get(), buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead, nullptr))
		{
			throw Win32Exception("ReadFile");
		}

		_ASSERT(bytesRead != 0);
		_ASSERT(bytesRead <= buffer.size());

		bytesLeft.QuadPart -= bytesRead;

		if (bytesRead < buffer.size())
		{
			buffer.resize(bytesRead);
		}

		if (progressOutput)
		{
			float complete = 100.0f - (100.0f / (float(fileSize.QuadPart) / float(bytesLeft.QuadPart)));
			*progressOutput << std::format("Verifying {:.2f}%", complete) << std::endl;
		}

		Update(buffer);
	}

	Finish();
}

void SHA256::Update(std::span<uint8_t> data)
{
	NTSTATUS status = BCryptHashData(
		_hashHandle,
		data.data(),
		static_cast<ULONG>(data.size_bytes()),
		0);

	if (status != 0)
	{
		throw Win32Exception("BCryptHashData", status);
	}
}

void SHA256::Finish()
{
	NTSTATUS status = BCryptFinishHash(
		_hashHandle,
		_hashData.data(),
		static_cast<ULONG>(_hashData.size()),
		0);

	if (status != 0)
	{
		throw Win32Exception("BCryptFinishHash", status);
	}
}

std::ostream& operator << (std::ostream& os, const SHA256& sha256)
{
	return os << sha256.Value();
}