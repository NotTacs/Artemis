#include "pch.h"
#include "../../Public/Athena/FortPlayerControllerAthena.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void FortPlayerControllerAthena::ServerAcknowledgePossession_Implementation(AFortPlayerControllerAthena* __this, APawn* P)
{
	static void (*APlayerController__ServerAcknowledgePossession_Implementation)(APlayerController * __this, APawn * P) = decltype(APlayerController__ServerAcknowledgePossession_Implementation)(APlayerController::GetDefaultObj()->VTable[0x122]);
	APlayerController__ServerAcknowledgePossession_Implementation(__this, P);

	static void (*UFortGlobals__InitializePlayerGameplayAbilities)(void*) = decltype(UFortGlobals__InitializePlayerGameplayAbilities)(InSDKUtils::GetImageBase() + 0x63edd2c);
	UFortGlobals__InitializePlayerGameplayAbilities(P->GetInterfaceAddress<IAbilitySystemInterface>());

	/* TODO: ADD GAMESTATE FUNC HERE*/
}

void FortPlayerControllerAthena::LoadModule()
{
	FMemoryLibrary::VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x122, ServerAcknowledgePossession_Implementation);
}