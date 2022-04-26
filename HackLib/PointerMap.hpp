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

	friend std::ostream& operator << (std::ostream&, const PointerMap&);

private:
	std::map<std::string, Pointer> _data;
};

inline std::ostream& operator << (std::ostream& os, const PointerMap& pm)
{
	for (const auto& [key, value] : pm._data)
	{
		os << key << '=' << value << std::endl;
	}

	return os;
}