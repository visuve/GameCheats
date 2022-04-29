#pragma once

#include "Pointer.hpp"

inline size_t SizeOfBasicType(std::type_index type)
{
	if (type == typeid(Pointer)) return Pointer::Size;
	if (type == typeid(uint8_t)) return sizeof(uint8_t);
	if (type == typeid(uint16_t)) return sizeof(uint16_t);
	if (type == typeid(uint32_t)) return sizeof(uint32_t);
	if (type == typeid(uint64_t)) return sizeof(uint64_t);
	if (type == typeid(int8_t)) return sizeof(int8_t);
	if (type == typeid(int16_t)) return sizeof(int16_t);
	if (type == typeid(int32_t)) return sizeof(int32_t);
	if (type == typeid(int64_t)) return sizeof(int64_t);
	if (type == typeid(float)) return sizeof(float);
	if (type == typeid(double)) return sizeof(double);
	if (type == typeid(uint8_t*)) return sizeof(uint8_t*);
	if (type == typeid(uint16_t*)) return sizeof(uint16_t*);
	if (type == typeid(uint32_t*)) return sizeof(uint32_t*);
	if (type == typeid(uint64_t*)) return sizeof(uint64_t*);
	if (type == typeid(int8_t*)) return sizeof(int8_t*);
	if (type == typeid(int16_t*)) return sizeof(int16_t*);
	if (type == typeid(int32_t*)) return sizeof(int32_t*);
	if (type == typeid(int64_t*)) return sizeof(int64_t*);
	if (type == typeid(float*)) return sizeof(float*);
	if (type == typeid(double*)) return sizeof(double*);

	return 0;
}

inline std::string_view BasicTypeToString(std::type_index type)
{
	if (type == typeid(Pointer)) return "\"Pointer\"";
	if (type == typeid(uint8_t)) return "uint8_t";
	if (type == typeid(uint16_t)) return "uint16_t";
	if (type == typeid(uint32_t)) return "uint32_t";
	if (type == typeid(uint64_t)) return "uint64_t";
	if (type == typeid(int8_t)) return "int8_t";
	if (type == typeid(int16_t)) return "int16_t";
	if (type == typeid(int32_t)) return "int32_t";
	if (type == typeid(int64_t)) return "int64_t";
	if (type == typeid(float)) return "float";
	if (type == typeid(double)) return "double";
	if (type == typeid(uint8_t*)) return "uint8_t*";
	if (type == typeid(uint16_t*)) return "uint16_t*";
	if (type == typeid(uint32_t*)) return "uint32_t*";
	if (type == typeid(uint64_t*)) return "uint64_t*";
	if (type == typeid(int8_t*)) return "int8_t*";
	if (type == typeid(int16_t*)) return "int16_t*";
	if (type == typeid(int32_t*)) return "int32_t*";
	if (type == typeid(int64_t*)) return "int64_t*";
	if (type == typeid(float*)) return "float*";
	if (type == typeid(double*)) return "double*";
	
	return "unknown type";
}

inline bool IsBasicType(std::type_index type)
{
	if (type == typeid(uint8_t)) return true;
	if (type == typeid(uint16_t)) return true;
	if (type == typeid(uint32_t)) return true;
	if (type == typeid(uint64_t)) return true;
	if (type == typeid(int8_t)) return true;
	if (type == typeid(int16_t)) return true;
	if (type == typeid(int32_t)) return true;
	if (type == typeid(int64_t)) return true;
	if (type == typeid(float)) return true;
	if (type == typeid(double)) return true;

	return false;
}

inline bool IsBasicPointer(std::type_index type)
{
	if (type == typeid(uint8_t*)) return true;
	if (type == typeid(uint16_t*)) return true;
	if (type == typeid(uint32_t*)) return true;
	if (type == typeid(uint64_t*)) return true;
	if (type == typeid(int8_t*)) return true;
	if (type == typeid(int16_t*)) return true;
	if (type == typeid(int32_t*)) return true;
	if (type == typeid(int64_t*)) return true;
	if (type == typeid(float*)) return true;
	if (type == typeid(double*)) return true;

	return false;
}