#include "FsOps.hpp"
#include "Exceptions.hpp"

std::filesystem::path FsOps::BackupRename(const std::filesystem::path& path)
{
	const auto now = std::chrono::system_clock::now();
	const auto sinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

	std::wstring extension = std::format(L".{}.bak", sinceEpoch.count());
	auto backupPath = std::filesystem::path(path).replace_extension(extension);

	std::filesystem::rename(path, backupPath);

	return backupPath;
}

void FsOps::Stab(const std::filesystem::path& path, std::streampos offset, std::string_view text)
{
	std::fstream file;
	file.exceptions(std::fstream::failbit | std::fstream::badbit);
	file.open(path, std::ios::in | std::ios::out | std::ios::binary);

	file.seekp(offset);

	file.write(text.data(), text.size());
}
