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

void FModuleManager::LoadAllModules()
{
	UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogMatchmakingServiceDedicatedServer all", nullptr);
	GameMapsSettings::LoadModule();
	World::LoadModule();
	FortGameModeAthena::LoadModule();
	NetDriver::LoadModule();
	/*TODO: MOVE TO OWN FILE*/
	FMemoryLibrary::Set<bool>(0xB30CF9F, false); //GIsClient
	FMemoryLibrary::Set<bool>(0xB30CF9F + 1, true); //GISServer
	FMemoryLibrary::SetTrue(0x78857F4); //AGameSession::KickPlayer
	//FMemoryLibrary::SetNull(0x258D0DC); //AFortGameSession::StoreSessionID
	OnlineBeaconClient::LoadModule();
	McpProfileGroup::LoadModule();
	FCommandLine::LoadModule();
	FortGameSessionDedicated::LoadModule();
	FortGameModeEmptyDedicated::LoadModule();
	FMemoryLibrary::SetTrue(0xce8648); //FOnlineSubSystemImpl::IsDedicated
	FMemoryLibrary::SetNull(0x64540A0); //FortGameSessionDedicatedAthena::OnVerifyAuthComplete SCUFFED
	
}