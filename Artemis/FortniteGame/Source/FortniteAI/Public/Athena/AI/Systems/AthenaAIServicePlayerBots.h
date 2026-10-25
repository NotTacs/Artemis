#pragma once
#include "CoreMinimal.h"

namespace PlayerBotsConsoleVariables
{
	extern int32 BotMaxSpawnAmount;
}

class AthenaAIServicePlayerBots
{
private:
	class Originals
	{
	public:
		static inline void (*WaitForMatchAssignmentReady)(UAthenaAIServicePlayerBots* __this, __int64 FlowHandle);
		static inline void (*OnPlayerBotSpawned)(UAthenaAIServicePlayerBots* __this, AActor* Actor, const int SpawnRequestID, double PawnMMR);
	};
public:
	static int32 GetPlaylistMaxPlayerCount(UAthenaAIServicePlayerBots* __this);
	static void CalculateSpawnAmountAndSkill(UAthenaAIServicePlayerBots* __this); /*I LOWK DONT THINK THIS FUNC IS FINDABLE IN IDA BUT IDK*/
	static void UseFailSafeMMRInfo(UAthenaAIServicePlayerBots* __this, float SkillOverride);
	static void InitializeMMRInfos(UAthenaAIServicePlayerBots* __this);
	static void PlacePlayerBotsInAircraft(UAthenaAIServicePlayerBots* __this, const TArray<AFortAthenaAircraft*> Aircrafts);
	static void OnPlayerBotSpawned(UAthenaAIServicePlayerBots* __this, AActor* Actor, const int SpawnRequestID, double PawnMMR);
	static void LoadModule();
};