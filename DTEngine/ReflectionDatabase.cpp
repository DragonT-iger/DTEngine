#include "pch.h"
#include "ReflectionDatabase.h"
#include <functional>
#include "Component.h"

//#include <iostream>

void ReflectionDatabase::RegisterDTGENERATED_BODY(const char* className)
{
	m_classes.emplace(className, ClassInfo{ className });

	//std::cout << "Registered class: " << className << std::endl;
}

void ReflectionDatabase::RegisterDTPROPERTY(const char* className, const char* propName,
	const std::type_info& type, std::function<void* (void*)> getter, std::function<void(void*, void*)> setter,
	const std::vector<std::string>& enumNames, std::function<void* (GameObject*)> componentSearcher)
{
	auto it = m_classes.find(className);
	if (it != m_classes.end())
	{
		it->second.m_properties.push_back({ propName, type, std::move(getter) , std::move(setter), std::move(componentSearcher) });
	}
}

const ClassInfo* ReflectionDatabase::GetClassInfomation(const std::string& className) const
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