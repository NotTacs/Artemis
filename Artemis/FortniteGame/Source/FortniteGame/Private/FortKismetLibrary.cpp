#include "pch.h"
#include "../Public/FortKismetLibrary.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

const UFortWorldItem* FortKismetLibrary::GiveItemToInventoryOwner(TScriptInterface<IFortInventoryOwnerInterface> InventoryOwner, const UFortWorldItemDefinition* ItemDefinition, FGuid ItemVariantGuid, int32 NumberToGive, bool bNotifyPlayer, int32 ItemLevel, int32 PickupInstigatorHandle, bool bUseItemPickupAnalyticEvent)
{
	IFortInventoryOwnerInterface* Owner = InventoryOwner.GetInterface();
	if (!Owner || !ItemDefinition || NumberToGive <= 0) {
		UE_LOG(LogFortInventory, Log, TEXT("Failed to give item to inventory owner: Invalid parameters."));
		return nullptr;
	}

	AFortInventory* Inventory = Owner->GetWorldInventory();
	if (!Inventory || !Inventory->HasAuthority()) 
	{
		UE_LOG(LogFortInventory, Error, TEXT("Failed to find Inventory or Inventory doesn't have authority."));
		return nullptr;
	}
	const int32 Level = ItemLevel >= 0 ? ItemLevel : ItemDefinition->MinLevel;
	FFortItemEntry ItemDescription(ItemDefinition, NumberToGive, Level, ItemVariantGuid);
	if (bNotifyPlayer)
	{
		ItemDescription.SetStateValue<int32>(EFortItemEntryState::ShouldShowItemToast, 1);
	}
	else
	{
		ItemDescription.SetStateValue<int32>(EFortItemEntryState::ShouldShowItemToast, 0);
	}

	const int32 Overflow = AFortInventory::GetOverflowFromAddingItem(ItemDescription, Owner, Inventory->InventoryType, 0);
	TArray<TTuple<const UFortWorldItem*, int32>> AddingPlan;
	for (const TTuple<const UFortWorldItem*, int32>& Step : AFortInventory::AddingItemPlan)
	{
		AddingPlan.Add(Step);
	}

	const bool bAllowOverflow = Overflow > 0 && Owner->IsItemDescriptionAllowedInOverflow(ItemDescription, false, true);
	if (Overflow > 0 && !bAllowOverflow)
	{
		UE_LOG(LogFortInventory, Verbose, TEXT("%s: %d of %d %s did not fit in the inventory"), TEXT("UFortKismetLibrary::GiveItemFromPlayer"), Overflow, NumberToGive, *ItemDefinition->GetName());
	}

	FGuid LastTouchedGuid;
	int32 NumGiven = 0;
	for (const TTuple<const UFortWorldItem*, int32>& Step : AddingPlan)
	{
		const UFortWorldItem* ExistingItem = Step.Get<0>();
		const int32 Amount = Step.Get<1>();

		if (Amount <= 0)
		{
			continue;
		}

		const int32 AmountToGive = bAllowOverflow ? Amount : UKismetMathLibrary::Min(Amount, NumberToGive - Overflow - NumGiven);
		if (AmountToGive <= 0)
		{
			break;
		}

		if (ExistingItem)
		{
			if (FFortItemEntry* ExistingEntry = Inventory->GetReplicatedItemEntry(ExistingItem->GetItemGuid()))
			{
				ExistingEntry->SetCount(ExistingEntry->Count + AmountToGive);
				Inventory->Inventory.MarkItemDirty(*ExistingEntry);
				LastTouchedGuid = ExistingEntry->ItemGuid;
				NumGiven += AmountToGive;
			}

			continue;
		}

		FFortItemEntry& NewEntry = Inventory->Inventory.ReplicatedEntries.Add_GetRef(ItemDescription);
		NewEntry.CreateNewGuid();
		NewEntry.SetCount(AmountToGive);
		NewEntry.SetParentInventory(Inventory, true);
		Inventory->Inventory.MarkItemDirty(NewEntry);

		LastTouchedGuid = NewEntry.ItemGuid;
		NumGiven += AmountToGive;
	}
	AddingPlan.Free();

	if (bAllowOverflow && NumGiven < NumberToGive)
	{
		FFortItemEntry& OverflowEntry = Inventory->Inventory.ReplicatedEntries.Add_GetRef(ItemDescription);
		OverflowEntry.CreateNewGuid();
		OverflowEntry.SetCount(NumberToGive - NumGiven);
		OverflowEntry.SetInInventoryOverflow(true);
		OverflowEntry.SetParentInventory(Inventory, true);
		Inventory->Inventory.MarkItemDirty(OverflowEntry);

		LastTouchedGuid = OverflowEntry.ItemGuid;
		NumGiven = NumberToGive;
	}

	if (NumGiven <= 0)
	{
		return nullptr;
	}

	Inventory->HandleInventoryLocalUpdate();

	return Inventory->GetItem(LastTouchedGuid);
}

DEFINE_FUNCTION(FortKismetLibrary::execGiveItemToInventoryOwner)
{
	P_GET_TINTERFACE(IFortInventoryOwnerInterface, InventoryOwner);
	P_GET_OBJECT(const UFortWorldItemDefinition, ItemDefinition);
	P_GET_STRUCT(FGuid, ItemVariantGuid);
	P_GET_PROPERTY(FIntProperty, NumberToGive);
	P_GET_UBOOL(bNotifyPlayer);
	P_GET_PROPERTY(FIntProperty, ItemLevel);
	P_GET_PROPERTY(FIntProperty, PickupInstigatorHandle);
	P_GET_UBOOL(bUseItemPickupAnalyticEvent);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(const UFortWorldItem**)RESULT_PARAM = GiveItemToInventoryOwner(InventoryOwner, ItemDefinition, ItemVariantGuid, NumberToGive, bNotifyPlayer, ItemLevel, PickupInstigatorHandle, bUseItemPickupAnalyticEvent);
	P_NATIVE_END;
}

bool FortKismetLibrary::PickLootDropsInternal(UObject* WorldContextObject, TArray<FFortItemEntry>& OutLootToDrop, FName TierGroupName, int32 WorldLevel, const TMap<FName, float>* NamedWeightsMap, int32 ForcedLootTier)
{
	return false;
}

DEFINE_FUNCTION(FortKismetLibrary::execPickLootDropsWithNamedWeights)
{
	P_GET_OBJECT(UObject, WorldContextObject);
	P_GET_ARRAY_REF(TArray<FFortItemEntry>, OutLootToDrop);
	P_GET_PROPERTY(FNameProperty, TierGroupName);
	P_GET_PROPERTY(FIntProperty, WorldLevel);
	P_GET_TMAP_REF(FName, float, NamedWeightsMap);
	P_GET_PROPERTY(FIntProperty, ForcedLootTier);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)RESULT_PARAM = PickLootDropsInternal(WorldContextObject, *OutLootToDrop, TierGroupName, WorldLevel, &NamedWeightsMap, ForcedLootTier);
	P_NATIVE_END;
}

DEFINE_FUNCTION(FortKismetLibrary::execPickLootDrops)
{
	P_GET_OBJECT(UObject, WorldContextObject);
	P_GET_ARRAY_REF(TArray<FFortItemEntry>, OutLootToDrop);
	P_GET_PROPERTY(FNameProperty, TierGroupName);
	P_GET_PROPERTY(FIntProperty, WorldLevel);
	P_GET_PROPERTY(FIntProperty, ForcedLootTier);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)RESULT_PARAM = PickLootDropsInternal(WorldContextObject, *OutLootToDrop, TierGroupName, WorldLevel, nullptr, ForcedLootTier);
	P_NATIVE_END;
}

void FortKismetLibrary::LoadModule()
{
	FMemoryLibrary::ExecHook(UFortKismetLibrary::StaticClass()->FindFunctionByName(FName(L"GiveItemToInventoryOwner", EFindName::FNAME_Add)), execGiveItemToInventoryOwner);
	FMemoryLibrary::ExecHook(UFortKismetLibrary::StaticClass()->FindFunctionByName(FName(L"PickLootDrops", EFindName::FNAME_Add)), execPickLootDrops);
	FMemoryLibrary::ExecHook(UFortKismetLibrary::StaticClass()->FindFunctionByName(FName(L"PickLootDropsWithNamedWeights", EFindName::FNAME_Add)), execPickLootDropsWithNamedWeights);
}