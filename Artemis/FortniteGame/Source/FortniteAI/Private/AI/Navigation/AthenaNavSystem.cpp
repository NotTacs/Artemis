#include "pch.h"
#include "../../../Public/AI/Navigation/AthenaNavSystem.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void AthenaNavSystemConfig::CreateAndConfigureNavSystem(UAthenaNavSystemConfig* __this, UWorld* InWorld)
{
	__this->bAutoSpawnMissingNavData = true;
	AthenaNavSystemConfig::Originals::CreateAndConfigureNavSystem(__this, InWorld);
}

void AthenaNavSystemConfig::LoadModule()
{
	FMemoryLibrary::DetourHook(0x1F96F68, CreateAndConfigureNavSystem, Originals::CreateAndConfigureNavSystem);
}