#pragma once

#include <memory>
#include <typeinfo>

#include "ComponentFactory.h"
#include "ReflectionDatabase.h"

#define DTGENERATED_BODY(CLASS_NAME) \
public: \
    const char* _GetTypeName() const override { return #CLASS_NAME; } \
    static void _ReflectProperties();\
    \
private: \
    inline static bool _DTGENERATED_BODYTrigger = [](){ \
        ComponentFactory::Instance().Register( #CLASS_NAME,[]() -> std::unique_ptr<Component> { return std::make_unique<CLASS_NAME>();} ); \
        ReflectionDatabase::Instance().RegisterDTGENERATED_BODY(#CLASS_NAME); \
        CLASS_NAME::_ReflectProperties(); \
        return true;\
    }();\


//#define BEGINPROPERTY(CLASS_NAME) \
//    void CLASS_NAME::_ReflectProperties() { \
//
//
//#define DTPROPERTY(CLASS_NAME, MEMBER_NAME) \
//        ReflectionDatabase::Instance().RegisterDTPROPERTY(#CLASS_NAME, #MEMBER_NAME, \
//                            typeid(decltype(CLASS_NAME::MEMBER_NAME)), offsetof(CLASS_NAME, MEMBER_NAME));
//
//#define ENDPROPERTY() \
//    }

#define BEGINPROPERTY(CLASS_NAME) \
    void CLASS_NAME::_ReflectProperties() { \

#define DTPROPERTY(CLASS_NAME, MEMBER_NAME) \
        ReflectionDatabase::Instance().RegisterDTPROPERTY(#CLASS_NAME, #MEMBER_NAME, \
            typeid(decltype(CLASS_NAME::MEMBER_NAME)), \
            [](Component* base) -> void* { \
                return &(static_cast<CLASS_NAME*>(base)->MEMBER_NAME); \
            });

// ENDPROPERTY (변경 없음)
#define ENDPROPERTY() \
    }