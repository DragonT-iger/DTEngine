#include "pch.h"
#include "ReflectionDatabase.h"
//#include <iostream>

void ReflectionDatabase::RegisterDTCLASS(const char* className)
{
	m_classes.emplace(className, ClassInfo{ className });

	//std::cout << "Registered class: " << className << std::endl;
}

void ReflectionDatabase::RegisterDTPROPERTY(const char* className, const char* propName,
	const std::type_info& type, size_t offset)
{
	auto it = m_classes.find(className);
	if (it != m_classes.end())
	{
		it->second.m_properties.push_back({ propName, type, offset });
	}
}

const ClassInfo* ReflectionDatabase::GetClassInfo(const std::string& className) const
{
	auto it = m_classes.find(className);
	if (it != m_classes.end())
	{
		return &it->second;
	}
	return nullptr;
}


ReflectionDatabase::ReflectionDatabase() = default;
ReflectionDatabase::~ReflectionDatabase() = default;