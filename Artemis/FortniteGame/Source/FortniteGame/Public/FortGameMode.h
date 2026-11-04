#pragma once
#include "CoreMinimal.h"

extern float& GWallReplacementCooldown;
extern int32& GWallReplacementFavorOwner;
extern int32& GWallReplacementFavorNonOwner;

class FortGameMode
{
public:
	static void QueueRequestForNewContestedBuildingPiece(AFortGameMode* __this, ABuildingSMActor* ExistingActor, AFortPlayerController* Requester, 
		const FBuildingClassData& BuildingClassData, const FVector& BuildLoc, const FRotator& BuildRot, bool bMirrored);
	static void ProcessContestedBuildingRequests(AFortGameMode* __this);
};