#pragma once

#include "NonCopyable.hpp"
#include "Win32File.hpp"

class SHA256
{
public:
	SHA256(const std::filesystem::path& path);
	SHA256(const Win32File& file);

	~SHA256();

	NonCopyable(SHA256);

	bool operator == (std::string_view expected) const;

	std::string Value() const;

	constexpr static size_t Bytes = 32;
	constexpr static size_t HashCharacters = Bytes * 2;

private:
	SHA256();
	void ProcessFile(const Win32File& file);
	void Update(std::span<uint8_t> data);
	void Finish();

	BCRYPT_ALG_HANDLE _algorithmHandle = nullptr;
	BCRYPT_HASH_HANDLE _hashHandle = nullptr;
	std::array<uint8_t, Bytes> _hashData;
};

std::ostream& operator << (std::ostream& os, const SHA256& sha256);