#include "pch.h"
#include "../../Public/Modules/ModuleManager.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"
#include "Engine/Source/Runtime/Engine/Classes/Engine/Engine.h"
#include "Engine/Source/Runtime/Engine/Classes/Engine/World.h"
#include "FortniteGame\Source\FortniteGame\Public\Athena\FortGameModeAthena.h"
#include "Engine\Source\Runtime\Engine\Classes\Engine\NetDriver.h"
#include "Engine\Source\Runtime\EngineSettings\Public\EngineSettingsModule.h"
#include "FortniteGame\Source\FortniteGame\Public\FortGameInstance.h"
#include "Engine\Plugins\Online\OnlineSubsystemUtils\Source\OnlineSubsystemUtils\Public\OnlineBeaconClient.h"
#include "Engine\Restricted\NotForLicensees\Plugins\OnlineGameplayFramework\Source\McpProfileSys\Public\McpProfileGroup.h"
#include "Engine\Source\Runtime\Core\Public\Misc\CommandLine.h"
#include "FortniteGame\Source\FortniteGame\Public\FortGameSessionDedicated.h"
#include "FortniteGame\Source\FortniteGame\Public\Lobby\FortGameModeEmptyDedicated.h"
#include "Engine\Source\Runtime\Core\Public\CoreGlobals.h"
#include "FortniteGame\Source\FortniteGame\Public\Online\FortGameSessionDedicatedAthena.h"
#include "Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Public\AbilitySystemComponent.h"
#include "FortniteGame\Source\FortniteGame\Public\Components\FortControllerComponent_Aircraft.h"
#include "FortniteGame\Source\FortniteGame\Public\Athena\FortPlayerControllerAthena.h"
#include "FortniteGame\Source\FortniteAI\Public\Athena\AI\Systems\AthenaAIServicePlayerBots.h"
#include "FortniteGame\Source\FortniteAI\Public\AI\Navigation\AthenaNavSystem.h"
#include "Engine\Source\Runtime\CoreUObject\Public\UObject\Object.h"

void FModuleManager::LoadAllModules()
{
	UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogMatchmakingServiceDedicatedServer all", nullptr);
	GameMapsSettings::LoadModule();
	World::LoadModule();
	FortGameModeAthena::LoadModule();
	NetDriver::LoadModule();
	CoreGlobals::LoadModule();
	FMemoryLibrary::SetTrue(0x78857F4); //AGameSession::KickPlayer
	//FMemoryLibrary::SetNull(0x258D0DC); //AFortGameState::StoreSessionId
	OnlineBeaconClient::LoadModule();
	McpProfileGroup::LoadModule();
	FCommandLine::LoadModule();
	FortGameSessionDedicated::LoadModule();
	FortGameModeEmptyDedicated::LoadModule();
	FMemoryLibrary::SetTrue(0xce8648); //FOnlineSubSystemImpl::IsDedicated
	FMemoryLibrary::SetNull(0x64540A0); //FortGameSessionDedicatedAthena::OnVerifyAuthComplete SCUFFED
	Object::LoadModule();
	FortGameSessionDedicatedAthena::LoadModule();
	AthenaAIServicePlayerBots::LoadModule();
	AthenaNavSystemConfig::LoadModule();
	FortPlayerControllerAthena::LoadModule();
	AbilitySystemComponent::LoadModule();
	FortControllerComponent_Aircraft::LoadModule();
	UE_LOG(LogModuleManager, Verbose, TEXT("All modules loaded"));
}