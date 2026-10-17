#include "pch.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"
#include "Engine/Source/Runtime/Engine/Classes/Engine/Engine.h"
#include "Engine/Source/Runtime/Engine/Classes/Engine/World.h"
#include "FortniteGame\Source\FortniteGame\Public\Athena\FortGameModeAthena.h"
#include "Engine\Source\Runtime\Engine\Classes\Engine\NetDriver.h"
#include "Engine\Source\Runtime\Core\Public\CoreGlobals.h"
#include "Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Public\AbilitySystemComponent.h"
#include "FortniteGame\Source\FortniteGame\Public\Components\FortControllerComponent_Aircraft.h"
#include "FortniteGame\Source\FortniteGame\Public\Athena\FortPlayerControllerAthena.h"
#include "FortniteGame\Source\FortniteAI\Public\Athena\AI\Systems\AthenaAIServicePlayerBots.h"
#include "FortniteGame\Source\FortniteAI\Public\AI\Navigation\AthenaNavSystem.h"
#include "Engine\Source\Runtime\CoreUObject\Public\UObject\Object.h"
#include "Backend/Public/Backend.h"

DWORD WINAPI Main(LPVOID lpThread)
{
    AllocConsole();
    FILE* File = nullptr;
    freopen_s(&File, "CONOUT$", "w+", stdout);
    MH_Initialize();
    Sleep(5000);
    Backend::LoadModule();
    Engine::LoadModule();
    World::LoadModule();
    CoreGlobals::LoadModule();
    FortGameModeAthena::LoadModule();
    NetDriver::LoadModule();
    AbilitySystemComponent::LoadModule();
    FortControllerComponent_Aircraft::LoadModule();
    FortPlayerControllerAthena::LoadModule();
    FMemoryLibrary::SetTrue(0x78857F4); //AGameSession::KickPlayer
    FMemoryLibrary::SetNull(0x258D0DC); //AFortGameState::StoreGameSessionID undo this later
    FMemoryLibrary::SetTrue(0x17dd630); //UGameInstance::IsDedicatedServerInstance
    AthenaAIServicePlayerBots::LoadModule();
    Object::LoadModule();
    AthenaNavSystemConfig::LoadModule();
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogAthenaBots VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogFortUIDirector", nullptr);
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

