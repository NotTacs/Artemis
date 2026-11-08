#include "pch.h"
#include "../../Public/Items/FortInventoryOwnerInterface.h"
#include "FortniteGame/Source/FortniteAI/Public/AI/FortAthenaAIBotController.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

bool FortInventoryOwnerInterface::CreateInventory(IFortInventoryOwnerInterface* __this)
{
	AFortAthenaAIBotController* FortAIController = Cast<AFortAthenaAIBotController>(__this->_getUObject());
	if (!FortAIController)
	{
		return false;
	}

	if (!FortAIController->Inventory)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = FortAIController;
		SpawnParams.Instigator = FortAIController->PlayerBotPawn;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FortAIController->Inventory = FortAIController->GetWorld()->SpawnActor<AFortInventory>(AFortInventory::StaticClass(), SpawnParams);
		if (FortAIController->Inventory)
		{
			FortAIController->Inventory->InventoryType = EFortInventoryType::World;
		}
	}

	return FortAIController->Inventory != nullptr;
}

void FortInventoryOwnerInterface::InitializeInventory(IFortInventoryOwnerInterface* __this, const TArray<FItemAndCount>& ItemList)
{
	AFortAthenaAIBotController* FortAIController = Cast<AFortAthenaAIBotController>(__this->_getUObject());

	if (FortAIController)
	{
		if (FortAIController->StartupInventory && &FortAIController->StartupInventory->Items != &ItemList)
		{
			TArray<FItemAndCount>& StartupItems = FortAIController->StartupInventory->Items;
			StartupItems.Free();
			for (const FItemAndCount& Item : ItemList)
			{
				StartupItems.Add(Item);
			}
		}

		if (!FortAIController->Inventory)
		{
			FortInventoryOwnerInterface::CreateInventory(__this);
		}

		bool bShouldIgnoreInventoryRestrictions = false;
		if (FortAIController->CachedAIRuntimeParametersComponent)
		{
			UFortAthenaAIRuntimeParameters_Inventory* InventoryRuntimeParameters = Cast<UFortAthenaAIRuntimeParameters_Inventory>(UFortAIFunctionLibrary::GetOrCreateAIRuntimeParameters(FortAIController, UFortAthenaAIRuntimeParameters_Inventory::StaticClass()));
			if (InventoryRuntimeParameters)
			{
				if (InventoryRuntimeParameters->bSkipInventoryInitialization)
				{
					return;
				}

				bShouldIgnoreInventoryRestrictions = InventoryRuntimeParameters->bDefaultInventoryIgnoresRestrictions;
			}
		}

		FortAIController->bIgnoreInventoryRestrictions = bShouldIgnoreInventoryRestrictions;

		if (FortAIController->Inventory)
		{
			if (FortAIController->Blackboard)
			{
				FortAIController->Blackboard->SetValue<UBlackboardKeyType_Object>(FortAIController->WeaponKey, nullptr);
			}

			for (FFortBotInventoryInfo& SlotItem : FortAIController->SlotItems)
			{
				SlotItem.ItemDefinition = nullptr;
				SlotItem.FortItem = nullptr;
				SlotItem.ItemType = EFortItemType::Max_None;
				SlotItem.WeaponType = AFortWeapon::FindWeaponTypeForStats(nullptr);
			}

			FortAthenaAIBotController::GiveInventory(FortAIController, const_cast<TArray<FItemAndCount>&>(ItemList));

			const FFortBotTargetInfo* TargetInfo = FortAIController->TargetHandler.CurrentTargetIdx != INDEX_NONE ? FortAIController->TargetHandler.GetTargetInfo(FortAIController->TargetHandler.CurrentTargetIdx) : nullptr;
			FortAIController->EquipWeapon(FortAIController->GetBestWeaponForTargetInfo(TargetInfo, false, false));
		}

		FortAIController->bIgnoreInventoryRestrictions = false;
	}
}

bool FortInventoryOwnerInterface::RemoveInventoryItem(IFortInventoryOwnerInterface* __this, FGuid ItemGuid, int32 Count, bool bForceRemoval, bool bForcePersistWhenEmpty)
{
	UE_LOG(LogFortInventory, Log, TEXT("%s called."), ANSI_TO_TCHAR(__FUNCTION__));
	UObject* InventoryOwner = __this->_getUObject();
	if (auto FortPlayerController = Cast<AFortPlayerController>(InventoryOwner))
	{
		if (FortPlayerController->GetLocalRole() != ENetRole::ROLE_Authority)
		{
			FortPlayerController->ServerRemoveInventoryItem(ItemGuid, Count, bForceRemoval, false, false);
			return false;
		}

		AFortInventory* WorldInventory = FortPlayerController->WorldInventory;

		if (!WorldInventory || Count == 0)
		{
			return false;
		}

		UFortWorldItem* Item = WorldInventory->GetItem(ItemGuid);
		FFortItemEntry* ItemEntry = WorldInventory->GetReplicatedItemEntry(ItemGuid);
		if (!Item || !ItemEntry)
		{
			UE_LOG(LogFortInventory, Log, TEXT("Cannot find replicated ItemInstance or Entry"));
			return false;
		}

		if (Count > 0 && Count < ItemEntry->Count)
		{
			ItemEntry->SetCount(ItemEntry->Count - Count);
			WorldInventory->Inventory.MarkItemDirty(*ItemEntry);
			WorldInventory->HandleInventoryLocalUpdate();
			return true;
		}

		const UFortWorldItemDefinition* ItemDefinition = Cast<UFortWorldItemDefinition>(Item->ItemEntry.ItemDefinition);
		if (bForcePersistWhenEmpty || (ItemDefinition && ItemDefinition->ShouldPersistInInventoryWhenFinalStackEmpty()))
		{
			ItemEntry->SetCount(0);
			WorldInventory->Inventory.MarkItemDirty(*ItemEntry);
			WorldInventory->HandleInventoryLocalUpdate();
			return true;
		}

		WorldInventory->OnRemoveItemStack(Item, ItemGuid);
		Item->RemoveFromInventory();
		WorldInventory->HandleInventoryLocalUpdate();

		FortPlayerController->OnInventoryItemRemoved(WorldInventory, ItemGuid);
		return true;
	}
	else if (auto FortAIController = Cast<AFortAthenaAIBotController>(InventoryOwner))
	{
		if (!FortAIController->HasAuthority() || !FortAIController->Inventory || Count == 0)
		{
			return false;
		}

		AFortInventory* Inventory = FortAIController->Inventory;

		UFortWorldItem* Item = Inventory->GetItem(ItemGuid);
		FFortItemEntry* ItemEntry = Inventory->GetReplicatedItemEntry(ItemGuid);
		if (!Item || !ItemEntry)
		{
			return false;
		}

		if (Count > 0 && Count < ItemEntry->Count)
		{
			ItemEntry->SetCount(ItemEntry->Count - Count);
			Inventory->Inventory.MarkItemDirty(*ItemEntry);
			Inventory->HandleInventoryLocalUpdate();
			return true;
		}

		for (FFortBotInventoryInfo& SlotItem : FortAIController->SlotItems)
		{
			if (SlotItem.FortItem == Item)
			{
				SlotItem.ItemDefinition = nullptr;
				SlotItem.FortItem = nullptr;
				SlotItem.ItemType = EFortItemType::Max_None;
				SlotItem.WeaponType = AFortWeapon::FindWeaponTypeForStats(nullptr);
			}
		}

		Inventory->OnRemoveItemStack(Item, ItemGuid);
		Item->RemoveFromInventory();
		Inventory->HandleInventoryLocalUpdate();
		return true;
	}
	else if (auto FortAIPawn = Cast<AFortAIPawn>(InventoryOwner))
	{
		if (!FortAIPawn->HasAuthority() || !FortAIPawn->bHasInventory || !FortAIPawn->Inventory || Count == 0)
		{
			return false;
		}

		UFortWorldItem* Item = FortAIPawn->Inventory->GetItem(ItemGuid);
		FFortItemEntry* ItemEntry = FortAIPawn->Inventory->GetReplicatedItemEntry(ItemGuid);
		if (!Item || !ItemEntry)
		{
			return false;
		}

		if (Count > 0 && Count < ItemEntry->Count)
		{
			ItemEntry->SetCount(ItemEntry->Count - Count);
			FortAIPawn->Inventory->Inventory.MarkItemDirty(*ItemEntry);
			FortAIPawn->Inventory->HandleInventoryLocalUpdate();
			return true;
		}

		FortAIPawn->Inventory->OnRemoveItemStack(Item, ItemGuid);
		Item->RemoveFromInventory();
		FortAIPawn->Inventory->HandleInventoryLocalUpdate();
		return true;
	}
	return false;
}

void FortInventoryOwnerInterface::LoadModule()
{
	FMemoryLibrary::VFTHook(Cast<IFortInventoryOwnerInterface>(AFortAthenaAIBotController::GetDefaultObj())->VTable, 0x3, CreateInventory);
	FMemoryLibrary::VFTHook(Cast<IFortInventoryOwnerInterface>(AFortAthenaAIBotController::GetDefaultObj())->VTable, 0x4, InitializeInventory);
	FMemoryLibrary::VFTHook(Cast<IFortInventoryOwnerInterface>(AFortAthenaAIBotController::GetDefaultObj())->VTable, 0x2F, RemoveInventoryItem);
	FMemoryLibrary::VFTHook(Cast<IFortInventoryOwnerInterface>(AFortAIPawn::GetDefaultObj())->VTable, 0x2F, RemoveInventoryItem);
	FMemoryLibrary::VFTHook(Cast<IFortInventoryOwnerInterface>(AFortPlayerControllerAthena::GetDefaultObj())->VTable, 0x2F, RemoveInventoryItem);
	FMemoryLibrary::VFTHook(Cast<IFortInventoryOwnerInterface>(AFortPlayerController::GetDefaultObj())->VTable, 0x2F, RemoveInventoryItem);
}