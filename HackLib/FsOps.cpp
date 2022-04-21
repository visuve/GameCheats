#include "FsOps.hpp"

void FsOps::ProcessDirectory(
	const std::filesystem::path path,
	const std::span<PathFunction>& functions)
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
	std::ifstream file;
	file.exceptions(std::ifstream::badbit);
	file.open(path);

	std::string line;
	uint32_t count = 0;

	for (uint32_t lineNum = 0; std::getline(file, line); ++lineNum)
	{
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
	std::ifstream input;
	input.exceptions(std::ifstream::badbit);
	input.open(from);

	std::ofstream output;
	output.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	output.open(to);

	std::string line;

	for (uint32_t lineNum = 0; std::getline(input, line); ++lineNum)
	{
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
