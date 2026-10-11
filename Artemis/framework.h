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

template<typename T>
T* Cast(UObject* Object)
{
    if (Object && Object->IsA(T::StaticClass()))
        return static_cast<T*>(Object);
    return nullptr;
}