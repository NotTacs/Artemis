#pragma once
#include "CoreMinimal.h"

class FortInventoryOwnerInterface
{
public:
	static void GiveInventoryItem(IFortInventoryOwnerInterface* __this, FFortItemEntry& ItemEntry);
	static void RemoveInventoryItem(IFortInventoryOwnerInterface* __this, const FGuid& ItemGuid);
	static void LoadModule();
};