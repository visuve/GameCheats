#include "Variable.hpp"
#include "TypeHelp.hpp"

Variable::Variable(const Variable& other) :
	Offset(other.Offset),
	Name(other.Name),
	_value(other._value)
{
}

Variable::Variable(Variable&& other) :
	Offset(other.Offset),
	Name(other.Name),
	_value(std::move(other._value))
{
}

std::type_index Variable::Type() const
{
	return _value.type();
}

bool Variable::operator == (const Variable& other) const
{
	// The name is irrelevant;
	// I do not want to have multiple Variables in a set with the same offset and type
	return Offset == other.Offset && Type() == other.Type();
}

std::strong_ordering Variable::operator <=> (const Variable& other) const
{
	std::strong_ordering offsetCompare = Offset <=> other.Offset;

	if (offsetCompare != 0)
	{
		return offsetCompare;
	}

	return HacklibOrder(Type()) <=> HacklibOrder(other.Type());
}

void Variable::Read(std::function<void(size_t, void*, size_t)> readFunction)
{
	if (Type() == typeid(uint8_t))
	{
		uint8_t value = 0;
		readFunction(Offset, &value, sizeof(uint8_t));
		_value = value;
	}
	else if (Type() == typeid(uint16_t))
	{
		uint16_t value = 0;
		readFunction(Offset, &value, sizeof(uint16_t));
		_value = value;
	}
	else if (Type() == typeid(uint32_t))
	{
		uint32_t value = 0;
		readFunction(Offset, &value, sizeof(uint32_t));
		_value = value;
	}
	else if (Type() == typeid(uint64_t))
	{
		uint64_t value = 0;
		readFunction(Offset, &value, sizeof(uint64_t));
		_value = value;
	}
	else if (Type() == typeid(int8_t))
	{
		int8_t value = 0;
		readFunction(Offset, &value, sizeof(int8_t));
		_value = value;
	}
	else if (Type() == typeid(int16_t))
	{
		int16_t value = 0;
		readFunction(Offset, &value, sizeof(int16_t));
		_value = value;
	}
	else if (Type() == typeid(int32_t))
	{
		int32_t value = 0;
		readFunction(Offset, &value, sizeof(int32_t));
		_value = value;
	}
	else if (Type() == typeid(int64_t))
	{
		int64_t value = 0;
		readFunction(Offset, &value, sizeof(int64_t));
		_value = value;
	}
	else if (Type() == typeid(float))
	{
		float value = 0;
		readFunction(Offset, &value, sizeof(float));
		_value = value;
	}
	else if (Type() == typeid(double))
	{
		double value = 0;
		readFunction(Offset, &value, sizeof(double));
		_value = value;
	}
	else
	{
		throw ArgumentException("Unsupported type");
	}
}

void Variable::Write(std::function<void(size_t, const void*, size_t)> writeFunction) const
{
	if (Type() == typeid(uint8_t))
	{
		const auto value = std::any_cast<uint8_t>(_value);
		writeFunction(Offset, &value, sizeof(uint8_t));
	}
	else if (Type() == typeid(uint16_t))
	{
		const auto value = std::any_cast<uint16_t>(_value);
		writeFunction(Offset, &value, sizeof(uint16_t));
	}
	else if (Type() == typeid(uint32_t))
	{
		const auto value = std::any_cast<uint32_t>(_value);
		writeFunction(Offset, &value, sizeof(uint32_t));
	}
	else if (Type() == typeid(uint64_t))
	{
		const auto value = std::any_cast<uint64_t>(_value);
		writeFunction(Offset, &value, sizeof(uint64_t));
	}
	else if (Type() == typeid(int8_t))
	{
		const auto value = std::any_cast<int8_t>(_value);
		writeFunction(Offset, &value, sizeof(int8_t));
	}
	else if (Type() == typeid(int16_t))
	{
		const auto value = std::any_cast<int16_t>(_value);
		writeFunction(Offset, &value, sizeof(int16_t));
	}
	else if (Type() == typeid(int32_t))
	{
		const auto value = std::any_cast<int32_t>(_value);
		writeFunction(Offset, &value, sizeof(int32_t));
	}
	else if (Type() == typeid(int64_t))
	{
		const auto value = std::any_cast<int64_t>(_value);
		writeFunction(Offset, &value, sizeof(int64_t));
	}
	else if (Type() == typeid(float))
	{
		const auto value = std::any_cast<float>(_value);
		writeFunction(Offset, &value, sizeof(float));
	}
	else if (Type() == typeid(double))
	{
		const auto value = std::any_cast<double>(_value);
		writeFunction(Offset, &value, sizeof(double));
	}
	else
	{
		throw ArgumentException("Unsupported type");
	}
}

std::ostream& operator << (std::ostream& os, const Variable& variable)
{
	if (variable.Type() == typeid(uint8_t))
	{
		os << variable.Name << '=' << variable.Value<uint8_t>();
	}
	else if (variable.Type() == typeid(uint16_t))
	{
		os << variable.Name << '=' << variable.Value<uint16_t>();
	}
	else if (variable.Type() == typeid(uint32_t))
	{
		os << variable.Name << '=' << variable.Value<uint32_t>();
	}
	else if (variable.Type() == typeid(uint64_t))
	{
		os << variable.Name << '=' << variable.Value<uint64_t>();
	}
	else if (variable.Type() == typeid(int8_t))
	{
		os << variable.Name << '=' << variable.Value<int8_t>();
	}
	else if (variable.Type() == typeid(int16_t))
	{
		os << variable.Name << '=' << variable.Value<int16_t>();
	}
	else if (variable.Type() == typeid(int32_t))
	{
		os << variable.Name << '=' << variable.Value<int32_t>();
	}
	else if (variable.Type() == typeid(int64_t))
	{
		os << variable.Name << '=' << variable.Value<int64_t>();
	}
	else if (variable.Type() == typeid(float))
	{
		os << variable.Name << '=' << variable.Value<float>();
	}
	else if (variable.Type() == typeid(double))
	{
		os << variable.Name << '=' << variable.Value<double>();
	}
	else
	{
		throw ArgumentException("Unsupported type");
	}

	return os;
}