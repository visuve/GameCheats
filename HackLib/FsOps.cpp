#include "FsOps.hpp"
#include "Win32File.hpp"

void FsOps::ProcessDirectory(const std::filesystem::path path, std::span<PathFunction>&& functions)
{
	for (const auto& iter : std::filesystem::recursive_directory_iterator(path))
	{
		for (const PathFunction& function : functions)
		{
			if (!function(iter.path()))
			{
				return;
			}
		}
	}
}

uint32_t FsOps::CountLines(
	const std::filesystem::path& path,
	const LineMatchFunction& predicate)
{
	uint32_t count = 0;
	Win32File file(path);

	for (uint32_t lineNum = 1; file; ++lineNum)
	{
		std::string line = file.ReadUntil('\n');

		if (predicate(lineNum, line))
		{
			++count;
		}
	}

	return count;
}

void FsOps::Replicate(
	const std::filesystem::path& from,
	const LineProcessFunction& fn,
	const std::filesystem::path& to)
{
	Win32File input(from);

	std::ofstream output;
	output.exceptions(std::fstream::failbit | std::fstream::badbit);
	output.open(to);

	for (uint32_t lineNum = 1; input; ++lineNum)
	{
		std::string line = input.ReadUntil('\n');

		fn(lineNum, line, output);
	}
}

std::filesystem::path FsOps::BackupRename(const std::filesystem::path& path)
{
	const auto now = std::chrono::system_clock::now();
	const auto sinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

	std::wstring extension = std::format(L".{}.bak", sinceEpoch.count());
	auto backupPath = std::filesystem::path(path).replace_extension(extension);

	std::filesystem::rename(path, backupPath);

	return backupPath;
}