#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Json/Public/Dom/JsonObject.h"

class FortGameModeAthena
{
private:
	class Originals
	{
	public:
		static inline void (*FinishWorldInitialization)(AFortGameModeAthena* __this, AFortWorldManager* WorldManager);
		static inline void (*WorldManagerTick)(AFortWorldManager* WorldManager);
		static inline void* (*UpdateStreamingLevel)(void** Captures);
		static inline void (*UpdateLevelStreaming)(UWorld* World);
	};
public:
	static void FinishWorldInitialization(AFortGameModeAthena* __this, AFortWorldManager* WorldManager);
	static void WorldManagerTick(AFortWorldManager* WorldManager);
	static void* UpdateStreamingLevel(void** Captures);
	static void UpdateLevelStreaming(UWorld* World);
	static void OnGivenMatchAssignment(AFortGameModeAthena* __this, bool bSuccess, TSharedPtr<FJsonObject>& MatchOptions);
	static bool ReadyToStartMatch_Implementation(AFortGameModeAthena* __this);
	static APawn* SpawnDefaultPawnFor_Implementation(AFortGameModeAthena* __this, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot);
	static TSubclassOf<AGameSession>* GetGameSessionClass(AFortGameModeAthena* __this, TSubclassOf<AGameSession>* OutClass);
	static void LoadModule();
};