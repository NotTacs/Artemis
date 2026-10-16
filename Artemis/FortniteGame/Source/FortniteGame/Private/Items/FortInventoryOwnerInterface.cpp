#include "pch.h"
#include "../../Public/Items/FortInventoryOwnerInterface.h"

void FortInventoryOwnerInterface::GiveInventoryItem(IFortInventoryOwnerInterface* __this, const FFortItemEntry& ItemEntry)
{
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
		UE_LOG(LogInventory, Verbose, TEXT("IFortInventoryOwnerInterface::GiveInventoryItem() : Failed to find OwnerInventory."));
		return;
	}

	OwnerInventory->Inventory.ReplicatedEntries.Add(ItemEntry);
	OwnerInventory->Inventory.MarkArrayDirty();
	OwnerInventory->HandleInventoryLocalUpdate();
}