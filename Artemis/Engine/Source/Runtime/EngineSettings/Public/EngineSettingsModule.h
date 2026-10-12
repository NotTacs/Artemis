#pragma once
#include "CoreMinimal.h"

class GameMapsSettings
{
public:
	static FString GetGameDefaultMap();
	static FString GetGlobalDefaultGameMode();
	static void LoadModule();
};