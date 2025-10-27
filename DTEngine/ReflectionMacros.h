#pragma once

#include "ComponentFactory.h"
#include <memory>


#define DTCLASS(CLASS_NAME) \
public: \
    static const char* GetStaticTypeName() { return #CLASS_NAME; } \
    const char* _GetTypeName() const override { return GetStaticTypeName(); } \
    \
private: \
    inline static bool s_isRegistered = [](){ \
        ComponentFactory::Instance().Register( \
            CLASS_NAME::GetStaticTypeName(), \
            []() -> std::unique_ptr<Component> { \
                return std::make_unique<CLASS_NAME>(); \
            } \
        ); \
        return true;\
    }();

// s_isRegistered 는 그냥 람다를 실행하기 위한 트릭임