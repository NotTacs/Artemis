#pragma once
#include "CoreMinimal.h"

class FortPlayerControllerAthena
{
private:
public:
	static void ServerAcknowledgePossession_Implementation(AFortPlayerControllerAthena* __this, APawn* P);
	static void RemoveAllInventoryItems(AFortPlayerControllerAthena* __this);
	static void RemoveAllInventoryItemsThenReset(AFortPlayerControllerAthena* __this);
	static void ServerExecuteInventoryItem_Implementation(AFortPlayerControllerAthena* __this, const FGuid& ItemGuid);
	static void LoadModule();
};