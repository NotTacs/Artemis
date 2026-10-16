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
	};
public:
	static int32 GetPlaylistMaxPlayerCount(UAthenaAIServicePlayerBots* __this);
	static void CalculateSpawnAmountAndSkill(UAthenaAIServicePlayerBots* __this); /*I LOWK DONT THINK THIS FUNC IS FINDABLE IN IDA BUT IDK*/
	static void UseFailSafeMMRInfo(UAthenaAIServicePlayerBots* __this, float SkillOverride);
	static void InitializeMMRInfos(UAthenaAIServicePlayerBots* __this);
	static void WaitForMatchAssignmentReady(UAthenaAIServicePlayerBots* __this, __int64 FlowHandle);
	static void LoadModule();
};