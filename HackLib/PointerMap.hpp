#pragma once

#include "Pointer.hpp"

class PointerMap
{
public:
	inline PointerMap(Pointer region, std::initializer_list<std::string>&& names)
	{
		for (const std::string& name : names)
		{
			_data[name] = region;
			++region;
		}
	}

	inline const Pointer& operator[](const std::string& name) const
	{
		return _data.at(name);
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
	std::map<std::string, Pointer> _data;
};

inline std::ostream& operator << (std::ostream& os, const PointerMap& pm)
{
	const std::string sep1 = ", ";
	std::string sep2;

	for (const auto& [key, value] : pm)
	{
		os << sep2 << std::endl << key << '=' << value;
		sep2 = sep1;
	}

	return os;
}