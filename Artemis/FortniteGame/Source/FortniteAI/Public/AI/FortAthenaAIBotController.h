#pragma once
#include "CoreMinimal.h"

class FortAthenaAIBotController
{
public:
	static void GiveInventory(AFortAthenaAIBotController* __this, TArray<FItemAndCount>& ItemList);
	static void LoadModule();
};