#pragma once
#include "CoreMinimal.h"

class FortInventoryOwnerInterface
{
public:
	static void GiveInventoryItem(IFortInventoryOwnerInterface* __this, FFortItemEntry& ItemEntry);
	static void LoadModule();
};