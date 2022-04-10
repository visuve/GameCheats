#pragma once

namespace FsOps
{
	// NOTE: std::regex_search has no yet option for std::string_view :(

	using PathFunction = std::function<bool(const std::filesystem::path&)>;
	using LineMatchFunction = std::function<bool(uint32_t, const std::string&)>;
	using LineProcessFunction = std::function<void(uint32_t, const std::string&, std::ofstream&)>;

	void ProcessDirectory(
		const std::filesystem::path path,
		const std::span<PathFunction>& functions);

	uint32_t CountLines(
		const std::filesystem::path& path,
		const LineMatchFunction& predicate);

	void Replicate(
		const std::filesystem::path& from,
		const LineProcessFunction& fn,
		const std::filesystem::path& to);

	std::filesystem::path BackupRename(const std::filesystem::path& path);
};