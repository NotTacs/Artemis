#pragma once
#include "CoreMinimal.h"

class FortPlayerController
{
public:
	static void ServerCreateBuildingActor_Implementation(AFortPlayerController* __this, FCreateBuildingActorData& CreateBuildingData);
	static bool FixUpCreateBuildingClassData(AFortPlayerController* __this,FBuildingClassData& BuildingClassData);
	static bool ServerCreateBuildingActorInternal(AFortPlayerController* __this, FBuildingClassData& BuildingClassData, FVector& BuildLoc, FRotator& BuildRot, bool bMirrored);

	static void ServerBeginEditingBuildingActor_Implementation(AFortPlayerController* __this, ABuildingSMActor* BuildingActorToEdit);
	static void ServerEditBuildingActor_Implementation(AFortPlayerController* __this, ABuildingSMActor* BuildingActorToEdit, UClass* NewBuildingClass, uint8 RotationIterations, bool bMirrored);
	static void ServerEndEditingBuildingActor_Implementation(AFortPlayerController* __this, ABuildingSMActor* BuildingActorToStopEditing);
	static void LoadModule();
};