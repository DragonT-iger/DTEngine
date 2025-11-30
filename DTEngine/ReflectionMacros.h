#pragma once

#include <memory>
#include <typeinfo>
#include "Component.h"

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
            [](void* base) -> void* { \
                return &(static_cast<CLASS_NAME*>(base)->MEMBER_NAME); \
            },\
            [](void* base, void* value) { \
                static_cast<CLASS_NAME*>(base)->MEMBER_NAME = \
                    *static_cast<decltype(CLASS_NAME::MEMBER_NAME)*>(value); \
            });



// 직접 접근도 가능하지만 당연히 캡슐화가 팁?

#define DTPROPERTY_ACCESSOR(CLASS_NAME, MEMBER_NAME, GETTER_FUNC, SETTER_FUNC) \
    ReflectionDatabase::Instance().RegisterDTPROPERTY(#CLASS_NAME, #MEMBER_NAME, \
        typeid(decltype(CLASS_NAME::MEMBER_NAME)), \
        [](void* base) -> void* { \
            return (void*)&(static_cast<CLASS_NAME*>(base)->GETTER_FUNC()); \
        }, \
        [](void* base, void* value) { \
            static_cast<CLASS_NAME*>(base)->SETTER_FUNC( \
                *static_cast<decltype(CLASS_NAME::MEMBER_NAME)*>(value) \
            ); \
        });

// Getter Setter 함수 호출 Transform을 직접 접근하면 안됨





#define DTPROPERTY_SETTER(CLASS_NAME, MEMBER_NAME, SETTER_FUNC) \
    ReflectionDatabase::Instance().RegisterDTPROPERTY(#CLASS_NAME, #MEMBER_NAME, \
        typeid(decltype(CLASS_NAME::MEMBER_NAME)), \
        [](void* base) -> void* { \
            return &(static_cast<CLASS_NAME*>(base)->MEMBER_NAME); \
        }, \
        [](void* base, void* value) { \
            static_cast<CLASS_NAME*>(base)->SETTER_FUNC( \
                *static_cast<decltype(CLASS_NAME::MEMBER_NAME)*>(value) \
            ); \
        });



#define DTPROPERTY_ENUM(CLASS_NAME, MEMBER_NAME, ...) \
    ReflectionDatabase::Instance().RegisterDTPROPERTY(#CLASS_NAME, #MEMBER_NAME, \
        typeid(decltype(CLASS_NAME::MEMBER_NAME)), \
        [](void* base) -> void* { \
            return &(static_cast<CLASS_NAME*>(base)->MEMBER_NAME); \
        }, \
        [](void* base, void* value) { \
            static_cast<CLASS_NAME*>(base)->MEMBER_NAME = \
                *static_cast<decltype(CLASS_NAME::MEMBER_NAME)*>(value); \
        }, \
        std::vector<std::string>{__VA_ARGS__});



#define ENDPROPERTY() \
    }




#define REGISTER_STRUCT_BEGIN(ClassName) \
    ReflectionDatabase::Instance().RegisterDTGENERATED_BODY(typeid(ClassName).name()); \
    []() { \
        using T = ClassName;

#define REGISTER_STRUCT_PROPERTY(ClassName, MemberName) \
        ReflectionDatabase::Instance().RegisterDTPROPERTY(typeid(ClassName).name(), #MemberName, \
            typeid(decltype(T::MemberName)), \
            [](void* base) -> void* { \
                return &(static_cast<ClassName*>(base)->MemberName); \
            }, \
            [](void* base, void* value) { \
                static_cast<ClassName*>(base)->MemberName = \
                    *static_cast<decltype(T::MemberName)*>(value); \
            });

#define REGISTER_STRUCT_END() \
    }();


