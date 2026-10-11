#pragma once
#include "framework.h"
#include "Engine/Source/Runtime/Json/Public/Dom/JsonObject.h"

class FortGameModeAthena
{
private:
	class Originals
	{
	public:
		static inline void (*FinishWorldInitialization)(AFortGameModeAthena* __this, AFortWorldManager* WorldManager);
	};
public:
	static void FinishWorldInitialization(AFortGameModeAthena* __this, AFortWorldManager* WorldManager);
	static void OnGivenMatchAssignment(AFortGameModeAthena* __this, bool bSuccess, TSharedPtr<FJsonObject>& MatchOptions);
	static bool ReadyToStartMatch(AFortGameModeAthena* __this);
	static APawn* SpawnDefaultPawnFor(AFortGameModeAthena* __this, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot);
	static void LoadModule();
};