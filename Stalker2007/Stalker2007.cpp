#include "HackLib.hpp"

constexpr char UnpatchedChecksum[] = "b789f5b63cbf403cd986710e94838e5cb0b631ba31696c382a4575dee242971f";
constexpr char PatchedChecksum[] = "52d325e3fbf0f468062090b9d594457e3fe0eb80827d49a157e745fa7f3da3ea";

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "patch", typeid(std::filesystem::path), "Remove XR_3DA.exe administrator requirement" },
		{ "infammo", typeid(std::nullopt), "Infinite ammunition" },
		{ "infhealth", typeid(std::nullopt), "Infinite health" },
		{ "infstamina", typeid(std::nullopt), "Infinite stamina" },
		{ "infarmor", typeid(std::nullopt), "Armor is never reduced" },
		{ "nowear", typeid(std::nullopt), "Weapon condition is never reduced" },
	});

	if (args.Contains("patch"))
	{
		std::filesystem::path path = args.Value<std::filesystem::path>("patch");

		Win32File file(path, GENERIC_READ | GENERIC_WRITE);

		if (SHA256(file) != UnpatchedChecksum)
		{
			LogError << "Cheksum mismatch. Won't patch, will definetely break!";
			return ERROR_REVISION_MISMATCH;
		}

		const std::string text = "asInvoker\"       ";
		file.WriteAt(size_t(0x17D73Fu), text.data(), text.size());

		if (SHA256(file) != PatchedChecksum)
		{
			LogError << "Cheksum mismatch. Stabbing the .exe failed :-( The game might be broken!";
			return ERROR_REVISION_MISMATCH;
		}

		std::wcout << path << L" stabbed";

		return 0;
	}

	DWORD pid = System::WaitForWindow(L"S.T.A.L.K.E.R.: Shadow Of Chernobyl");

	Process process(pid);

	if (!process.Verify(PatchedChecksum))
	{
		LogError << "Please patch the game before applying these cheats";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("lessweaponweight"))
	{
		// original: movss xmm0,[esi+000000A4] - F3 0F 10 86 A4 00 00 00
		Pointer ptr = process.Address(L"xrGame.dll", 0x21DFBF);

		process.WriteBytes(ptr, ByteStream("F3 0F 59 05 45 23 01 00"));
	}

	MemoryRegion ptrs = process.AllocateRegion(
	{
		{ "player", typeid(Pointer) },
		{ "weapon", typeid(Pointer) },
		{ "condition", typeid(float*) },
		{ "health", typeid(float*) },
		{ "stamina", typeid(float*) },
		{ "armor", typeid(float*) }
	});

	LogInfo << "Created pointers:\n" << Logger::Color::Blue << ptrs.DeserializeToCheatEngineXml();

	// Hmmm IAT @ xrGame.dll+451278

	{
		ByteStream stream;

		stream << "8B 93 38 34 00 00"; // mov edx,[ebx+00003438]
		stream << "89 15" << ptrs["weapon"]; // mov [player], edx

		stream << "83 FA 00"; // cmp edx,00
		stream << "74 22"; // je 22

		stream << "8B C2"; // mov eax, edx
		stream << "05 A4 00 00 00"; // add eax, 0xA4
		stream << "A3" << ptrs["condition"]; // mov [weapon], eax

		// Unroll the fucker
		stream << "8B 82 88 00 00 00"; // mov eax, [edx+88]
		stream << "8B 40 08"; // mov eax, [eax+08]
		stream << "8B 40 08"; // mov eax, [eax+08]
		stream << "05 A4 00 00 00"; // add eax, 0xA4
		stream << "A3" << ptrs["armor"]; // mov [armor], eax

		stream << "8B 93 34 34 00 00"; // mov edx,[ebx+00003434]
		stream << "89 15" << ptrs["player"]; // mov [player], edx

		stream << "8B 82 3C 09 00 00"; // mov eax,[edx+0000093C]
		stream << "A3" << ptrs["health"]; // mov [health], eax

		stream << "83 C0 54"; // add eax, 54
		stream << "A3" << ptrs["stamina"]; // mov [stamina], eax

		process.InjectX86(L"xrGame.dll", 0x3D136C, 1, stream);
	}

	// TODO: this occasionally crashes after a gun jam
	if (args.Contains("infammo"))
	{
		ByteStream stream;

		stream << "39 3D" << ptrs["weapon"]; // cmp dword ptr [weapon],edi
		stream << "74 0E"; // je 10
		stream << "83 87 C0 05 00 00 C8";
		stream << "83 87 7C 05 00 00 FF";
		process.InjectX86(L"xrGame.dll", 0x21F1CF, 9, stream);
	}

	ByteStream stream;

	if (args.Contains("infhealth"))
	{
		stream << "C7 05" << ptrs["health"] << "00 00 80 3F"; // mov [health], (float) 1
	}

	if (args.Contains("infstamina"))
	{
		stream << "C7 05" << ptrs["stamina"] << "00 00 80 3F"; // mov [stamina], (float) 1
	}

	if (args.Contains("infarmor"))
	{
		stream << "C7 05" << ptrs["armor"] << "00 00 80 3F"; // mov [armor], (float) 1
	}

	if (args.Contains("nowear"))
	{
		stream << "C7 05" << ptrs["condition"] << "00 00 80 3F"; // mov [condition], (float) 1
	}

	if (stream.Size() > 0)
	{
		// TODO: infarmor and nowear are now too effectively godmode 
		// because the original call-op here is blocked
		process.InjectX86(L"xrGame.dll", 0x1DD3F8, 0, stream);
	}

	return process.WairForExit();;
}