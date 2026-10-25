#include "pch.h"
#include "../../Public/Athena/FortGameStateAthena.h"
#include "FortniteGame\Source\FortniteAI\Public\Athena\AI\Systems\AthenaAIServicePlayerBots.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void FortGameStateAthena::PlacePlayersInAircraft(AFortGameStateAthena* __this)
{
	Originals::PlacePlayersInAircraft(__this);
	AthenaAIServicePlayerBots::PlacePlayerBotsInAircraft(UAthenaAIBlueprintLibrary::GetAIServicePlayerBots(UWorld::GetWorld()), __this->Aircrafts);
}

bool FortGameStateAthena::CanUpdateGamePhaseStep(AFortGameStateAthena* __this)
{
	return true; /*because IsDedicatedInstance does not return true until later on our gs (due to late inject) the gameinstance creates the loadingscreenmanager which shouldn't exist*/
}

void FortGameStateAthena::LoadModule()
{
	FMemoryLibrary::DetourHook(0x5fc03ec, PlacePlayersInAircraft, Originals::PlacePlayersInAircraft);
	FMemoryLibrary::DetourHook(0x1026d28, CanUpdateGamePhaseStep);
}