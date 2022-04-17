#pragma once

class SHA256
{
public:
	SHA256(const std::filesystem::path& path);

	~SHA256();

	bool operator == (std::string_view expected);

	std::string Value() const;

private:
	size_t PropertySize(std::wstring_view property);
	void ProcessFile();
	void Update(std::span<uint8_t> data);
	void Finish();

	BCRYPT_ALG_HANDLE _algorithmHandle = nullptr;
	BCRYPT_HASH_HANDLE _hashHandle = nullptr;
	HANDLE _file = nullptr;

	std::vector<uint8_t> _hashObject;
	std::vector<uint8_t> _hashData;
};

std::ostream& operator << (std::ostream& os, const SHA256& sha256);