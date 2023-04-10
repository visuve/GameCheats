#include "CmdArgs.hpp"
#include "Logger.hpp"
#include "FileGenerator.hpp"

void Filter(
	std::vector<std::string>& exports,
	const std::filesystem::path& executablePath,
	std::string_view libraryFileName)
{
	PE::Executable executable(executablePath);

	auto imports = executable.ImportedFunctions(libraryFileName);

	for (auto exportIter = exports.begin(); exportIter != exports.end();)
	{
		auto importIter = std::find(imports.begin(), imports.end(), *exportIter);

		if (importIter != imports.end())
		{
			imports.erase(importIter); // This is now dealth with
			++exportIter;
			continue;
		}

		exportIter = exports.erase(exportIter);
	}

	for (const auto& imported : imports)
	{
		LogWarning << imported << "not found in" << libraryFileName << "for" << executablePath;
	}
}

void GenerateFiles(
	const std::filesystem::path& libraryPath,
	const std::filesystem::path& executablePath,
	const std::filesystem::path& outputDirectory)
{
	const std::filesystem::path libraryFileName = libraryPath.filename();
	std::string libraryName = std::filesystem::path(libraryFileName).replace_extension("").string();

	const std::filesystem::path asmPath = outputDirectory / "jumps.asm";
	const std::filesystem::path cPath = outputDirectory / "proxy.c";
	const std::filesystem::path xmlPath = outputDirectory / (libraryName + ".vcxproj");

	PE::Library library(libraryPath);

	COFF::ArchitectureType architecture = library.Architecture();
	auto exports = library.ExportedFunctions();

	if (!executablePath.empty())
	{
		Filter(exports, executablePath, libraryFileName.string());
	}

	if (exports.empty())
	{
		throw ArgumentException("No exports!");
	}

	std::ofstream asmFile(asmPath);
	FileGenerator::GenerateASM(asmFile, architecture, exports);

	std::ofstream ceeFile(cPath);
	FileGenerator::GenerateC(ceeFile, libraryPath, exports);

	std::ofstream xmlFile(xmlPath);
	FileGenerator::GenerateVisualStudioProject(xmlFile, libraryName, asmPath, cPath);

	Log << asmPath << cPath << '&' << xmlPath << "generated";
}

int main(int argc, char** argv)
{
	int exitCode = 0;

	const CmdArgs args(argc, argv,
	{
		{ "library", typeid(std::filesystem::path), "The DLL to replicate" },
		{ "executable", typeid(std::filesystem::path), "[Optional] The EXE to use as a filter for proxied functions" },
		{ "output", typeid(std::filesystem::path), "The output directory for the generated files" },
	});

	try
	{
		const std::filesystem::path library = args.Value<std::filesystem::path>("library");
		const std::filesystem::path executable = args.Value<std::filesystem::path>("executable", "");
		const std::filesystem::path output = args.Value<std::filesystem::path>("output");

		if (!std::filesystem::exists(output))
		{
			std::filesystem::create_directories(output);
		}
		else if (!std::filesystem::is_directory(output))
		{
			throw ArgumentException("Expected a directory for the output parameter");
		}

		GenerateFiles(library, executable, output);

		Log << "Please compile the project file in" << output;
	}
	catch (const CmdArgs::Exception& e)
	{
		LogError << e.what() << "\n";
		std::cerr << e.Usage();
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::exception& e)
	{
		LogError << e.what();
		return ERROR_PROCESS_ABORTED;
	}

	return exitCode;
}