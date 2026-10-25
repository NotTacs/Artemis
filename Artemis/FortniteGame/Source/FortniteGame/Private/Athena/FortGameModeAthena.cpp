#include "pch.h"
#include "../../Public/Athena/FortGameModeAthena.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"
#include "FortniteGame\Source\FortniteGame\Public\Items\FortInventoryOwnerInterface.h"

void FortGameModeAthena::FinishWorldInitialization(AFortGameModeAthena* __this, AFortWorldManager* WorldManager)
{
	printf(__FUNCTION__"\n");
	static void (*FortGameMode__FinishWorldInitialization)(AFortGameMode * __this, AFortWorldManager * WorldManager) = decltype(FortGameMode__FinishWorldInitialization)(AFortGameMode::GetDefaultObj()->VTable[0x15A]);
	FortGameMode__FinishWorldInitialization(__this, WorldManager);
	//FName DefaultPlaylistSolo = UKismetStringLibrary::Conv_StringToName(L"playlist_defaultsolo");
	//if (!__this->CurrentPlaylistName.IsValid())
	//{
	//	static void (*FortGameMode__SetCurrentPlaylistName)(AFortGameMode * __this, FName) = decltype(FortGameMode__SetCurrentPlaylistName)(__this->VTable[0x12E]);
	//	FortGameMode__SetCurrentPlaylistName(__this, DefaultPlaylistSolo);
	//}
	//TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	//OnGivenMatchAssignment(__this, true, JsonObject);

	printf("GameSession: %s\n", __this->GameSession->GetFullName().c_str());

	Originals::FinishWorldInitialization(__this, WorldManager);
}

void FortGameModeAthena::OnGivenMatchAssignment(AFortGameModeAthena* __this, bool bSuccess, TSharedPtr<FJsonObject>& MatchOptions)
{
	printf(__FUNCTION__"\n");
	static void (*FortGameMode__OnGivenMatchAssignment)(AFortGameModeAthena * __this, bool bSuccess, TSharedPtr<FJsonObject>& MatchOptions) = decltype(FortGameMode__OnGivenMatchAssignment)(AFortGameMode::GetDefaultObj()->VTable[0x184]);
	FortGameMode__OnGivenMatchAssignment(__this, bSuccess, MatchOptions);

	for (const auto& [Key, Value] : MatchOptions->Values)
	{
		if (!Value.IsValid())
		{
			continue;
		}

		switch (Value->Type)
		{
		case EJson::String:
			UE_LOG(LogTemp, Log, TEXT("Key: %s | Type: String | Value: %s"), *Key, *Value->AsString());
			break;

		case EJson::Number:
			UE_LOG(LogTemp, Log, TEXT("Key: %s | Type: Number | Value: %f"), *Key, Value->AsNumber());
			break;

		case EJson::Boolean:
			UE_LOG(LogTemp, Log, TEXT("Key: %s | Type: Boolean | Value: %s"), *Key, Value->AsBool() ? TEXT("True") : TEXT("False"));
			break;

		case EJson::Object:
			UE_LOG(LogTemp, Log, TEXT("Key: %s | Type: Object"), *Key);
			break;

		case EJson::Array:
			UE_LOG(LogTemp, Log, TEXT("Key: %s | Type: Array"), *Key);
			break;

		case EJson::Null:
		case EJson::None:
			UE_LOG(LogTemp, Log, TEXT("Key: %s | Type: Null/None"), *Key);
			break;
		}
	}

	AFortGameStateAthena* GameState = Cast<AFortGameStateAthena>(__this->GameState);
	UFortGameInstance* OwningGameInstance = Cast<UFortGameInstance>(UWorld::GetWorld()->OwningGameInstance);
	if (GameState)
	{
		UFortPlaylistAthena* Playlist = OwningGameInstance->PlaylistManager->GetAthenaPlaylist(__this->CurrentPlaylistName);
		GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
		GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
		GameState->CurrentPlaylistInfo.MarkArrayDirty();
		GameState->OnRep_CurrentPlaylistInfo();
		GameState->CurrentPlaylistId = __this->CurrentPlaylistId;
		GameState->OnRep_CurrentPlaylistId();

		__this->WarmupRequiredPlayerCount = 1;

		__this->AISettings = Playlist->AISettings;

		__this->AISettings->AIServices[1] = UAthenaAIServicePlayerBots::StaticClass();

		UCurveTable* AthenaGameDataTable = Playlist->GameData.LoadSynchronus();
		for (auto& [RowName, Curve] : AthenaGameDataTable->GetRowMap<FSimpleCurve>())
		{
			if (RowName.ToString() == "Default.SafeZone.Damage")
			{
				for (auto& Key : Curve->Keys)
				{
					if (Key.Time == 0) Key.Value = 0;
				}
			}
		}

		UE_LOG(LogFort, Log, TEXT("Patched %s"), *AthenaGameDataTable->GetName());
	}
}

bool FortGameModeAthena::ReadyToStartMatch_Implementation(AFortGameModeAthena* __this)
{
	static bool (*FortGameMode__ReadyToStartMatch_Implementation)(AFortGamePvPBase * __this) = decltype(FortGameMode__ReadyToStartMatch_Implementation)(AFortGamePvPBase::GetDefaultObj()->VTable[0x113]);

	if (__this->bWorldIsReady && __this->MatchState == *reinterpret_cast<FName*>(InSDKUtils::GetImageBase() + 0xB418188) 
		&& __this->CountReadyPlayers(false) >= __this->WarmupRequiredPlayerCount && FortGameMode__ReadyToStartMatch_Implementation(__this))
		return true;
	return false;
}

APawn* FortGameModeAthena::SpawnDefaultPawnFor_Implementation(AFortGameModeAthena* __this, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot)
{
	static APawn* (*FortGameMode__SpawnDefaultPawnFor)(AFortGameMode * __this, AFortPlayerControllerAthena * NewPlayer, AActor * StartSpot) = decltype(FortGameMode__SpawnDefaultPawnFor)(AFortGameMode::GetDefaultObj()->VTable[0xD8]);
	return FortGameMode__SpawnDefaultPawnFor(__this, NewPlayer, StartSpot);
}

void FortGameModeAthena::StartNewSafeZonePhase(AFortGameModeAthena* __this, int32 NewSafeZonePhase)
{
	printf(__FUNCTION__"\n");
	Originals::StartNewSafeZonePhase(__this, NewSafeZonePhase);
	AFortGameStateAthena* GameState = Cast<AFortGameStateAthena>(__this->GameState);
	AFortSafeZoneIndicator* SafeZoneIndicator = __this->SafeZoneIndicator;
	if (!GameState || !GameState->MapInfo || !SafeZoneIndicator || SafeZoneIndicator->bSafezoneEventDriven)
		return;
	const int32 Phase = __this->SafeZonePhase;
	float WaitTime = GameState->MapInfo->SafeZoneDefinition.WaitTimeCached.IsValidIndex(Phase) ? GameState->MapInfo->SafeZoneDefinition.WaitTimeCached[Phase] : 0.f;
	float ShrinkTime = GameState->MapInfo->SafeZoneDefinition.ShrinkTimeCached.IsValidIndex(Phase) ? GameState->MapInfo->SafeZoneDefinition.ShrinkTimeCached[Phase] : 0.f;

	SafeZoneIndicator->SafeZoneStartShrinkTime = GameState->GetServerWorldTimeSeconds() + WaitTime;
	SafeZoneIndicator->SafeZoneFinishShrinkTime = SafeZoneIndicator->SafeZoneStartShrinkTime + ShrinkTime;
}

void FortGameModeAthena::FinishRestartPlayer(AFortGameModeAthena* __this, AController* NewPlayer, const FRotator& StartRotation)
{
	printf(__FUNCTION__"\n");
	Originals::FinishRestartPlayer(__this, NewPlayer, StartRotation);
	if (NewPlayer->IsA(AFortAthenaAIBotController::StaticClass()))
		return;
	AFortPlayerStateAthena* FortPlayerState = Cast<AFortPlayerStateAthena>(NewPlayer->PlayerState);
	const bool bInitialSpawn = !FortPlayerState->DeathInfo.bInitialized;
	TArray<FItemAndCount> StartingItems = __this->GetStartingItems(bInitialSpawn, NewPlayer);
	IFortInventoryOwnerInterface* InventoryOwner = Cast<IFortInventoryOwnerInterface>(NewPlayer);
	for (auto& StartingItem : StartingItems)
	{
		if (StartingItem.Count > 0 && StartingItem.Item) {
			if (NewPlayer->IsA(AFortPlayerControllerAthena::StaticClass()) && StartingItem.Item->IsA(UFortSmartBuildingItemDefinition::StaticClass()))
				continue;
			FFortItemEntry ItemEntry = FFortItemEntry(StartingItem.Item, StartingItem.Count);
			FortInventoryOwnerInterface::GiveInventoryItem(InventoryOwner, ItemEntry);
		}
	}
	if (NewPlayer->IsA(AFortPlayerControllerAthena::StaticClass()))
	{
		FFortAthenaLoadout CosmeticLoadout = ((AFortPlayerControllerAthena*)NewPlayer)->CosmeticLoadoutPC;
		FFortItemEntry ItemEntry = FFortItemEntry(CosmeticLoadout.Pickaxe->WeaponDefinition, 1);
		FortInventoryOwnerInterface::GiveInventoryItem(InventoryOwner, ItemEntry);
	}

	static bool bFirst = false;
	if (!bFirst) /*TOOD: Move block to a better spot.*/
	{
		UAthenaAIServicePlayerBots* AIServicePlayerBots = UAthenaAIBlueprintLibrary::GetAIServicePlayerBots(UWorld::GetWorld());

		TArray<AFortPoiVolume*> Volumes;
		UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPoiVolume::StaticClass(), (TArray<AActor*>*) & Volumes);

		for (auto volume : Volumes)
		{
			FCachedPOIVolumeLocations Loc{};
			Loc.bEQSQueryPending = false;
			printf("navMeshLoc: %s\n", volume->K2_GetActorLocation().ToString().c_str());
			Loc.NavMeshLocations.AddImproper(volume->K2_GetActorLocation());
			Loc.POIVolume = volume;
			AIServicePlayerBots->CachedValidPOIVolumeLocations.AddImproper(Loc);
		}

		for (auto& idk : AIServicePlayerBots->OnGroundTagQueryPOIList)
		{
			for (auto volume : Volumes)
			{
				if (volume->ContainsLocationTag(idk.POIFilterQuery.TagDictionary[0]))
				{

					idk.ValidPOIVolumeList.Add(volume);
				}
			}
		}

		for (auto& idk : AIServicePlayerBots->BattleBusTagQueryPOIList)
		{
			for (auto volume : Volumes)
			{
				if (volume->ContainsLocationTag(idk.POIFilterQuery.TagDictionary[0]))
				{

					idk.ValidPOIVolumeList.Add(volume);
				}
			}
		}

		for (auto& idk : AIServicePlayerBots->SecondaryBattleBusTagQueryPOIList)
		{
			for (auto volume : Volumes)
			{
				if (volume->ContainsLocationTag(idk.POIFilterQuery.TagDictionary[0]))
				{

					idk.ValidPOIVolumeList.Add(volume);
				}
			}
		}

		Volumes.Free();

		TArray<ABuildingFoundation*> Foundations;
		UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), ABuildingFoundation::StaticClass(), (TArray<AActor*>*) & Foundations);

		for (auto foundation : Foundations)
		{
			AIServicePlayerBots->CachedBuildingFoundations.Add(foundation);
		}

		Foundations.Free();

		AIServicePlayerBots->DebugMinimapData.DebugMinimapIconBrush.ResourceObject = UObject::StaticLoadObject<UTexture2D>("/Game/Athena/HUD/Spectator/DeimosRiftMapIcon.DeimosRiftMapIcon");
		AIServicePlayerBots->DebugMinimapData.DebugMinimapIconBrush.ImageSize = FVector2D(36, 36);
		AIServicePlayerBots->DebugMinimapData.DebugMinimapIconScale = FVector2D(0.5, 0.5);

		bFirst = true;
	}
}

TSubclassOf<AGameSession>* FortGameModeAthena::GetGameSessionClass(AFortGameModeAthena* __this, TSubclassOf<AGameSession>* OutClass)
{
	*OutClass = AFortGameSessionDedicatedAthena::StaticClass();
	return OutClass;
}

void FortGameModeAthena::LoadModule()
{
	FMemoryLibrary::DetourHook(0x5f90568, FinishWorldInitialization, Originals::FinishWorldInitialization);
	FMemoryLibrary::VFTHook(AFortGameModeAthena::GetDefaultObj()->VTable, 0x184, OnGivenMatchAssignment);
	FMemoryLibrary::VFTHook(AFortGameModeAthena::GetDefaultObj()->VTable, 0x113, ReadyToStartMatch_Implementation);
	FMemoryLibrary::VFTHook(AFortGameModeAthena::GetDefaultObj()->VTable, 0xD8, SpawnDefaultPawnFor_Implementation);
	FMemoryLibrary::VFTHook(AFortGameModeAthena::GetDefaultObj()->VTable, 0xE3, GetGameSessionClass);
	FMemoryLibrary::DetourHook(0x5FA67BC, StartNewSafeZonePhase, Originals::StartNewSafeZonePhase);
	FMemoryLibrary::DetourHook(0x5F8F684, FinishRestartPlayer, Originals::FinishRestartPlayer);
}