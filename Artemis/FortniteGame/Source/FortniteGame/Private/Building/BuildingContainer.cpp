#include "pch.h"
#include "../../Public/Building/BuildingContainer.h"

void BuildingContainer::UninitializeTierGroupData(ABuildingContainer* __this)
{
	if (!__this->GetWorld())
	{
		return;
	}

	__this->ContainerLootTierKey = FName(L"None", EFindName::FNAME_Find);
	__this->SetReplicatedLootTier(INDEX_NONE);
	//__this->bLootSpawningUpdated = false;
}
void BuildingContainer::InitializeTierGroupData(ABuildingContainer* __this, EFortBuildingPersistentState PersistentState, bool bFirstUnInitialize)
{
	UWorld* World = __this->GetWorld();
	if (!World)
		return;

	if (bFirstUnInitialize)
	{
		BuildingContainer::UninitializeTierGroupData(__this);
	}

	if (__this->GetLocalRole() != ENetRole::ROLE_Authority)
	{
		return;
	}

	const bool bSpawnAllLoot = false;
	float ExistenceThreshold = bSpawnAllLoot ? 1.0f : 0.0f;

	AFortWorldManager* WorldManager = AFortWorldManager::Get(World);
}