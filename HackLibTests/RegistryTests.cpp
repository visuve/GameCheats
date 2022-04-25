#include "Registry.hpp"

TEST(RegistryTests, Sanity)
{
	// This whole executable will not compile without

	Registry registry(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\X64", 
		KEY_QUERY_VALUE);

	EXPECT_EQ(registry.Read<DWORD>(L"Installed"), 1);
}

TEST(RegistryTests, WrongType)
{
	Registry registry(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\X64",
		KEY_QUERY_VALUE);

	EXPECT_THROW(registry.Read<std::wstring>(L"Installed"), std::system_error);
}