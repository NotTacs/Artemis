#pragma once
#include "CoreMinimal.h"

class FortInventoryOwnerInterface
{
public:
	static bool CreateInventory(IFortInventoryOwnerInterface* __this);
	static void InitializeInventory(IFortInventoryOwnerInterface* __this, const TArray<FItemAndCount>& ItemList);
	static bool RemoveInventoryItem(IFortInventoryOwnerInterface* __this, FGuid ItemGuid, int32 Count, bool bForceRemoveFromQuickBars, bool bForceRemoval);
	static void LoadModule();
};