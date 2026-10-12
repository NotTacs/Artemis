#include "pch.h"
#include "../../Public/Lobby/FortGameModeEmptyDedicated.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

TSubclassOf<AGameSession>* FortGameModeEmptyDedicated::GetGameSessionClass(AFortGameModeEmptyDedicated* __this, TSubclassOf<AGameSession>* OutClass)
{
	*OutClass = AFortGameSessionDedicated::StaticClass();
	return OutClass;
}

void FortGameModeEmptyDedicated::LoadModule()
{
	FMemoryLibrary::VFTHook(AFortGameModeEmptyDedicated::GetDefaultObj()->VTable, 0xE3, GetGameSessionClass);
}
