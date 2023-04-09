#pragma once

#include "PE.hpp"

namespace FileGenerator
{
	void GenerateASM(
		std::ostream& out, 
		COFF::ArchitectureType architecture,
		std::span<std::string> functions);

	void GenerateC(std::ostream& out,
		const std::filesystem::path& libraryPath,
		std::span<std::string> functions);

	void GenerateVisualStudioProject(
		std::ostream& out,
		std::string_view libraryName,
		const std::filesystem::path& azm,
		const std::filesystem::path& cee);
}