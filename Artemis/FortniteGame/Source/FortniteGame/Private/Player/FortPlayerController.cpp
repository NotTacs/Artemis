#include "pch.h"
#include "../../Public/Player/FortPlayerController.h"
#include "../../Public/FortGameMode.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void FortPlayerController::ServerCreateBuildingActor_Implementation(AFortPlayerController* __this, FCreateBuildingActorData& CreateBuildingData)
{
	UWorld* World = __this->GetWorld();
	if (!World)
	{
		return;
	}

	AFortGameStateZone* GameStateZone = Cast<AFortGameStateZone>(World->GameState);
	if (!GameStateZone)
	{
		return;
	}

	if (!GameStateZone->AllPlayerBuildableClasses.IsValidIndex(CreateBuildingData.BuildingClassHandle))
	{
		return;
	}

	FBuildingClassData BuildingClassData{};
	BuildingClassData.PreviousBuildingLevel = -1;
	BuildingClassData.UpgradeLevel = 0;
	BuildingClassData.BuildingClass =GameStateZone->AllPlayerBuildableClasses[CreateBuildingData.BuildingClassHandle];
	FVector BuildLoc = CreateBuildingData.BuildLoc;
	FRotator BuildRot = CreateBuildingData.BuildRot;
	const bool bMirrored = CreateBuildingData.bMirrored;

	if (!FixUpCreateBuildingClassData(__this,BuildingClassData))
	{
		return;
	}

	TArray<ABuildingActor*> ExistingBuildings;
	EFortBuildPreviewMarkerOptionalAdjustment MarkerAdjustment;
	const EFortStructuralGridQueryResults PreGridResult = __this->CanPlaceBuildableClassInStructuralGrid(BuildingClassData.BuildingClass, BuildLoc, BuildRot, bMirrored, ExistingBuildings, MarkerAdjustment);
	if (PreGridResult == EFortStructuralGridQueryResults::CanAdd)
	{
		for (ABuildingActor* ExistingBuilding : ExistingBuildings)
		{
			ABuildingSMActor* ExistingSMActor = Cast<ABuildingSMActor>(ExistingBuilding);
			if (!ExistingSMActor || !ExistingSMActor->bDestroyed)
			{
				continue;
			}

			if (UGameplayStatics::GetTimeSeconds(World) - ExistingSMActor->DestroyedTime > GWallReplacementCooldown)
			{
				continue;
			}

			if (AFortGameMode* FortGameMode = Cast<AFortGameMode>(World->AuthorityGameMode))
			{
				FortGameMode::QueueRequestForNewContestedBuildingPiece(FortGameMode, ExistingSMActor, __this, BuildingClassData, BuildLoc, BuildRot, bMirrored);
				return;
			}
		}
	}

	ServerCreateBuildingActorInternal(__this, BuildingClassData, BuildLoc, BuildRot, bMirrored, CreateBuildingData.SyncKey, false, false, false);
}

bool FortPlayerController::FixUpCreateBuildingClassData(AFortPlayerController* __this, FBuildingClassData& BuildingClassData)
{
	if (!BuildingClassData.BuildingClass)
		return false;
	if (!BuildingClassData.BuildingClass->IsSubclassOf(ABuildingActor::StaticClass()))
		return false;
	if (!BuildingClassData.BuildingClass->DefaultObject)
	{
		BuildingClassData.BuildingClass->CreateDefaultObject();
	}
	ABuildingSMActor* BuildingCDO = Cast<ABuildingSMActor>(BuildingClassData.BuildingClass->DefaultObject);
	if (!BuildingCDO)
		return false;
	if (BuildingCDO->GetCurrentBuildingLevel() <= 0)
	{
		return true;
	}
	UWorld* CurrentWorld = __this->GetWorld();
	AFortGameStateZone* GameStateZone = Cast<AFortGameStateZone>(CurrentWorld->GameState);

	FPlayerBuildableClassFilter ClassFilter;
	ClassFilter.ResourceType = BuildingCDO->ResourceType;
	ClassFilter.BuildingType = BuildingCDO->BuildingType;
	ClassFilter.Level = 0;
	ClassFilter.EditModeMetadata = BuildingCDO->EditModePatternData;

	TArray<TSubclassOf<ABuildingSMActor>> MatchingPlayerBuildableClasses;
	GameStateZone->GetPlayerBuildableClasses(MatchingPlayerBuildableClasses, ClassFilter);

	if (MatchingPlayerBuildableClasses.Num() == 0)
	{
		ClassFilter.EditModeMetadata = nullptr;
		GameStateZone->GetPlayerBuildableClasses(MatchingPlayerBuildableClasses, ClassFilter);
	}

	TSubclassOf<ABuildingSMActor> FixedUpClass = MatchingPlayerBuildableClasses.Num() > 0 ? MatchingPlayerBuildableClasses[0] : nullptr;
	if (!FixedUpClass || !FixedUpClass->IsSubclassOf(ABuildingActor::StaticClass()))
	{
		return false;
	}

	BuildingClassData.PreviousBuildingLevel = -1;
	BuildingClassData.UpgradeLevel = 0;
	BuildingClassData.BuildingClass = FixedUpClass;
	return true;
}


ABuildingSMActor* FortPlayerController::ServerCreateBuildingActorInternal(AFortPlayerController* __this, FBuildingClassData& BuildingClassData, FVector& BuildLoc, FRotator& BuildRot, bool bMirrored,
	float SyncKey, bool bIgnoreNativeActionCheck, bool bIgnoreCanAffordCheck, bool bIgnoreBuildingValidityCheck)
{
	TSubclassOf<ABuildingSMActor> ClassToBuild = BuildingClassData.BuildingClass.Get();
	if (!ClassToBuild)
	{
		return nullptr;
	}

	if (!bIgnoreNativeActionCheck && !__this->CanPerformNativeAction(UFortGlobals::GameplayTags()->NativeActionBuild()))
	{
		return nullptr;
	}

	const bool bIsRestricted = !bIgnoreBuildingValidityCheck && __this->IsBuildingRestricted(BuildingClassData, BuildLoc, BuildRot);

	bool bCanAfford = false;
	EFortStructuralGridQueryResults StructuralGridResult = EFortStructuralGridQueryResults::CanAdd;
	TArray<ABuildingActor*> ExistingBuildings;

	if (!bIsRestricted)
	{
		bCanAfford = bIgnoreCanAffordCheck || __this->CanAffordToPlaceBuildableClass(BuildingClassData);
		const bool bHasRequiredLevel = __this->HasRequiredBuildingLevel(BuildingClassData);

		const FVector OriginalBuildLoc = BuildLoc;
		const FRotator OriginalBuildRot = BuildRot;

		EFortBuildPreviewMarkerOptionalAdjustment MarkerOptionalAdjustment;
		StructuralGridResult = __this->CanPlaceBuildableClassInStructuralGrid(ClassToBuild, BuildLoc, BuildRot, bMirrored, ExistingBuildings, MarkerOptionalAdjustment);
		if (bIgnoreBuildingValidityCheck)
		{
			StructuralGridResult = EFortStructuralGridQueryResults::CanAdd;
		}

		if (BuildLoc != OriginalBuildLoc || BuildRot != OriginalBuildRot)
		{
			UE_LOG(LogFort, Log, TEXT("ServerCreateBuildingActor_Implementation() yielded different build location after calling CanPlaceBuildableClassInStructuralGrid() - %s %s  to  %s %s"), OriginalBuildLoc.ToWString().c_str(), OriginalBuildRot.ToWString().c_str(), BuildLoc.ToWString().c_str(), BuildRot.ToWString().c_str());
		}

		if (bCanAfford && bHasRequiredLevel && StructuralGridResult == EFortStructuralGridQueryResults::CanAdd)
		{
			for (ABuildingActor* ExistingBuilding : ExistingBuildings)
			{
				if (ExistingBuilding && ExistingBuilding->bDestroyOnPlayerBuildingPlacement)
				{
					ExistingBuilding->Die(UFortGlobals::GameplayTags()->DeathCauseReplaced(), nullptr, nullptr, nullptr);
				}
			}

			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ABuildingSMActor* NewBuilding = __this->GetWorld()->SpawnActor<ABuildingSMActor>(ClassToBuild, BuildLoc, BuildRot, SpawnInfo);
			if (!NewBuilding)
			{
				return nullptr;
			}

			NewBuilding->CurrentBuildingLevel = BuildingClassData.UpgradeLevel;
			NewBuilding->SetMirrored(bMirrored);

			NewBuilding->InitializeKismetSpawnedBuildingActor(NewBuilding, __this, true, nullptr);

			const int32 PlacementCost = bIgnoreCanAffordCheck ? 0 : __this->PayBuildableClassPlacementCost(BuildingClassData);
			__this->DoCreateBuildingAnalytics(NewBuilding, PlacementCost);

			if (AFortGameMode* FortGameMode = Cast<AFortGameMode>(__this->GetWorld()->AuthorityGameMode))
			{
				FortGameMode->ScoreBuildingConstruction(__this, NewBuilding);
			}

			if (__this->MyFortPawn)
			{
				UAISense_Hearing::ReportNoiseEvent(__this->MyFortPawn, NewBuilding->K2_GetActorLocation(), 1.f, __this->MyFortPawn, 1000.f, FName(L"None", EFindName::FNAME_Add));
				__this->MyFortPawn->PlayBlueprintPlaceAnimation();

				if (AFortWeap_BuildingTool* BuildingTool = Cast<AFortWeap_BuildingTool>(__this->MyFortPawn->CurrentWeapon))
				{
					BuildingTool->PlayWeaponFireFX(false);
				}
			}

			return NewBuilding;
		}
	}

	FText FailureMessage;
	if (bIsRestricted)
	{
		FailureMessage = NSLOCTEXT("Fort.Player.Defaults", "BuildingIsRestrictedText", "Building in this location is restricted!");
	}
	else
	{
		FailureMessage = __this->GetErrorMessageFromStructuralQuery(StructuralGridResult);
	}

	
	if (UKismetTextLibrary::TextIsEmpty(FailureMessage))
	{
		if (!ClassToBuild->DefaultObject)
		{
			ClassToBuild->CreateDefaultObject();
		}
		const ABuildingSMActor* BuildingCDO = Cast<ABuildingSMActor>(ClassToBuild->DefaultObject);
		const FText ResourceName = UFortGameData::Get()->GetResourceDisplayName(BuildingCDO->ResourceType);
		FailureMessage = FText::Format(NSLOCTEXT("Fort.Player.Defaults", "UnableToAfford", "Not enough {0} to build! Change building material or gather more!"), ResourceName);
	}

	if (!UKismetTextLibrary::TextIsEmpty(FailureMessage))
	{
		__this->ClientSendMessage(FailureMessage, nullptr);
	}

	__this->ClientTriggerUIFeedbackEvent(!bIsRestricted && !bCanAfford ? FName(TEXT("BuildPreviewUnableToAfford"), EFindName::FNAME_Add) : FName(TEXT("BuildPreviewUnableToPlace"), EFindName::FNAME_Add));
	return nullptr;
}

void FortPlayerController::ServerBeginEditingBuildingActor_Implementation(AFortPlayerController* __this, ABuildingSMActor* BuildingActorToEdit)
{
	if (!BuildingActorToEdit || !__this->MyFortPawn || BuildingActorToEdit->bDestroyed)
	{
		return;
	}

	AFortPlayerStateZone* PlayerState = Cast<AFortPlayerStateZone>(__this->PlayerState);
	if (!PlayerState)
	{
		return;
	}

	if (BuildingActorToEdit->EditingPlayer && BuildingActorToEdit->EditingPlayer != PlayerState)
	{
		return;
	}

	BuildingActorToEdit->SetEditingPlayer(PlayerState);

	IFortInventoryOwnerInterface* InventoryOwner = Cast<IFortInventoryOwnerInterface>(__this);
	UFortGameData* GameData = UFortGameData::Get();
	if (!InventoryOwner || !GameData)
	{
		return;
	}

	FItemVariantHandle EditToolHandle{};
	EditToolHandle.Item = GameData->EditToolItem.LoadSynchronus();
	if (!EditToolHandle.Item)
	{
		return;
	}

	UFortWorldItem* EditToolItem = Cast<UFortWorldItem>(InventoryOwner->FindExistingItemForDefinition(EditToolHandle, false));
	UFortWorldItemDefinition* EditToolDefinition = EditToolItem ? Cast<UFortWorldItemDefinition>(EditToolItem->ItemEntry.ItemDefinition) : nullptr;
	if (!EditToolDefinition || !EditToolDefinition->ServerExecute(EditToolItem, __this))
	{
		return;
	}

	if (AFortWeap_EditingTool* EditTool = Cast<AFortWeap_EditingTool>(__this->MyFortPawn->CurrentWeapon))
	{
		EditTool->EditActor = BuildingActorToEdit;
		EditTool->ForceNetUpdate();
	}
}

void FortPlayerController::ServerEditBuildingActor_Implementation(AFortPlayerController* __this, ABuildingSMActor* BuildingActorToEdit, UClass* NewBuildingClass, uint8 RotationIterations, bool bMirrored)
{
	if (!BuildingActorToEdit || !NewBuildingClass || !BuildingActorToEdit->GetWorld() || BuildingActorToEdit->bDestroyed)
	{
		return;
	}

	if (BuildingActorToEdit->EditingPlayer != __this->PlayerState)
	{
		return;
	}

	BuildingActorToEdit->SetEditingPlayer(nullptr);

	ABuildingSMActor* (*ReplaceBuildingActor)(ABuildingSMActor* __this, EBuildingReplacementType ReplacementType, UClass* ClassToReplaceWith, int32 BuildingLevel, int32 RotationIterations, bool bMirrored, AFortPlayerController* Instigator) = decltype(ReplaceBuildingActor)(BuildingActorToEdit->VTable[0x18F]);
	ReplaceBuildingActor(BuildingActorToEdit, EBuildingReplacementType::BRT_Edited, NewBuildingClass, BuildingActorToEdit->GetCurrentBuildingLevel(), RotationIterations, bMirrored, __this);

	__this->DoEditBuildingActorAnalytics(BuildingActorToEdit);

	if (AFortGameMode* FortGameMode = Cast<AFortGameMode>(__this->GetWorld()->AuthorityGameMode))
	{
		void (*ScoreBuildingEdit)(AFortGameMode* __this, AController* EditingController, ABuildingSMActor* EditedBuilding) = decltype(ScoreBuildingEdit)(FortGameMode->VTable[0x154]);
		ScoreBuildingEdit(FortGameMode, __this, BuildingActorToEdit);
	}
}

void FortPlayerController::ServerEndEditingBuildingActor_Implementation(AFortPlayerController* __this, ABuildingSMActor* BuildingActorToStopEditing)
{
	if (!BuildingActorToStopEditing || !__this->MyFortPawn || BuildingActorToStopEditing->bDestroyed)
	{
		return;
	}

	if (BuildingActorToStopEditing->EditingPlayer != __this->PlayerState)
	{
		return;
	}

	BuildingActorToStopEditing->SetEditingPlayer(nullptr);

	if (AFortWeap_EditingTool* EditTool = Cast<AFortWeap_EditingTool>(__this->MyFortPawn->CurrentWeapon))
	{
		EditTool->EditActor = nullptr;
		EditTool->ForceNetUpdate();
	}
}

void FortPlayerController::ServerPlayEmoteItem_Implementation(AFortPlayerController* __this, const UFortMontageItemDefinitionBase* EmoteAsset, float EmoteRandomNumber)
{
	UE_LOG(LogFort, Log, L"ServerPlayEmoteItem_Implementation called.");
	if (!EmoteAsset || !__this->MyFortPawn)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = __this->MyFortPawn->AbilitySystemComponent;
	if (!AbilitySystemComponent || !__this->CanPerformNativeAction(EmoteAsset->GetAssociatedAbilityTag()))
	{
		return;
	}

	FGameplayTagContainer FailureTags;
	if (!EmoteAsset->CanActivateMontageItem(__this, &FailureTags))
	{
		return;
	}

	FFortAssets::GetAsset<UAnimMontage>(EmoteAsset->Animation, true);

	__this->MyFortPawn->EmoteRandomNum = EmoteRandomNumber;

	UE_LOG(LogFort, Log, TEXT("EmoteGameplayAbility: %s"), *UGameDataCosmetics::Get()->EmoteGameplayAbility.ObjectID.AssetPathName.ToFString());
	TSubclassOf<UClass> EmoteAbilityClass = FFortAssets::GetSubclassOf(UGameDataCosmetics::Get()->EmoteGameplayAbility);
	if (EmoteAsset->IsA<UAthenaSprayItemDefinition>())
	{
		if (TSubclassOf<UClass> SprayAbility = FFortAssets::GetSubclassOf(UGameDataCosmetics::Get()->SprayGameplayAbility))
		{
			EmoteAbilityClass = SprayAbility;
		}
	}
	else if (const UAthenaToyItemDefinition* ToyItemDefinition = Cast<UAthenaToyItemDefinition>(EmoteAsset))
	{
		if (TSubclassOf<UClass> ToySpawnAbility = FFortAssets::GetSubclassOf(ToyItemDefinition->ToySpawnAbility))
		{
			EmoteAbilityClass = ToySpawnAbility;
		}
	}
	else if (const UAthenaDanceItemDefinition* DanceItemDefinition = Cast<UAthenaDanceItemDefinition>(EmoteAsset))
	{
		if (TSubclassOf<UClass> CustomDanceAbility = FFortAssets::GetSubclassOf(DanceItemDefinition->CustomDanceAbility))
		{
			EmoteAbilityClass = CustomDanceAbility;
		}
	}

	if (!EmoteAbilityClass)
	{
		UE_LOG(LogFort, Log, L"Failed to find EmoteAbilityClass.");
		return;
	}

	FGameplayAbilitySpec Spec(EmoteAbilityClass->GetDefaultObject<UFortGameplayAbility>(), 1, INDEX_NONE, const_cast<UFortMontageItemDefinitionBase*>(EmoteAsset));
	AbilitySystemComponent->GiveAbilityAndActivateOnce(Spec);
}

void FortPlayerController::LoadModule()
{
	FMemoryLibrary::VFTHook(AFortPlayerController::GetDefaultObj()->VTable, 0x24D, ServerCreateBuildingActor_Implementation);
	FMemoryLibrary::VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x24D, ServerCreateBuildingActor_Implementation);

	FMemoryLibrary::VFTHook(AFortPlayerController::GetDefaultObj()->VTable, 0x254, ServerBeginEditingBuildingActor_Implementation);
	FMemoryLibrary::VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x254, ServerBeginEditingBuildingActor_Implementation);
	FMemoryLibrary::VFTHook(AFortPlayerController::GetDefaultObj()->VTable, 0x24F, ServerEditBuildingActor_Implementation);
	FMemoryLibrary::VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x24F, ServerEditBuildingActor_Implementation);
	FMemoryLibrary::VFTHook(AFortPlayerController::GetDefaultObj()->VTable, 0x252, ServerEndEditingBuildingActor_Implementation);
	FMemoryLibrary::VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x252, ServerEndEditingBuildingActor_Implementation);

	FMemoryLibrary::VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x1E8, ServerPlayEmoteItem_Implementation);
	FMemoryLibrary::VFTHook(AFortPlayerController::GetDefaultObj()->VTable, 0x1E8, ServerPlayEmoteItem_Implementation);
}