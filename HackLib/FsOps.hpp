#pragma once

namespace FsOps
{
	std::filesystem::path BackupRename(const std::filesystem::path& path);

	void Stab(const std::filesystem::path& path, std::streampos offset, std::string_view text);
};