#pragma once
#include "CoreMinimal.h"

class BuildingContainer
{
public:
	static void UninitializeTierGroupData(ABuildingContainer* __this);
	static void InitializeTierGroupData(ABuildingContainer* __this, EFortBuildingPersistentState PersistentState, bool bFirstUnInitialize);
	static void LoadModule();
};