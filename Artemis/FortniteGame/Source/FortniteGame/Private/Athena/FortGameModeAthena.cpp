#include "pch.h"
#include "../../Public/Athena/FortGameModeAthena.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

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
}