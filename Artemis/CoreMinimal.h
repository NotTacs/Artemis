#pragma once

#define WIN32_LEAN_AND_MEAN 

#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <functional>
#include <cwchar>
#include <cstdint>
#include <intrin.h>
#include <type_traits>
#include "SDK/SDK.hpp"
#include "MinHook.h"
#include "Engine/Source/Runtime/Core/Public/Logging/LogMacros.h"
#include "Engine/Source/Runtime/Core/Public/CoreGlobals.h"

template<class T>
FORCEINLINE T* Cast(UObject* Src)
{
    if (Src == NULL)
        return NULL;

    if (void* InterfaceAddress = Src->GetInterfaceAddress(T::StaticClass()))
        return (T*)InterfaceAddress;

    if (Src->IsA(T::StaticClass()))
        return (T*)Src;

    return NULL;
}
