#pragma once

#include "NonCopyable.hpp"

class SHA256
{
public:
	SHA256(const std::filesystem::path& path);

	~SHA256();

	NonCopyable(SHA256);

	bool operator == (std::string_view expected) const;

	std::string Value() const;

private:
	SHA256();

	size_t PropertySize(std::wstring_view property);
	void ProcessFile(const std::filesystem::path& path);
	void Update(std::span<uint8_t> data);
	void Finish();

	BCRYPT_ALG_HANDLE _algorithmHandle = nullptr;
	BCRYPT_HASH_HANDLE _hashHandle = nullptr;

	constexpr static size_t Bytes = 32;
	constexpr static size_t HashCharacters = Bytes * 2;

	std::vector<uint8_t> _hashObject;
	std::array<uint8_t, Bytes> _hashData;
};

std::ostream& operator << (std::ostream& os, const SHA256& sha256);