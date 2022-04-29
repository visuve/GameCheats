#include "MemoryRegion.hpp"

MemoryRegion::NamedValue::NamedValue(Pointer address, std::type_index type) :
	Address(address),
	Type(type)
{
}

MemoryRegion::MemoryRegion(Pointer region, const std::initializer_list<NameTypePair>& pairs)
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

std::string MemoryRegion::DeserializeToCheatEngineXml() const
{
	std::stringstream stream;
	stream << R"(<?xml version="1.0" encoding="utf-8"?>)";
	stream << "\n<CheatTable>";
	stream << "\n  <CheatEntries>";

	for (const auto& [key, value] : _data)
	{
		stream << "\n    <CheatEntry>";
		stream << "\n      <Description>" << key << "</Description>";

		if (value.Type == typeid(Pointer))
		{
			stream << "\n      <VariableType>" << Pointer::Size << " Bytes</VariableType>";
			stream << "\n      <ShowAsHex>1</ShowAsHex>";
		}
		else if (value.Type == typeid(uint8_t) || value.Type == typeid(uint8_t*))
		{
			stream << "\n      <VariableType>Byte</VariableType>";
		}
		else if (value.Type == typeid(uint16_t) || value.Type == typeid(uint16_t*))
		{
			stream << "\n      <VariableType>2 Bytes</VariableType>";
		}
		else if (value.Type == typeid(uint32_t) || value.Type == typeid(uint32_t*))
		{
			stream << "\n      <VariableType>4 Bytes</VariableType>";
		}
		else if (value.Type == typeid(uint64_t) || value.Type == typeid(uint64_t*))
		{
			stream << "\n      <VariableType>8 Bytes</VariableType>";
		}
		else if (value.Type == typeid(int8_t) || value.Type == typeid(int8_t*))
		{
			stream << "\n      <VariableType>Byte</VariableType>";
			stream << "\n      <ShowAsSigned>1</ShowAsSigned>";
		}
		else if (value.Type == typeid(int16_t) || value.Type == typeid(int16_t*))
		{
			stream << "\n      <VariableType>2 Bytes</VariableType>";
			stream << "\n      <ShowAsSigned>1</ShowAsSigned>";
		}
		else if (value.Type == typeid(int32_t) || value.Type == typeid(int32_t*))
		{
			stream << "\n      <VariableType>4 Bytes</VariableType>";
			stream << "\n      <ShowAsSigned>1</ShowAsSigned>";
		}
		else if (value.Type == typeid(int64_t) || value.Type == typeid(int64_t*))
		{
			stream << "\n      <VariableType>8 Bytes</VariableType>";
			stream << "\n      <ShowAsSigned>1</ShowAsSigned>";
		}
		else if (value.Type == typeid(float) || value.Type == typeid(float*))
		{
			stream << "\n      <VariableType>Float</VariableType>";
		}
		else if (value.Type == typeid(double) || value.Type == typeid(double*))
		{
			stream << "\n      <VariableType>Double</VariableType>";
		}
		else
		{
			throw ArgumentException("Unknown type to Cheat Engine");
		}

		if (IsBasicPointer(value.Type))
		{
			stream << "\n      <Offsets>";
			stream << "\n        <Offset>" << Pointer::Size << "</Offset>";
			stream << "\n      </Offsets>";
		}

		stream << "\n      <Address>" << value.Address << "</Address>";
		stream << "\n    </CheatEntry>";
	}


	stream << "\n  </CheatEntries>";
	stream << "\n</CheatTable>";

	return stream.str();
}

std::ostream& operator << (std::ostream& os, const MemoryRegion& pm)
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