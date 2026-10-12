#pragma once
#include "CoreMinimal.h"

class FortGameModeEmptyDedicated
{
public:
	static TSubclassOf<AGameSession>* GetGameSessionClass(AFortGameModeEmptyDedicated* __this, TSubclassOf<AGameSession>* OutClass);
	static void LoadModule();
};