#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Core/Public/Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTemp, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogArtemis, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, Verbose);
DECLARE_LOG_CATEGORY_EXTERN(LogAthenaAIServicePlayerBots, Log, Verbose);
DECLARE_LOG_CATEGORY_EXTERN(LogFort, VeryVerbose, VeryVerbose);

class CoreGlobals
{
public:
	static void LoadModule();
};