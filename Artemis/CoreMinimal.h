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
#include <thread>
#include <chrono>
#include "SDK/SDK.hpp"
#include "MinHook.h"
#include "Engine/Source/Runtime/Core/Public/Logging/LogMacros.h"
#include "Engine/Source/Runtime/Core/Public/CoreGlobals.h"
#include "Engine/Source/Runtime/Core/Public/Containers/StringConv.h"
#include "Engine/Source/Runtime/CoreUObject/Public/UObject/Stack.h"
#include "Engine/Source/Runtime/CoreUObject/Public/UObject/ScriptMacros.h"
#include "FortniteGame/Source/FortniteGame/Public/FortAssets.h"

enum { INDEX_NONE = -1 };

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

template<class T>
FORCEINLINE T* Cast(const UObject* Src)
{
    if (Src == NULL)
        return NULL;

    if (void* InterfaceAddress = const_cast<UObject*>(Src)->GetInterfaceAddress(T::StaticClass()))
        return (T*)InterfaceAddress;

    if (Src->IsA(T::StaticClass()))
        return (T*)Src;

    return NULL;
}

template<typename T>
FORCEINLINE T* NewObject(UObject* Owner, UClass* Class = T::StaticClass())
{
    return (T*)UGameplayStatics::SpawnObject(Class, Owner);
}

#include "Engine/Source/Runtime/Core/Public/Delegates/Delegate.h"
#include "Engine/Restricted/NotForLicensees/Plugins/Online/OnlineSubsystemMcp/Source/Public/McpQueryResult.h"