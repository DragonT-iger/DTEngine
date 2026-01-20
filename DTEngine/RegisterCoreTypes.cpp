#include "pch.h"
#include "RegisterCoreTypes.h"
#include "ReflectionMacros.h" 
#include "ReflectionDatabase.h"
#include "SimpleMathHelper.h"
#include "TilemapGenerator.h"


bool RegisterCoreTypes::Initialize()
{

    REGISTER_STRUCT_BEGIN(Vector2)
        REGISTER_STRUCT_PROPERTY(Vector2, x)
        REGISTER_STRUCT_PROPERTY(Vector2, y)
    REGISTER_STRUCT_END()

    REGISTER_STRUCT_BEGIN(Vector3)
        REGISTER_STRUCT_PROPERTY(Vector3, x)
        REGISTER_STRUCT_PROPERTY(Vector3, y)
        REGISTER_STRUCT_PROPERTY(Vector3, z)
    REGISTER_STRUCT_END()


    REGISTER_STRUCT_BEGIN(Quaternion)
        REGISTER_STRUCT_PROPERTY(Quaternion, x)
        REGISTER_STRUCT_PROPERTY(Quaternion, y)
        REGISTER_STRUCT_PROPERTY(Quaternion, z)
        REGISTER_STRUCT_PROPERTY(Quaternion, w)
    REGISTER_STRUCT_END()

    REGISTER_STRUCT_BEGIN(Vector4)
        REGISTER_STRUCT_PROPERTY(Vector4, x)
        REGISTER_STRUCT_PROPERTY(Vector4, y)
        REGISTER_STRUCT_PROPERTY(Vector4, z)
		REGISTER_STRUCT_PROPERTY(Vector4, w)
    REGISTER_STRUCT_END()



    return true;
}