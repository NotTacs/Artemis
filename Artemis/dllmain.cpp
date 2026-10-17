#include "pch.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"
#include "Engine/Source/Runtime/Engine/Classes/Engine/Engine.h"

DWORD WINAPI Main(LPVOID lpThread)
{
    AllocConsole();
    FILE* File = nullptr;
    freopen_s(&File, "CONOUT$", "w+", stdout);
    FMemoryLibrary::Setup();
    Engine::InitEngine();

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    if (reason == 1)
    {
        CreateThread(0, 0, Main, 0, 0, 0);
    }
    return TRUE;
}

