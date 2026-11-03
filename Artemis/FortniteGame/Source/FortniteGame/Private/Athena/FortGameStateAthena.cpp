#include "pch.h"
#include "../../Public/Athena/FortGameStateAthena.h"
#include "FortniteGame\Source\FortniteAI\Public\Athena\AI\Systems\AthenaAIServicePlayerBots.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void FortGameStateAthena::PlacePlayersInAircraft(AFortGameStateAthena* __this)
{
	Originals::PlacePlayersInAircraft(__this);
	AthenaAIServicePlayerBots::PlacePlayerBotsInAircraft(UAthenaAIBlueprintLibrary::GetAIServicePlayerBots(UWorld::GetWorld()), __this->Aircrafts);
}

void FortGameStateAthena::LoadModule()
{
	FMemoryLibrary::DetourHook(0x5fc03ec, PlacePlayersInAircraft, Originals::PlacePlayersInAircraft);
}