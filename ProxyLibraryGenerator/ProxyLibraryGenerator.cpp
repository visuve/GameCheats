#include "CmdArgs.hpp"
#include "Logger.hpp"
#include "FileGenerator.hpp"

void GenerateFiles(const std::filesystem::path& outputDirectory, const std::filesystem::path& libraryPath)
{
	std::string libraryName = libraryPath.filename().replace_extension("").string();

	const std::filesystem::path asmPath = outputDirectory / "jumps.asm";
	const std::filesystem::path cPath = outputDirectory / "proxy.c";
	const std::filesystem::path xmlPath = outputDirectory / (libraryName + ".vcxproj");

	PE::Library library(libraryPath);

	COFF::ArchitectureType architecture = library.Architecture();
	auto functions = library.ExportedFunctions();

	std::ofstream asmFile(asmPath);
	FileGenerator::GenerateASM(asmFile, architecture, functions);

	std::ofstream ceeFile(cPath);
	FileGenerator::GenerateC(ceeFile, libraryPath, functions);

	std::ofstream xmlFile(xmlPath);
	FileGenerator::GenerateVisualStudioProject(xmlFile, libraryName, asmPath, cPath);

	Log << asmPath << cPath << '&' << xmlPath << "generated";
}

int main(int argc, char** argv)
{
	int exitCode = 0;

	const CmdArgs args(argc, argv,
	{
		{ "input", typeid(std::filesystem::path), "The DLL to replicate" },
		{ "output", typeid(std::filesystem::path), "The output directory for the generated files" },
	});

	try
	{
		const std::filesystem::path input = args.Value<std::filesystem::path>("input");
		const std::filesystem::path output = args.Value<std::filesystem::path>("output");

		if (!std::filesystem::exists(output))
		{
			std::filesystem::create_directories(output);
		}
		else if (!std::filesystem::is_directory(output))
		{
			throw ArgumentException("Expected a directory for the output parameter");
		}

		GenerateFiles(output, input);

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