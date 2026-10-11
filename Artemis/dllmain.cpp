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
    MH_Initialize();
    Sleep(5000);

    Engine::LoadModule();
    World::LoadModule();
    /*TODO: MOVE TO OWN FILE*/
    FMemoryLibrary::Set<bool>(0xB30CF9F, false); //GIsClient
    FMemoryLibrary::Set<bool>(0xB30CF9F + 1, true); //GISServer
    FortGameModeAthena::LoadModule();
    NetDriver::LoadModule();
    FMemoryLibrary::SetTrue(0x78857F4); //AGameSession::KickPlayer
    FMemoryLibrary::SetNull(0x258D0DC); //AFortGameSession::StoreSessionID undo this later
    FMemoryLibrary::SetTrue(0x17dd630); //UGameInstance::IsDedicatedServerInstance

    /* THIS IS SKUNKY WE NEED TO DO PROPER */
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers[0]->PlayerController->SwitchLevel(L"Artemis_Terrain");
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);

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

