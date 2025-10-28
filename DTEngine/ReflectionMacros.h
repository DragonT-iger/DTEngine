#pragma once

#include <memory>
#include <typeinfo>

#include "ComponentFactory.h"
#include "ReflectionDatabase.h"

#define DTCLASS(CLASS_NAME) \
public: \
    using ThisType = CLASS_NAME;\
    static const char* GetStaticTypeName() { return #CLASS_NAME; } \
    const char* _GetTypeName() const override { return GetStaticTypeName(); } \
    \
private: \
    inline static bool _DTCLASSTrigger = [](){ \
        ComponentFactory::Instance().Register( \
            CLASS_NAME::GetStaticTypeName(), \
            []() -> std::unique_ptr<Component> { \
                return std::make_unique<CLASS_NAME>(); \
            } \
        ); \
        ReflectionDatabase::Instance().RegisterDTCLASS(CLASS_NAME::GetStaticTypeName()); \
        return true;\
    }();\

// _DTCLASSTrigger 는 그냥 람다를 실행하기 위한 트릭임


#define DTPROPERTY(TYPE, NAME) \
private: \
    inline static bool s_propRegistered_##NAME = [](){ \
        ReflectionDatabase::Instance().RegisterDTPROPERTY( \
            ThisType::GetStaticTypeName(),  /* "Transform" */ \
            #NAME,                          /* "m_position" */ \
            typeid(TYPE),                   /* typeid(Vector3) */ \
            offsetof(ThisType, NAME)        /* offsetof(Transform, m_position) */ \
        ); \
        return true; \
    }();