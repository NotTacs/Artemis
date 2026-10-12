#include "pch.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"
#include "Engine/Source/Runtime/Engine/Classes/Engine/Engine.h"
#include "Engine/Source/Runtime/Engine/Classes/Engine/World.h"
#include "FortniteGame\Source\FortniteGame\Public\Athena\FortGameModeAthena.h"
#include "Engine\Source\Runtime\Engine\Classes\Engine\NetDriver.h"

DWORD WINAPI Main(LPVOID lpThread)
{
    AllocConsole();
    FILE* File = nullptr;
    freopen_s(&File, "CONOUT$", "w+", stdout);
    FMemoryLibrary::Setup();
    printf("Initialized Minhook.\n");
    Engine::InitEngine();

    /* THIS IS SKUNKY WE NEED TO DO PROPER */
    //UWorld::GetWorld()->OwningGameInstance->LocalPlayers[0]->PlayerController->SwitchLevel(L"Artemis_Terrain");
    

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

