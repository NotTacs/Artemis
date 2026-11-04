#include "pch.h"
#include "../../Public/Items/FortInventoryOwnerInterface.h"

void FortInventoryOwnerInterface::GiveInventoryItem(IFortInventoryOwnerInterface* __this, FFortItemEntry& ItemEntry)
{
	if (!__this || ItemEntry.Count < 0 || ItemEntry.ItemDefinition == nullptr)
		return;
	UObject* Owner = __this->_getUObject();
	/* THIS BLOCK WILL BE REPLACED WITH GetWorldInventory*/
	AFortInventory* OwnerInventory = nullptr;
	if (AFortPlayerControllerAthena* FortPlayerController = Cast<AFortPlayerControllerAthena>(Owner))
	{
		OwnerInventory = FortPlayerController->WorldInventory;
	}
	else if (AFortAthenaAIBotController* FortAIController = Cast<AFortAthenaAIBotController>(Owner))
	{
		OwnerInventory = FortAIController->Inventory;
	}
	if (!OwnerInventory)
	{
		UE_LOG(LogInventory, Error, TEXT("IFortInventoryOwnerInterface::GiveInventoryItem() : Failed to find OwnerInventory."));
		return;
	}
	ItemEntry.ParentInventory = OwnerInventory;

	OwnerInventory->Inventory.ReplicatedEntries.Add(ItemEntry);
	OwnerInventory->Inventory.MarkArrayDirty();
	OwnerInventory->HandleInventoryLocalUpdate();
}

void FortInventoryOwnerInterface::RemoveInventoryItem(IFortInventoryOwnerInterface* __this, const FGuid& ItemGuid)
{
	if (!__this)
		return;
	AFortInventory* OwnerInventory = __this->GetWorldInventory();
	if (!OwnerInventory)
	{
		UE_LOG(LogInventory, Error, TEXT("IFortInventoryOwnerInterface::RemoveInventoryItem() : Failed to find OwnerInventory."));
		return;
	}
	FFortItemEntry RemovedItemEntry;
	for (int32 EntryIndex = 0; EntryIndex < OwnerInventory->Inventory.ReplicatedEntries.Num(); EntryIndex++)
	{
		if (ItemGuid == OwnerInventory->Inventory.ReplicatedEntries[EntryIndex].ItemGuid)
		{
			RemovedItemEntry = OwnerInventory->Inventory.ReplicatedEntries[EntryIndex];
			OwnerInventory->Inventory.ReplicatedEntries.Remove(EntryIndex);
			break;
		}
	}

	int32 PickupInstigatorHandle = -1;
	/*TODO: ADD GetStateValue*/
	bool bHasPickupInstigatorHandle = false;
	for (auto& StateValue : RemovedItemEntry.StateValues)
	{
		if (StateValue.StateType == EFortItemEntryState::PickupInstigatorHandle)
		{
			PickupInstigatorHandle = StateValue.IntValue;
			bHasPickupInstigatorHandle = true;
			break;
		}
	}
	if (bHasPickupInstigatorHandle && PickupInstigatorHandle != -1)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(__this->_getUObject()))
		{
			/*if (UFortPickupInstigatorRegistryComponent* PickupRegistry = UFortPickupInstigatorRegistryComponent)*/
		}
	}

	OwnerInventory->HandleInventoryLocalUpdate();
}