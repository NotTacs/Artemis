#include "pch.h"
#include "../../Public/AI/FortAthenaAIBotController.h"

void FortAthenaAIBotController::GiveInventory(AFortAthenaAIBotController* __this, TArray<FItemAndCount>& ItemList)
{
	if (!__this->HasAuthority())
	{
		return;
	}

	if (!__this->Inventory)
	{
		return;
	}

	TScriptInterface<IFortInventoryOwnerInterface> InventoryOwner = Cast<IFortInventoryOwnerInterface>(__this);

	for (const FItemAndCount& ItemAndCount : ItemList)
	{
		const UFortWorldItemDefinition* WorldItemDefinition = Cast<UFortWorldItemDefinition>(ItemAndCount.Item);
		if (!WorldItemDefinition || ItemAndCount.Count <= 0)
		{
			continue;
		}

		const UFortWorldItem* GivenItem = UFortKismetLibrary::GiveItemToInventoryOwner(InventoryOwner, WorldItemDefinition, ItemAndCount.ItemVariantGuid, ItemAndCount.Count, false, -1, -1, false);

		if (!GivenItem || GivenItem->GetNumberOfSlotsTaken() <= 0)
		{
			if (GivenItem)
			{
				UE_LOG(LogFortInventory, Warning, TEXT("Failed to give item %s to bot %s, item has no slots taken"), *WorldItemDefinition->GetName(), *__this->GetName());
			}
			else
			{
				UE_LOG(LogFortInventory, Warning, TEXT("Failed to give item %s to bot %s"), *WorldItemDefinition->GetName(), *__this->GetName());
			}
			continue;
		}

		bool bAlreadySlotted = false;
		FFortBotInventoryInfo* FreeSlot = nullptr;
		for (FFortBotInventoryInfo& SlotItem : __this->SlotItems)
		{
			if (SlotItem.FortItem == GivenItem)
			{
				bAlreadySlotted = true;
				break;
			}

			if (!FreeSlot && !SlotItem.FortItem)
			{
				FreeSlot = &SlotItem;
			}
		}

		if (!bAlreadySlotted && FreeSlot)
		{
			FreeSlot->ItemDefinition = const_cast<UFortWorldItemDefinition*>(WorldItemDefinition);
			FreeSlot->FortItem = const_cast<UFortWorldItem*>(GivenItem);
			FreeSlot->ItemType = WorldItemDefinition->GetItemType();
			FreeSlot->WeaponType = AFortWeapon::FindWeaponTypeForStats(WorldItemDefinition);
		}
	}
}