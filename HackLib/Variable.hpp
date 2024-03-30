#pragma once

#include "Exceptions.hpp"
#include "NonCopyable.hpp"

class Variable
{
public:
	template <typename T>
	Variable(size_t offset, const T& value, const std::string& name) :
		Offset(offset),
		Name(name),
		_value(value)
	{
	}

	Variable(const Variable&);
	Variable(Variable&&);
	NonAssignable(Variable);

	const size_t Offset;
	const std::string Name;

	std::type_index Type() const;

	template <typename T>
	T Value() const
	{
		if (Type() != typeid(T))
		{
			throw ArgumentException("Type mismatch");
		}

		return std::any_cast<T>(_value);
	}

	bool operator == (const Variable& other) const;
	std::strong_ordering operator <=> (const Variable& other) const;

	void Read(std::function<void(size_t, void*, size_t)> readFunction);
	void Write(std::function<void(size_t, const void*, size_t)> writeFunction) const;

private:
	std::any _value;
};

std::ostream& operator << (std::ostream& os, const Variable& data);