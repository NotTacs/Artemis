#include "pch.h"
#include "../../../Public/Athena/AI/FortAthenaAISpawnerDataComponent_AIBotCosmeticBase.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void FortAthenaAISpawnerDataComponent_AIBotCosmeticBase::GetLoadout_Implementation(UFortAthenaAISpawnerDataComponent_AIBotCosmeticBase* __this, FFortAthenaLoadout* OutLoadout)
{
	printf(__FUNCTION__"\n");
	/*TODO: Make this proper and not be scuffed.*/
	OutLoadout->Character = UObject::StaticLoadObject<UAthenaCharacterItemDefinition>("/Game/Athena/Items/Cosmetics/Characters/CID_001_Athena_Commando_F_Default.CID_001_Athena_Commando_F_Default");
	OutLoadout->Pickaxe = UObject::StaticLoadObject<UAthenaPickaxeItemDefinition>("/Game/Athena/Items/Cosmetics/Pickaxes/DefaultPickaxe.DefaultPickaxe");
}

void FortAthenaAISpawnerDataComponent_AIBotCosmeticBase::LoadModule()
{
	FMemoryLibrary::DetourHook(0x6a41de4, GetLoadout_Implementation);
}