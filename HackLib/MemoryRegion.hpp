#pragma once

#include "Pointer.hpp"
#include "TypeHelp.hpp"

class MemoryRegion
{
public:
	struct NamedValue
	{
		NamedValue(Pointer address, std::type_index type) :
			Address(address),
			Type(type)
		{
		}

		Pointer Address;
		std::type_index Type;
	};

	using NameTypePair = std::pair<std::string, std::type_index>;

	inline MemoryRegion(Pointer region, const std::initializer_list<NameTypePair>& pairs)
	{
		for (const auto& [name, type] : pairs)
		{
			size_t typeSize = SizeOfBasicType(type);

			if (typeSize == 0)
			{
				throw ArgumentException(name + " has unknown type");
			}

			_data.emplace(name, NamedValue(region, type));

			region += typeSize;
		}
	}

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

private:
	std::map<std::string, NamedValue> _data;
};

inline std::ostream& operator << (std::ostream& os, const MemoryRegion& pm)
{
	const std::string sep1 = ", ";
	std::string sep2;

	for (const auto& [key, value] : pm)
	{
		os << sep2 << key << '=' << value.Address << " (" << BasicTypeToString(value.Type) << ')';
		sep2 = sep1;
	}

	return os;
}