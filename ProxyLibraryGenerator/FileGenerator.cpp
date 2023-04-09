#include "FileGenerator.hpp"
#include "System.hpp"

namespace FileGenerator
{
	void GenerateASM(std::ostream& out, COFF::ArchitectureType architecture, std::span<std::string> functions)
	{
		const std::string functionAdressType = architecture == COFF::I386 ? "DWORD" : "QWORD";
		const std::string addressType = architecture == COFF::I386 ? "dword" : "qword";

		out << "; THIS FILE IS GENERATED." << std::endl;
		out << "; See: https://github.com/visuve/GameCheats/ProxyLibraryGenerator" << std::endl << std::endl;

		if (architecture == COFF::I386)
		{
			out << ".MODEL FLAT, C" << std::endl << std::endl;
		}

		out << ".DATA" << std::endl;

		for (std::string_view fn : functions)
		{
			out << std::format("EXTERN Real_{} : {}", fn, functionAdressType) << std::endl;
		}

		out << std::endl << ".CODE" << std::endl;

		for (std::string_view fn : functions)
		{
			out << std::endl << "JumpTo_" << fn << " PROC" << std::endl;
			out << std::format("\tjmp {} ptr [Real_{}]", addressType, fn) << std::endl;
			out << "JumpTo_" << fn << " ENDP" << std::endl;
		}

		out << std::endl << "END" << std::endl;
	}

	void GenerateC(std::ostream& out, const std::filesystem::path& libraryPath, std::span<std::string> functions)
	{
		out << "// THIS FILE IS GENERATED." << std::endl;
		out << "// See: https://github.com/visuve/GameCheats/ProxyLibraryGenerator" << std::endl << std::endl;

		out << "#define WIN32_LEAN_AND_MEAN" << std::endl;
		out << "#include <windows.h>" << std::endl;
		out << std::endl;

		for (std::string_view fn : functions)
		{
			out << "extern void JumpTo_" << fn << "();" << std::endl;
		}

		for (std::string_view fn : functions)
		{
			out << std::endl << std::format("__declspec(dllexport) void {}()", fn) << std::endl;
			out << "{" << std::endl;
			out << std::format("\tJumpTo_{}();", fn) << std::endl;
			out << "}" << std::endl;
		}

		out << std::endl;

		out << "HMODULE Real_Library = 0;" << std::endl;

		for (std::string_view fn : functions)
		{
			out << std::format("FARPROC Real_{} = 0;", fn) << std::endl;
		}

		out << std::endl;

		out << "void LoadFunctions()" << std::endl;
		out << '{' << std::endl;
		for (std::string_view fn : functions)
		{
			out << std::format("\tReal_{0} = GetProcAddress(Real_Library, \"{0}\");", fn) << std::endl;
		}
		out << '}' << std::endl << std::endl;

		out << "BOOL __stdcall ProxyLibraryMain(HINSTANCE instance, DWORD reason, LPVOID reserved)" << std::endl;
		out << '{' << std::endl;

		out << "\tswitch (reason)" << std::endl;
		out << "\t{" << std::endl;
		out << "\t\tcase DLL_PROCESS_ATTACH:" << std::endl;
		out << "\t\t{" << std::endl;
		out << "\t\t\tMessageBoxW(0, L\"Now is a good time to attach a debugger!\", L\"HackLib\", 0); " << std::endl << std::endl;
		out << "\t\t\t// NOTE: THIS IS DANGEROUS:" << std::endl;
		out << "\t\t\t// https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices" << std::endl;
		out << std::format("\t\t\tReal_Library = LoadLibraryW(L\"{}\");", libraryPath.generic_string()) << std::endl << std::endl;
		out << "\t\t\tif (!Real_Library)" << std::endl;
		out << "\t\t\t{" << std::endl;
		out << "\t\t\t\treturn FALSE;" << std::endl;
		out << "\t\t\t}" << std::endl << std::endl;
		out << "\t\t\tLoadFunctions();" << std::endl;
		out << "\t\t\tbreak;" << std::endl;
		out << "\t\t}" << std::endl;
		out << "\t\tcase DLL_PROCESS_DETACH:" << std::endl;
		out << "\t\t{" << std::endl;
		out << "\t\t\tif (reserved)" << std::endl;
		out << "\t\t\t{" << std::endl;
		out << "\t\t\t\tbreak;" << std::endl;
		out << "\t\t\t}" << std::endl << std::endl;
		out << "\t\t\tFreeLibrary(Real_Library);" << std::endl;
		out << "\t\t\tbreak;" << std::endl;
		out << "\t\t}" << std::endl;
		out << "\t}" << std::endl << std::endl;

		out << "\treturn TRUE;" << std::endl;

		out << '}' << std::endl;
	}

	void GenerateVisualStudioProject(
		std::ostream& out,
		std::string_view libraryName,
		const std::filesystem::path& asmFilePath,
		const std::filesystem::path& cFilePath)
	{
		constexpr char VisualStudioVersion[] = "v143";

		out << R"(<?xml version="1.0" encoding="utf-8"?>)" << std::endl;
		out << R"(<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">)" << std::endl;

		out << std::endl;
		out << "  <!-- THIS FILE IS GENERATED. -->" << std::endl;
		out << "  <!-- See: https://github.com/visuve/GameCheats/ProxyLibraryGenerator -->" << std::endl;
		out << std::endl;

		out << R"(  <ItemGroup Label="ProjectConfigurations">)" << std::endl;;
		out << R"(    <ProjectConfiguration Include="Debug|Win32">)" << std::endl;;
		out << R"(      <Configuration>Debug</Configuration>)" << std::endl;;
		out << R"(      <Platform>Win32</Platform>)" << std::endl;;
		out << R"(    </ProjectConfiguration>)" << std::endl;;
		out << R"(    <ProjectConfiguration Include="Release|Win32">)" << std::endl;
		out << R"(      <Configuration>Release</Configuration>)" << std::endl;
		out << R"(      <Platform>Win32</Platform>)" << std::endl;
		out << R"(    </ProjectConfiguration>)" << std::endl;
		out << R"(    <ProjectConfiguration Include="Debug|x64">)" << std::endl;
		out << R"(      <Configuration>Debug</Configuration>)" << std::endl;
		out << R"(      <Platform>x64</Platform>)" << std::endl;
		out << R"(    </ProjectConfiguration>)" << std::endl;
		out << R"(    <ProjectConfiguration Include="Release|x64">)" << std::endl;
		out << R"(      <Configuration>Release</Configuration>)" << std::endl;
		out << R"(      <Platform>x64</Platform>)" << std::endl;
		out << R"(    </ProjectConfiguration>)" << std::endl;
		out << R"(  </ItemGroup>)" << std::endl;

		out << std::endl;

		out << R"(  <PropertyGroup Label="Globals">)" << std::endl;
		out << R"(    <ConfigurationType>DynamicLibrary</ConfigurationType>)" << std::endl;
		out << R"(    <PlatformToolset>)" << VisualStudioVersion << R"(</PlatformToolset>)" << std::endl;
		out << R"(    <ProjectGuid>)" << System::GenerateGuid() << R"(</ProjectGuid>)" << std::endl;
		out << R"(    <ProjectName>)" << libraryName << R"(</ProjectName>)" << std::endl;
		out << R"(  </PropertyGroup>)" << std::endl;

		out << std::endl;

		out << R"(  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />)" << std::endl;
		out << R"(  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />)" << std::endl;
		out << R"(  <Import Project="$(VCTargetsPath)\BuildCustomizations\masm.props" />)" << std::endl;

		out << std::endl;

		out << R"(  <PropertyGroup>)" << std::endl;
		out << R"(    <IntDir>$(SolutionDir)obj</IntDir>)" << std::endl;
		out << R"(    <OutDir>$(SolutionDir)bin</OutDir>)" << std::endl;
		out << R"(  </PropertyGroup>)" << std::endl;

		out << std::endl;

		out << R"(  <ItemDefinitionGroup>)" << std::endl;
		out << R"(    <Link>)" << std::endl;
		out << R"(      <AdditionalDependencies>kernel32.lib;user32.lib;</AdditionalDependencies>)" << std::endl;
		out << R"(      <EntryPointSymbol>ProxyLibraryMain</EntryPointSymbol>)" << std::endl;
		out << R"(      <GenerateDebugInformation>true</GenerateDebugInformation>)" << std::endl;
		out << R"(      <IgnoreAllDefaultLibraries>true</IgnoreAllDefaultLibraries>)" << std::endl;
		out << R"(    </Link>)" << std::endl;
		out << R"(  </ItemDefinitionGroup>)" << std::endl;

		out << std::endl;

		out << R"(  <ItemGroup>)" << std::endl;
		out << R"(    <MASM Include=")" << asmFilePath.string() << R"(" />)" << std::endl;
		out << R"(    <ClCompile Include=")" << cFilePath.string() << R"(" />)" << std::endl;
		out << R"(  </ItemGroup>)" << std::endl;

		out << std::endl;

		out << R"(  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />)" << std::endl;
		out << R"(  <Import Project="$(VCTargetsPath)\BuildCustomizations\masm.targets" />)" << std::endl;

		out << std::endl;

		out << R"(</Project>)" << std::endl;
	}
}