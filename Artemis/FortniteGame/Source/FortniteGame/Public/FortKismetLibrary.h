#pragma once
#include "CoreMinimal.h"

class FortKismetLibrary
{
public:
	static const UFortWorldItem* GiveItemToInventoryOwner(TScriptInterface<IFortInventoryOwnerInterface> InventoryOwner, const UFortWorldItemDefinition* ItemDefinition, FGuid ItemVariantGuid, int32 NumberToGive, bool bNotifyPlayer, int32 ItemLevel, int32 PickupInstigatorHandle, bool bUseItemPickupAnalyticEvent);
	static DEFINE_FUNCTION(execGiveItemToInventoryOwner);
	static void LoadModule();
};