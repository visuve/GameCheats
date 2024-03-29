#include "HackLib.hpp"

struct Resources
{
	uint32_t Wood = 0;
	uint32_t Mercury = 0;
	uint32_t Ore = 0;
	uint32_t Sulfur = 0;
	uint32_t Crystal = 0;
	uint32_t Gems = 0;
	uint32_t Gold = 0;

	Resources& operator += (uint32_t value)
	{
		Wood += value;
		Mercury += value;
		Ore += value;
		Sulfur += value;
		Crystal += value;
		Gems += value;
		Gold += value;

		return *this;
	}
};

std::ostream& operator << (std::ostream& os, const Resources& r)
{
	os << "Wood:    " << r.Wood << std::endl;
	os << "Mercury: " << r.Mercury << std::endl;
	os << "Ore:     " << r.Ore << std::endl;
	os << "Sulfur:  " << r.Sulfur << std::endl;
	os << "Crystal: " << r.Crystal << std::endl;
	os << "Gems:    " << r.Gems << std::endl;
	os << "Gold:    " << r.Gold;
	return os;
}

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "addresources", typeid(std::nullopt), "Adds 48879 to each resource" },
	});

	DWORD pid = System::WaitForExe(L"HOMM3 2.0.exe");

	Process process(pid);

	if (!process.Verify("608da95c6dae5de21b2135701365e18d3de173d3f0fd9753812afe6a5b13fa05"))
	{
		LogError << "Expected Heroes of Might & Magic III - HD Edition (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("addresources"))
	{
		Pointer resourcePointer = process.ResolvePointer(0x00281E78u, 0x94u);

		Resources resources = process.Read<Resources>(resourcePointer);

		LogInfo << "Before:";
		LogInfo << resources;

		resources += 0xBEEF;

		LogInfo << "\nAfter:";
		LogInfo << resources;

		process.Write(resourcePointer, resources);
	}

	return 0;
}
