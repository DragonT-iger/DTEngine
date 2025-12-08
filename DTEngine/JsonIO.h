#pragma once
#include <string>
#include <array>
#include <vector>
#include "../ThirdParty/nlohmann/json_fwd.hpp"
#include <unordered_map>
#include <optional>


struct ArrayState {
    const nlohmann::json* array;
    int index;
};

class JsonWriter {
public:
    JsonWriter();
    ~JsonWriter();

    void BeginArray(const char* name);
    void EndArray();

    void NextArrayItem(); 
    void EndArrayItem();

    void Write(const char* name, const std::string& v);
    void Write(const char* name, const char* v);
    void Write(const char* name, bool v);
    void Write(const char* name, float v);
    void Write(const char* name, int v);
	void Write(const char* name, double v);
    void Write(const char* name, uint64_t v);
    void Write(const char* name, float x, float y);
    void Write(const char* name, float x, float y, float z);
    void Write(const char* name, float x, float y, float z, float w);


    void BeginObject(const char* name);
    void EndObject();

    bool SaveFile(const std::string& fullPath) const;


private:
    std::string ToString() const;

    nlohmann::json& Current();

private:
    std::unique_ptr<nlohmann::json> m_root; 
    std::vector<nlohmann::json*> m_stack;
};

class JsonReader {
public:
    explicit JsonReader(const char* jsonText);
    explicit JsonReader(const std::string& jsonText);
    ~JsonReader();

    bool Has(const char* name) const;

    std::string ReadString(const char* name, const std::string& def = {}) const;
    bool        ReadBool(const char* name, bool def = true) const;
    float       ReadFloat(const char* name, float def = 0.f) const;
    int         ReadInt(const char* name, int def = 0) const;
    uint64_t    ReadUInt64(const char* name, uint64_t def = 0) const;
	std::array<float, 2> ReadVec2(const char* name, std::array<float, 2> def = { 0,0 }) const;
    std::array<float, 3> ReadVec3(const char* name, std::array<float, 3> def = { 0,0,0 }) const;
    std::array<float, 4> ReadVec4(const char* name, std::array<float, 4> def = { 0,0,0,1 }) const;


    bool BeginObject(const char* name);
    void EndObject();

    static std::optional<JsonReader> LoadJson(const std::string& relativePath);

    int  BeginArray(const char* name);
    bool NextArrayItem(); 
    void EndArrayItem();
    void EndArray();


    JsonReader(JsonReader&&) noexcept;
    JsonReader& operator=(JsonReader&&) noexcept;

    JsonReader(const JsonReader&) = delete;
    JsonReader& operator=(const JsonReader&) = delete;

private:
    std::unique_ptr<nlohmann::json> m_root;
    nlohmann::json* m_cursor{};
    std::vector<ArrayState> m_arrayStack;
    std::vector<nlohmann::json*> m_stack;
};
