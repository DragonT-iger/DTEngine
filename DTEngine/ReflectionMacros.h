#pragma once

#include <memory>
#include <typeinfo>

#include "ComponentFactory.h"
#include "ReflectionDatabase.h"

#define DTCLASS(CLASS_NAME) \
public: \
    const char* _GetTypeName() const override { return #CLASS_NAME; } \
    \
private: \
    inline static bool _DTCLASSTrigger = [](){ \
        ComponentFactory::Instance().Register( #CLASS_NAME,[]() -> std::unique_ptr<Component> { return std::make_unique<CLASS_NAME>();} ); \
        ReflectionDatabase::Instance().RegisterDTCLASS(#CLASS_NAME); \
        return true;\
    }();\

// _DTCLASSTrigger 는 그냥 실행하기 위한 발사대임 발싸~


#define DTPROPERTY(CLASS_NAME, NAME) \
private: \
    inline static bool _DTPROPERTYTrigger##NAME = [](){ \
        ReflectionDatabase::Instance().RegisterDTPROPERTY( \
            ThisType::GetStaticTypeName(),  \
            #NAME,                          \
            typeid(CLASS_NAME),             \
            offsetof(ThisType, NAME)        \
        ); \
        return true; \
    }();