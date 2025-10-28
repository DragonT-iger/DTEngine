#include "pch.h"
#include "JsonIO.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

using nlohmann::json;


JsonWriter::JsonWriter() : m_root(std::make_unique<json>(json::object())), m_stack{ m_root.get() } {}

JsonWriter::~JsonWriter() = default;

void JsonWriter::BeginArray(const char* name) {
    Current()[name] = json::array();
    m_stack.push_back(&Current()[name]); 
}

void JsonWriter::EndArray() {
    if (m_stack.size() > 1) {
        m_stack.pop_back(); 
    }
}

void JsonWriter::Write(const char* name, const std::string& v) { Current()[name] = v; }
void JsonWriter::Write(const char* name, bool v) { Current()[name] = v; }
void JsonWriter::Write(const char* name, float v) { Current()[name] = v; }
void JsonWriter::Write(const char* name, int v) { Current()[name] = v; }
void JsonWriter::Write(const char* name, uint64_t v) { Current()[name] = v; }

void JsonWriter::Write(const char* name, float x, float y, float z) {
    Current()[name] = { x, y, z };
}

void JsonWriter::Write(const char* name, float x, float y, float z, float w) {
    Current()[name] = { x, y, z , w };
}

void JsonWriter::BeginObject(const char* name) {
    Current()[name] = json::object();
    m_stack.push_back(&Current()[name]);
}

void JsonWriter::EndObject() {
    if (m_stack.size() > 1) { 
        m_stack.pop_back();
    }
}

bool JsonWriter::SaveFile(const std::string& fullPath) const
{
    std::ofstream ofs(fullPath, std::ios::binary);
    if (!ofs) return false;

    ofs << this->ToString();
    return true;
}

std::string JsonWriter::ToString() const { return m_root->dump(2); }

json& JsonWriter::Current() { return *m_stack.back(); }

/* ---------------- JsonReader ---------------- */

JsonReader::JsonReader(const char* jsonText) {
    m_root = new json(json::parse(jsonText));
    m_cursor = m_root;
}
JsonReader::JsonReader(const std::string& jsonText) : JsonReader(jsonText.c_str()) {}

JsonReader::~JsonReader() = default;

bool JsonReader::Has(const char* name) const {
    return m_cursor->contains(name);
}

std::string JsonReader::ReadString(const char* name, const std::string& def) const {
    if (!Has(name)) return def;
    const auto& v = (*m_cursor)[name];
    return v.is_string() ? v.get<std::string>() : def;
}
bool JsonReader::ReadBool(const char* name, bool def) const {
    if (!Has(name)) return def;
    const auto& v = (*m_cursor)[name];
    return v.is_boolean() ? v.get<bool>() : def;
}
float JsonReader::ReadFloat(const char* name, float def) const {
    if (!Has(name)) return def;
    const auto& v = (*m_cursor)[name];
    return v.is_number() ? v.get<float>() : def;
}
int JsonReader::ReadInt(const char* name, int def) const {
    if (!Has(name)) return def;
    const auto& v = (*m_cursor)[name];
    return v.is_number_integer() ? v.get<int>() : def;
}

uint64_t JsonReader::ReadUInt64(const char* name, uint64_t def) const {
    if (!Has(name)) return def;
    const auto& v = (*m_cursor)[name];

    if (v.is_number_integer() || v.is_number_unsigned()) {
        return v.get<uint64_t>();
    }
    return def;
}


std::array<float, 3> JsonReader::ReadVec3(const char* name, std::array<float, 3> def) const {
    if (!Has(name)) return def;
    const auto& a = (*m_cursor)[name];
    if (a.is_array() && a.size() == 3 && a[0].is_number()) {
        return { a[0].get<float>(), a[1].get<float>(), a[2].get<float>() };
    }
    return def;
}

std::array<float, 4> JsonReader::ReadVec4(const char* name, std::array<float, 4> def) const {
    if (!Has(name)) return def;
    const auto& a = (*m_cursor)[name];
    if (a.is_array() && a.size() == 4 && a[0].is_number()) {
        return { a[0].get<float>(), a[1].get<float>(), a[2].get<float>() , a[3].get<float>() };
    }
    return def;
}


bool JsonReader::BeginObject(const char* name) {
    if (Has(name)) {
        const auto& obj = (*m_cursor)[name];
        if (obj.is_object()) {
            m_stack.push_back(m_cursor); 
            m_cursor = const_cast<json*>(&obj);
            return true;
        }
    }
    return false;
}

void JsonReader::EndObject() {
    if (!m_stack.empty()) {
        m_cursor = m_stack.back();
        m_stack.pop_back();
    }
}

std::optional<JsonReader> JsonReader::LoadJson(const std::string& fullPath)
{
    std::ifstream ifs(fullPath, std::ios::binary);
    
    std::cout << fullPath << std::endl;

    if (!ifs) {
		std::cout << "Failed to open JSON file: " << fullPath << std::endl;
        return std::nullopt;
    }

    std::ostringstream ss;
    ss << ifs.rdbuf();


    if (ss.str().empty()) {
		std::cout << "JSON file is empty: " << fullPath << std::endl;
		return std::nullopt;
    }

    return JsonReader{ ss.str() };
}


int JsonReader::BeginArray(const char* name) {
    m_array = nullptr; m_index = -1;
    if (Has(name)) {
        const auto& arr = (*m_cursor)[name];
        if (arr.is_array()) {
            m_array = &arr;
            return static_cast<int>(arr.size());
        }
    }
    return 0;
}

void JsonReader::EndArray() {
    if (!m_stack.empty()) {
        m_cursor = m_stack.back();
        m_stack.pop_back();
    }
    m_array = nullptr; m_index = -1;
}

bool JsonReader::NextArrayItem() {
    if (!m_array) return false;

    ++m_index;

    if (m_index >= static_cast<int>(m_array->size())) return false;

    m_stack.push_back(m_cursor);

    m_cursor = const_cast<json*>(&(*m_array)[m_index]);
    return true;
}

void JsonWriter::NextArrayItem() {
    json& curr = Current();
    if (!curr.is_array()) {
        return;
    }
    curr.push_back(json::object());

    m_stack.push_back(&curr.back());
}

void JsonWriter::EndArrayItem() {
    if (m_stack.size() > 1) {
        m_stack.pop_back();
    }
}
