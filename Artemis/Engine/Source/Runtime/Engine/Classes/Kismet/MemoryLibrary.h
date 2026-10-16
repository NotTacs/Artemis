#pragma once
#include "CoreMinimal.h"

class FMemoryLibrary
{
private:
    static inline void* nullptrForHook = nullptr;
    static uint8_t* AllocateNearbyPage(void* TargetAddr);
    static bool ReturnTrue()
    {
        return true;
    }
    static void Return()
    {
        return;
    }
public:
    template<typename T = void*>
    static void VFTHook(void** VTable, int Index, void* Detour, T& og = nullptrForHook)
    {
        if (!std::is_same_v<T, void*>)
            og = (T)VTable[Index];
        DWORD temp;
        VirtualProtect(VTable + Index, 8, PAGE_EXECUTE_READWRITE, &temp);
        VTable[Index] = Detour;
        VirtualProtect(VTable + Index, 8, temp, &temp);
    }
    template <typename T = void*>
    static void ExecHook(UFunction* Func, void* Detour, T& og = nullptrForHook)
    {
        if (!Func)
            return;
        if (!std::is_same_v<T, void*>)
            og = (T)Func->ExecFunction;

        Func->ExecFunction = reinterpret_cast<UFunction::FNativeFuncPtr>(Detour);
    }
    template<typename T = void*>
    static void DetourHook(uintptr_t Address, void* Detour, T& OriginalPtr = nullptrForHook)
    {
        Address += InSDKUtils::GetImageBase();
        MH_CreateHook(LPVOID(Address), Detour, std::is_same_v<T, void*> ? nullptr : (void**)&OriginalPtr);
        MH_EnableHook(LPVOID(Address));
    }

    static void Rel32Swap(uintptr_t Target, void* Detour);
    static void Rel32Swap_Jnz(uintptr_t Target, void* Detour);
    static void Rel32Swap_Jmp(uintptr_t Target, void* Detour);
    static void Rel32Swap_Lea(uintptr_t Target, void* Detour);

    template<typename T>
    static void Set(uintptr_t Address, T Value)
    {
        Address += InSDKUtils::GetImageBase();
        *reinterpret_cast<T*>(Address) = Value;
    }
    template<typename T>
    static void Set(void* Ptr, int Offset, T Value)
    {
        *reinterpret_cast<T*>(__int64(Ptr) + Offset) = Value;
    }

    static void SetTrue(uintptr_t Address)
    {
        DetourHook(Address, ReturnTrue);
    }

    static void SetNull(uintptr_t Address)
    {
        DetourHook(Address, Return);
    }

    template<typename T = uint8>
    static void SetByte(uintptr_t Ptr, T Value)
    {
        Ptr += InSDKUtils::GetImageBase();
        DWORD oldProtect;
        VirtualProtect(LPVOID(Ptr), sizeof(Value), PAGE_EXECUTE_READWRITE, &oldProtect);
        *reinterpret_cast<T*>(Ptr) = Value;
        VirtualProtect(LPVOID(Ptr), sizeof(Value), oldProtect, &oldProtect);
    }
};

inline uint8_t* FMemoryLibrary::AllocateNearbyPage(void* TargetAddr)
{
    SYSTEM_INFO SysInfo;
    GetSystemInfo(&SysInfo);

    const uint64_t PageSize = SysInfo.dwPageSize;
    const uint64_t StartAddr = (uint64_t(TargetAddr) & ~(PageSize - 1));
    const uint64_t MinAddr = min(StartAddr - 0x7FFFFF00, (uint64_t)SysInfo.lpMinimumApplicationAddress);
    const uint64_t MaxAddr = max(StartAddr + 0x7FFFFF00, (uint64_t)SysInfo.lpMaximumApplicationAddress);
    const uint64_t StartPage = (StartAddr - (StartAddr % PageSize));

    for (uint64_t PageOffset = 1; PageOffset; PageOffset++)
    {
        uint64_t ByteOffset = PageOffset * PageSize;
        uint64_t HighAddr = StartPage + ByteOffset;
        uint64_t LowAddr = (StartPage > ByteOffset) ? StartPage - ByteOffset : 0;

        bool NeedsExit = HighAddr > MaxAddr && LowAddr < MinAddr;

        if (HighAddr < MaxAddr)
        {
            if (void* OutAddr = VirtualAlloc((void*)HighAddr, PageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
            {
                return (uint8_t*)OutAddr;
            }
        }

        if (LowAddr > MinAddr)
        {
            if (void* OutAddr = VirtualAlloc((void*)LowAddr, PageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
            {
                return (uint8_t*)OutAddr;
            }
        }

        if (NeedsExit)
        {
            break;
        }
    }

    return nullptr;
}

inline void FMemoryLibrary::Rel32Swap(uintptr_t Target, void* Detour)
{
    Target = Target + InSDKUtils::GetImageBase();
    void** T = reinterpret_cast<void**>(&Target);
    auto Impl = (uint8*)(*T);
    auto NearPage = AllocateNearbyPage(Impl);

    if (!NearPage)
    {
        return;
    }

    uint8_t Shellcode[] =
    {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    memcpy(Shellcode + 6, &Detour, 8);
    memcpy(NearPage, Shellcode, sizeof(Shellcode));

    auto Offset = static_cast<int>(NearPage - (Impl + 5));

    memcpy(Impl + 1, &Offset, sizeof(int));
}

inline void FMemoryLibrary::Rel32Swap_Jnz(uintptr_t Target, void* Detour)
{
    Target = Target + InSDKUtils::GetImageBase();
    void** T = reinterpret_cast<void**>(&Target);
    auto Impl = (uint8_t*)(*T);
    auto NearPage = AllocateNearbyPage(Impl);

    if (!NearPage)
    {
        return;
    }

    uint8_t Shellcode[] =
    {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    memcpy(Shellcode + 6, &Detour, 8);
    memcpy(NearPage, Shellcode, sizeof(Shellcode));
    auto Offset = static_cast<int>(NearPage - (Impl + 6));
    DWORD OldProtect;
    VirtualProtect(Impl + 2, sizeof(int), PAGE_EXECUTE_READWRITE, &OldProtect);
    memcpy(Impl + 2, &Offset, sizeof(int));
    VirtualProtect(Impl + 2, sizeof(int), OldProtect, &OldProtect);
}

inline void FMemoryLibrary::Rel32Swap_Jmp(uintptr_t Target, void* Detour)
{
    Target = Target + InSDKUtils::GetImageBase();
    void** T = reinterpret_cast<void**>(&Target);
    auto Impl = (uint8_t*)(*T);
    auto NearPage = AllocateNearbyPage(Impl);

    if (!NearPage)
    {
        return;
    }

    uint8_t Shellcode[] =
    {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    memcpy(Shellcode + 6, &Detour, 8);
    memcpy(NearPage, Shellcode, sizeof(Shellcode));

    auto Offset = static_cast<int>(NearPage - (Impl + 5));

    DWORD OldProtect;
    VirtualProtect(Impl + 1, sizeof(int), PAGE_EXECUTE_READWRITE, &OldProtect);
    memcpy(Impl + 1, &Offset, sizeof(int));
    VirtualProtect(Impl + 1, sizeof(int), OldProtect, &OldProtect);
}

inline void FMemoryLibrary::Rel32Swap_Lea(uintptr_t Target, void* Detour)
{
    Target = Target + InSDKUtils::GetImageBase();
    void** T = reinterpret_cast<void**>(&Target);
    auto Impl = (uint8_t*)(*T);
    auto NearPage = AllocateNearbyPage(Impl);

    if (!NearPage)
    {
        return;
    }

    uint8_t Shellcode[] =
    {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    memcpy(Shellcode + 6, &Detour, 8);
    memcpy(NearPage, Shellcode, sizeof(Shellcode));

    auto Offset = static_cast<int>(NearPage - (Impl + 7));

    DWORD OldProtect;
    VirtualProtect(Impl + 3, sizeof(int), PAGE_EXECUTE_READWRITE, &OldProtect);
    memcpy(Impl + 3, &Offset, sizeof(int));
    VirtualProtect(Impl + 3, sizeof(int), OldProtect, &OldProtect);
}