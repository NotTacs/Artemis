#pragma once
#include "CoreMinimal.h"

class FortKismetLibrary
{
public:
	static const UFortWorldItem* GiveItemToInventoryOwner(TScriptInterface<IFortInventoryOwnerInterface> InventoryOwner, const UFortWorldItemDefinition* ItemDefinition, FGuid ItemVariantGuid, int32 NumberToGive, bool bNotifyPlayer, int32 ItemLevel, int32 PickupInstigatorHandle, bool bUseItemPickupAnalyticEvent);
	static DEFINE_FUNCTION(execGiveItemToInventoryOwner);
	static bool PickLootDropsInternal(UObject* WorldContextObject, TArray<FFortItemEntry>& OutLootToDrop, FName TierGroupName, int32 WorldLevel, const TMap<FName, float>* NamedWeightsMap, int32 ForcedLootTier);
	static bool PickLootDrops(UObject* WorldContextObject, TArray<FFortItemEntry>& OutLootToDrop, FName TierGroupName, int32 WorldLevel, int32 ForcedLootTier);
	static bool PickLootDropsWithNamedWeights(UObject* WorldContextObject, TArray<FFortItemEntry>& OutLootToDrop, FName TierGroupName, int32 WorldLevel, const TMap<FName, float>& NamedWeightsMap, int32 ForcedLootTier);
	static DEFINE_FUNCTION(execPickLootDrops);
	static DEFINE_FUNCTION(execPickLootDropsWithNamedWeights);
	static void LoadModule();
};