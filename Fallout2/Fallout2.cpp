#include "HackLib.hpp"

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "charpoints", typeid(int), "Set character points" },
		{ "actionpoints", typeid(int), "Set action points" },
		{ "skillpoints", typeid(int), "Set skill points" },
		{ "money", typeid(int), "Set money" },
	});

	DWORD pid = System::WaitForExe(L"fallout2HR.exe");

	Process process(pid);

	if (!process.Verify("048684f802b05859e2b160fa358be1b99075bf31b49a312a536645581aa16b5f"))
	{
		LogError << "Expected Fallout 2 \"HR\" (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("charpoints"))
	{
		int charpoints = args.Value<int>("charpoints");
		process.Write<int32_t>(0x118538, charpoints);
	}

	if (args.Contains("actionpoints"))
	{
		int actionpoints = args.Value<int>("actionpoints");
		process.Write<int32_t>(0x11C3B4, actionpoints);
	}

	if (args.Contains("skillpoints"))
	{
		// The individual skills are at 0x11C4AC-0x11C4F0
		int skillpoints = args.Value<int>("skillpoints");
		process.Write<int32_t>(0x2681AC, skillpoints);
	}

	if (args.Contains("money"))
	{
		Pointer moneyPtr = process.ResolvePointer(0x119058, 0x34, 0x1C);

		int before = process.Read<int32_t>(moneyPtr);
		int after = args.Value<int>("money");
		process.Write(moneyPtr, after);

		LogInfo << "Money:" << before << "->" << after;
	}

	return 0;
}
