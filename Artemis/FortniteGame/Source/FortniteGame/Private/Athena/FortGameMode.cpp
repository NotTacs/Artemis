#include "pch.h"
#include "../../Public/FortGameMode.h"
#include "../../Public/Player/FortPlayerController.h"

float& GWallReplacementCooldown = *(float*)(InSDKUtils::GetImageBase() + 0xB1A2F5C);
int32& GWallReplacementFavorOwner = *(int32*)(InSDKUtils::GetImageBase() + 0xB4CDA38);
int32& GWallReplacementFavorNonOwner = *(int32*)(InSDKUtils::GetImageBase() + 0xB4CDA34);

void FortGameMode::QueueRequestForNewContestedBuildingPiece(AFortGameMode* __this, ABuildingSMActor* ExistingActor, AFortPlayerController* Requester,
	const FBuildingClassData& BuildingClassData, const FVector& BuildLoc, const FRotator& BuildRot, bool bMirrored)
{
	if (!ExistingActor || !ExistingActor->bDestroyed)
	{
		return;
	}

	TArray<FContestedBuildingRequest>& Requests = __this->ContestedBuildingRequests().FindOrAdd(FObjectKey(ExistingActor));

	for (const FContestedBuildingRequest& Request : Requests)
	{
		if (Request.Requester == Requester)
		{
			return;
		}
	}

	FContestedBuildingRequest NewRequest{};
	NewRequest.Requester = Requester;
	NewRequest.BuildingClassData = BuildingClassData;
	NewRequest.BuildLoc = BuildLoc;
	NewRequest.BuildRot = BuildRot;
	NewRequest.bMirrored = bMirrored;
	Requests.Add(NewRequest);
}

void FortGameMode::ProcessContestedBuildingRequests(AFortGameMode* __this)
{
	UWorld* World = __this->GetWorld();

	for (int i = 0; i < __this->ContestedBuildingRequests().Num(); i++)
	{
		ABuildingSMActor* ExistingActor = __this->ContestedBuildingRequests()[i].Key().ResolveObjectPtr<ABuildingSMActor>();
		TArray<FContestedBuildingRequest>& Requests = __this->ContestedBuildingRequests()[i].Value();
		if (ExistingActor && UGameplayStatics::GetTimeSeconds(UWorld::GetWorld()) - ExistingActor->DestroyedTime <= GWallReplacementCooldown)
		{
			continue;
		}

		bool bBuilt = false;

		if (ExistingActor && (GWallReplacementFavorOwner || GWallReplacementFavorNonOwner))
		{
			const uint8 OwnerTeam = ExistingActor->TeamIndex;
			for (FContestedBuildingRequest& Request : Requests)
			{
				AFortPlayerController* RequesterPC = Request.Requester.Get();
				if (!RequesterPC)
				{
					continue;
				}

				const bool bIsOwner = RequesterPC->GetTeam() == OwnerTeam;
				if (GWallReplacementFavorOwner ? !bIsOwner : bIsOwner)
				{
					continue;
				}

				if (FortPlayerController::ServerCreateBuildingActorInternal(RequesterPC, Request.BuildingClassData, Request.BuildLoc, Request.BuildRot, Request.bMirrored))
				{
					bBuilt = true;
					break;
				}
			}
		}

		while (!bBuilt && Requests.Num() > 0)
		{
			const int32 WinnerIndex = UKismetMathLibrary::RandomIntegerInRange(0, Requests.Num() - 1);
			FContestedBuildingRequest Winner = Requests[WinnerIndex];
			Requests.Remove(WinnerIndex);

			AFortPlayerController* RequesterPC = Winner.Requester.Get();
			bBuilt = RequesterPC && FortPlayerController::ServerCreateBuildingActorInternal(RequesterPC, Winner.BuildingClassData, Winner.BuildLoc, Winner.BuildRot, Winner.bMirrored);
		}

		__this->ContestedBuildingRequests().RemoveAt(i);
	}
}