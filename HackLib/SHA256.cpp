#include "Exceptions.hpp"
#include "Logger.hpp"
#include "SHA256.hpp"
#include "StrConvert.hpp"

SHA256::SHA256(const std::filesystem::path& path) :
	SHA256()
{
	ProcessFile(path);
}

SHA256::~SHA256()
{
	if (_hashHandle)
	{
		BCryptDestroyHash(_hashHandle);
	}
	
	if (_algorithmHandle)
	{
		BCryptCloseAlgorithmProvider(_algorithmHandle, 0);
	}
}

bool SHA256::operator == (std::string_view expected) const
{
	if (expected.length() != HashCharacters)
	{
		throw LogicException("SHA-256 hashes are 32 bytes, i.e. 64 hex characters");
	}

	std::string actual = Value();

	if (!StrConvert::IEquals(expected, actual))
	{
		LogError << "Expected " << expected << ", got " << actual;
		return false;
	}

	return true;
}

std::string SHA256::Value() const
{
	std::string hash;
	hash.reserve(HashCharacters);

	for (uint8_t x : _hashData)
	{
		std::format_to(std::back_inserter(hash), "{:02X}", x);
	}

	return hash;
}

SHA256::SHA256()
{
	{
		NTSTATUS status = BCryptOpenAlgorithmProvider(
			&_algorithmHandle,
			BCRYPT_SHA256_ALGORITHM,
			MS_PRIMITIVE_PROVIDER,
			0);

		if (status != 0)
		{
			throw Win32Exception("BCryptOpenAlgorithmProvider", status);
		}
	}
	{
		NTSTATUS status = BCryptCreateHash(
			_algorithmHandle,
			&_hashHandle,
			nullptr,
			0,
			nullptr,
			0,
			0);

		if (status != 0 || !_hashHandle)
		{
			throw Win32Exception("BCryptCreateHash", status);
		}
	}
}

void SHA256::ProcessFile(const std::filesystem::path& path)
{
	auto before = std::chrono::high_resolution_clock::now();

	std::vector<uint8_t> buffer(0x100000);

	std::ifstream file;
	file.exceptions(std::ifstream::badbit);
	file.open(path, std::ios::binary);

	uintmax_t fileSize = std::filesystem::file_size(path);
	uintmax_t bytesProcessed = 0;

	while (file)
	{
		file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

		const size_t bytesRead = static_cast<size_t>(file.gcount());

		if (bytesRead > buffer.size())
		{
			throw RuntimeException(
				std::format("Read {0} bytes when the buffer had a size of {1}",
					bytesRead,
					buffer.size()));
		}

		bytesProcessed += bytesRead;

		if (bytesProcessed > fileSize)
		{
			throw RuntimeException(
				std::format("Read {0} bytes when the file size was {1}",
					bytesRead, 
					fileSize));
		}

		if (bytesRead < buffer.size())
		{
			buffer.resize(bytesRead);
		}

		float complete = float(bytesProcessed) / float(fileSize) * 100.f;
		Log << Logger::Color::Yellow << std::format("Verifying {:.2f}%", complete);

		Update(buffer);
	}

	Finish();

	auto after = std::chrono::high_resolution_clock::now();

	Log << "Verifying took" << std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
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