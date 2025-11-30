#pragma once
#include "JsonIO.h"
#include "ReflectionDatabase.h"

struct FixupTask
{
	void* targetObject;
	PropertyInfo property;
	uint64_t targetID;
};

void ReadPropertyRecursive(JsonReader& reader, const PropertyInfo& prop, void* base, std::vector<FixupTask>& fixupList);
void DeserializeComponentProperties(JsonReader& reader, Component* comp, std::vector<FixupTask>& fixupList);