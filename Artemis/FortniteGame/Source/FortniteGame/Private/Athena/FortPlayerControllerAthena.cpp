#include "pch.h"
#include "../../Public/Athena/FortPlayerControllerAthena.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"


void FortPlayerControllerAthena::ServerAcknowledgePossession_Implementation(AFortPlayerControllerAthena* __this, APawn* P)
{
	static void (*APlayerController__ServerAcknowledgePossession_Implementation)(APlayerController * __this, APawn * P) = decltype(APlayerController__ServerAcknowledgePossession_Implementation)(APlayerController::GetDefaultObj()->VTable[0x122]);
	APlayerController__ServerAcknowledgePossession_Implementation(__this, P);

	IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(P);

	if (Interface == NULL)
		return;

	static void (*UFortGlobals__InitializePlayerGameplayAbilities)(void*) = decltype(UFortGlobals__InitializePlayerGameplayAbilities)(InSDKUtils::GetImageBase() + 0x63edd2c);
	UFortGlobals__InitializePlayerGameplayAbilities(Interface);

	/* TODO: ADD GAMESTATE FUNC HERE*/
}

void FortPlayerControllerAthena::RemoveAllInventoryItems(AFortPlayerControllerAthena* __this)
{
	printf(__FUNCTION__"\n");
	AFortInventory* WorldInventory = __this ? __this->WorldInventory : nullptr;

	if (!WorldInventory)
		return;

	//WorldInventory->Inventory.ReplicatedEntries.Free();
	//WorldInventory->Inventory.MarkArrayDirty();
	//WorldInventory->HandleInventoryLocalUpdate();
}

void FortPlayerControllerAthena::RemoveAllInventoryItemsThenReset(AFortPlayerControllerAthena* __this)
{
	RemoveAllInventoryItems(__this);
	InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<void (*)(AController*)>(__this, 0x650 / 8), __this);
}

void FortPlayerControllerAthena::ServerExecuteInventoryItem_Implementation(AFortPlayerControllerAthena* __this, const FGuid& ItemGuid)
{
	if (!__this || !__this->WorldInventory)
		return;
	AFortInventory* Inventory = __this->WorldInventory;
	UFortWorldItem** WorldItemPtr = Inventory->Inventory.ItemInstanceMap.Find(ItemGuid);
	UFortWorldItem* WorldItem = WorldItemPtr ? *WorldItemPtr : nullptr;
	if (WorldItem)
	{
		UFortWorldItemDefinition* WorldItemDefinition = Cast<UFortWorldItemDefinition>(WorldItem->ItemEntry.ItemDefinition);
		if (WorldItemDefinition)
		{
			WorldItemDefinition->ServerExecute(WorldItem, __this);
		}
	}
}

void FortPlayerControllerAthena::OnPossess(AFortPlayerControllerAthena* __this, AFortPlayerPawnAthena* NewPawn)
{
	FortPlayerControllerAthena::Originals::OnPossess(__this, NewPawn);
	if (__this->GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (AFortPlayerState* FortPlayerState = Cast<AFortPlayerState>(__this->PlayerState))
		{
			FortPlayerState->ApplyCustomizationToCharacter(__this, Cast<AFortPlayerPawn>(NewPawn));
			FortPlayerState->ForceNetUpdate();
		}
	}
}

void FortPlayerControllerAthena::ReservePortal(AFortPlayerControllerAthena* __this)
{

}

bool FortPlayerControllerAthena::PostLogin(AFortPlayerControllerAthena* __this)
{
	FortPlayerControllerAthena::Originals::PostLogin(__this);
	if (AFortCreativePortalManager::Get(__this))
	{

	}
}

void FortPlayerControllerAthena::LoadModule()
{
	FMemoryLibrary::VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x122, ServerAcknowledgePossession_Implementation);

	FMemoryLibrary::PatchBytesWithCall(0x5FA4D99,
		{ 0x48, 0x8B, 0x03, 0x48, 0x8B, 0xCB, 0xFF, 0x90, 0x50, 0x06, 0x00, 0x00 },
		{ 0x48, 0x8B, 0xCB },
		RemoveAllInventoryItemsThenReset);

	FMemoryLibrary::VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x22C, ServerExecuteInventoryItem_Implementation);
	FMemoryLibrary::DetourHook(0x6003998, OnPossess, Originals::OnPossess);
}