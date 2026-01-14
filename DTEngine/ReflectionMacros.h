#pragma once

#include <memory>
#include <typeinfo>
#include "Component.h"

#include "ComponentFactory.h"
#include "ReflectionDatabase.h"

template <typename T>
void RegisterDTPropertyHelper(const char* className, const char* propName,
    const std::type_info& typeInfo,
    std::function<void* (void*)> getter,
    std::function<void(void*, void*)> setter,
    const std::vector<std::string>& enumNames = {})
{
    if constexpr (std::is_pointer_v<T>)
    {
        using PointeeType = std::remove_pointer_t<T>;

        if constexpr (std::is_base_of_v<Component, PointeeType>)
        {
            ReflectionDatabase::Instance().RegisterComponentPointerType(typeid(T));
        }
    }

    // 4. 기존 속성 등록 수행
    ReflectionDatabase::Instance().RegisterDTPROPERTY(className, propName, typeInfo, getter, setter, enumNames);
}

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
        RegisterDTPropertyHelper<decltype(CLASS_NAME::MEMBER_NAME)>(#CLASS_NAME, #MEMBER_NAME, \
            typeid(decltype(CLASS_NAME::MEMBER_NAME)), \
            [](void* base) -> void* { \
                return &(static_cast<CLASS_NAME*>(base)->MEMBER_NAME); \
            },\
            [](void* base, void* value) { \
                static_cast<CLASS_NAME*>(base)->MEMBER_NAME = \
                    *static_cast<decltype(CLASS_NAME::MEMBER_NAME)*>(value); \
            });

#define DTPROPERTY_ACCESSOR(CLASS_NAME, MEMBER_NAME, GETTER_FUNC, SETTER_FUNC) \
    RegisterDTPropertyHelper<decltype(CLASS_NAME::MEMBER_NAME)>(#CLASS_NAME, #MEMBER_NAME, \
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
    RegisterDTPropertyHelper<decltype(CLASS_NAME::MEMBER_NAME)>(#CLASS_NAME, #MEMBER_NAME, \
        typeid(decltype(CLASS_NAME::MEMBER_NAME)), \
        [](void* base) -> void* { \
            return &(static_cast<CLASS_NAME*>(base)->MEMBER_NAME); \
        }, \
        [](void* base, void* value) { \
            static_cast<CLASS_NAME*>(base)->SETTER_FUNC( \
                *static_cast<decltype(CLASS_NAME::MEMBER_NAME)*>(value) \
            ); \
        });



//#define DTPROPERTY_ENUM(CLASS_NAME, MEMBER_NAME, ...) \
//    RegisterDTPropertyHelper<decltype(CLASS_NAME::MEMBER_NAME)>(#CLASS_NAME, #MEMBER_NAME, \
//        typeid(decltype(CLASS_NAME::MEMBER_NAME)), \
//        [](void* base) -> void* { \
//            return &(static_cast<CLASS_NAME*>(base)->MEMBER_NAME); \
//        }, \
//        [](void* base, void* value) { \
//            static_cast<CLASS_NAME*>(base)->MEMBER_NAME = \
//                *static_cast<decltype(CLASS_NAME::MEMBER_NAME)*>(value); \
//        }, \
//        std::vector<std::string>{__VA_ARGS__});



#define ENDPROPERTY() \
    }




#define REGISTER_STRUCT_BEGIN(CLASS_NAME) \
    ReflectionDatabase::Instance().RegisterDTGENERATED_BODY(typeid(CLASS_NAME).name()); \
    []() { \
        using T = CLASS_NAME;

#define REGISTER_STRUCT_PROPERTY(CLASS_NAME, MEMBER_NAME) \
        RegisterDTPropertyHelper<decltype(CLASS_NAME::MEMBER_NAME)>(typeid(CLASS_NAME).name(), #MEMBER_NAME, \
            typeid(decltype(T::MEMBER_NAME)), \
            [](void* base) -> void* { \
                return &(static_cast<CLASS_NAME*>(base)->MEMBER_NAME); \
            }, \
            [](void* base, void* value) { \
                static_cast<CLASS_NAME*>(base)->MEMBER_NAME = \
                    *static_cast<decltype(T::MEMBER_NAME)*>(value); \
            });

#define REGISTER_STRUCT_END() \
    }();


