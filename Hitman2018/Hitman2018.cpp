#include "HackLib.hpp"

extern "C" void InfAmmo();

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CmdArgs args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Increasing ammunition" },
		{ "crosshair", typeid(std::nullopt), "The crosshair does not spread when shooting" }
	});

	DWORD pid = System::WaitForWindow(L"HITMAN 2");

	Process process(pid);

	if (!process.Verify("578fef87dc2e3f6f47727f5a99fabf10a31d0059d9c397e858e2a344b960766b"))
	{
		LogError << "Expected Hitman 2 with DX 12 (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		auto bytes = Process::ReadFunction(InfAmmo);

		process.InjectX64(0x1531F1, 4, bytes);
	}

	if (args.Contains("crosshair"))
	{
		process.ChangeBytes(0x125F82,
			ByteStream("F3 44 0F 11 83 0C 08 00 00"), // movss [rbx+0000080C],xmm8
			ByteStream("F3 44 0F 11 8B 0C 08 00 00"));  // movss [rbx+0000080C],xmm9

	}

	process.WairForExit();

	return 0;
}
