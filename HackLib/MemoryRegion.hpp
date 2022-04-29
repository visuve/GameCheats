#pragma once

#include "Pointer.hpp"
#include "TypeHelp.hpp"

class MemoryRegion
{
public:
	struct NamedValue
	{
		NamedValue(Pointer address, std::type_index type);
		Pointer Address;
		std::type_index Type;
	};

	using NameTypePair = std::pair<std::string, std::type_index>;

	MemoryRegion(Pointer region, const std::initializer_list<NameTypePair>& pairs);

	inline const Pointer& operator[](const std::string& name) const
	{
		return _data.at(name).Address;
	}

	inline auto begin() const
	{
		return _data.cbegin();
	}

	inline auto end() const
	{
		return _data.cend();
	}

	std::string DeserializeToCheatEngineXml() const;

private:
	std::map<std::string, NamedValue> _data;
};

std::ostream& operator << (std::ostream& os, const MemoryRegion& pm);