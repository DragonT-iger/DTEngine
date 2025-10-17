#pragma once
#include <string>
#include <array>
#include <vector>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <optional>

class JsonWriter {
public:
    JsonWriter();

    void BeginObject();
    void EndObject();

    void BeginArray(const char* name);
    void EndArray();

    void Key(const char* /*unused*/);

    void Write(const char* name, const std::string& v);
    void Write(const char* name, bool v);
    void Write(const char* name, float v);
    void Write(const char* name, int v);
    void WriteVec3(const char* name, float x, float y, float z);
    void WriteVec4(const char* name, float x, float y, float z, float w);
    void WritePointer(const char* name, void* ptr);

    // 배열에 객체 하나 푸시/팝 (선택 API)
    void ArrayBeginObject(const char* arrayName);
    void ArrayEndObject();

    std::string ToString() const;

private:
    nlohmann::json& Current();

private:
    nlohmann::json* m_root;
    std::vector<nlohmann::json*> m_stack;
    std::vector<std::string> m_arrayStack;
	std::unordered_map<int , std::string> m_pointerToId;
};

class JsonReader {
public:
    explicit JsonReader(const char* jsonText);
    explicit JsonReader(const std::string& jsonText);

    bool Has(const char* name) const;

    std::string ReadString(const char* name, const std::string& def = {}) const;
    bool        ReadBool(const char* name, bool def = true) const;
    float       ReadFloat(const char* name, float def = 0.f) const;
    int         ReadInt(const char* name, int def = 0) const;
    std::array<float, 3> ReadVec3(const char* name, std::array<float, 3> def = { 0,0,0 }) const;
    std::array<float, 4> ReadVec4(const char* name, std::array<float, 4> def = { 0,0,0 }) const;


    static std::optional<JsonReader> LoadJson(const std::string& fullPath);

    // 배열 순회
    int  BeginArray(const char* name);
    bool NextArrayItem();
    void EndArray();

private:
    nlohmann::json* m_root{};
    nlohmann::json* m_cursor{};
    const nlohmann::json* m_array{ nullptr };
    int m_index{ -1 };
    std::vector<nlohmann::json*> m_stack;
};
