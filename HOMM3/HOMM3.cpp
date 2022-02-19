#include "HOMM3-PCH.hpp"

/*
	More resources in Heroes of Might & Magic III - HD Edition
	Tested with Steam version SHA-256
	608da95c6dae5de21b2135701 365e18d3de173d3f0fd9753812afe6a5b13fa05
*/

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

int wmain()
{
	try
	{
		Process process(L"HOMM3 2.0.exe");

		uint8_t* resourcePointer = process.ResolvePointer(0x00281E78u, 0x94u);

		Resources resources = process.Read<Resources>(resourcePointer);
		
		std::cout << "Before:" << std::endl;
		std::cout << resources << std::endl;
		
		resources += 0xBEEF;

		std::cout << "\nAfter:" << std::endl;
		std::cout << resources << std::endl;

		process.Write(resourcePointer, resources);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
