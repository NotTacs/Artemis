#include "pch.h"
#include "../../../../Public/Athena/AI/Systems/AthenaAIServicePlayerBots.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

int32 PlayerBotsConsoleVariables::BotMaxSpawnAmount = *reinterpret_cast<int32*>(InSDKUtils::GetImageBase() + 0xB1BE974);

int32 AthenaAIServicePlayerBots::GetPlaylistMaxPlayerCount(UAthenaAIServicePlayerBots* __this)
{
	if (!__this->CachedGameState)
	{
		UE_LOG(LogAthenaAIServicePlayerBots, Error, TEXT("UAthenaAIServicePlayerBots::GetPlaylistMaxPlayerCount: No game state cached, failed to get max player count"));
		return 0;
	}
	UFortPlaylistAthena* CurrentPlaylistData = __this->CachedGameState->CurrentPlaylistInfo.BasePlaylist;
	if (!CurrentPlaylistData)
	{
		UE_LOG(LogAthenaAIServicePlayerBots, Error, TEXT("UAthenaAIServicePlayerBots::GetPlaylistMaxPlayerCount: No playlist data recieved from game state, failed to get max player count"));
		return 0;
	}
	if (CurrentPlaylistData->bOverrideMaxPlayers && CurrentPlaylistData->MaxHumanAndBotParticipants > 0)
	{
		return CurrentPlaylistData->MaxHumanAndBotParticipants;
	}
	else
	{
		return CurrentPlaylistData->MaxPlayers;
	}
}

void AthenaAIServicePlayerBots::CalculateSpawnAmountAndSkill(UAthenaAIServicePlayerBots* __this)
{
	if (!__this->bSpawningAmountComputed)
	{
		int32 MMRBracketLow = __this->CachedMMRSpawningInfo.MMRBracketLow;
		if (MMRBracketLow > -99999
			&& MMRBracketLow < __this->CachedMMRSpawningInfo.MMRBracketHigh
			&& __this->CachedMMRSpawningInfo.SpawningInfos.Num() > 0
			&& __this->CachedMMRSpawningInfo.TotalSpawningInfoWeight > 0)
		{
			if (__this->CachedAmountOfBotsToSpawn() == -1)
			{
				int32 OverridenMaxSpawnCount = __this->OverridenMaxSpawnAmount();
				if (OverridenMaxSpawnCount == -1)
				{
					if (__this->CachedGameMode)
					{
						int32 NumBotsToSpawn = -1; /*HMU IF YOU CAN FIND AFortGameModeAthena::NumBotsToSpawn, but I'm pretty sure this is always gonna be 0*/
						int32 MaxPlayerCount = AthenaAIServicePlayerBots::GetPlaylistMaxPlayerCount(__this);
						if (NumBotsToSpawn < 0 || NumBotsToSpawn >= MaxPlayerCount)
						{
							NumBotsToSpawn = 0;
							if (int32 CalculatedBotSpawnAmount = MaxPlayerCount - __this->CachedHumanPlayersCount())
								NumBotsToSpawn = CalculatedBotSpawnAmount;
						}

						OverridenMaxSpawnCount = NumBotsToSpawn;

						if (PlayerBotsConsoleVariables::BotMaxSpawnAmount > 0 && OverridenMaxSpawnCount > PlayerBotsConsoleVariables::BotMaxSpawnAmount)
						{
							OverridenMaxSpawnCount = PlayerBotsConsoleVariables::BotMaxSpawnAmount;
						}
					}
				}
				__this->CachedAmountOfBotsToSpawn() = OverridenMaxSpawnCount;
			}

			__this->CachedMMRSpawningInfo.CachedTotalAmountToSpawnComputed = 0;
			for (FMMRSpawningInfo& SpawningInfo : __this->CachedMMRSpawningInfo.SpawningInfos)
			{
				const float Weight = (__this->CachedMMRSpawningInfo.TotalSpawningInfoWeight > 0) ?
					SpawningInfo.BotSpawningDataInfoWeight / static_cast<float>(__this->CachedMMRSpawningInfo.TotalSpawningInfoWeight) : 0.0f;
				const int32 AmountToSpawn = static_cast<int32>(static_cast<float>(__this->CachedAmountOfBotsToSpawn()) * Weight);
				SpawningInfo.NumBotsToSpawn = AmountToSpawn;

				/*TODO: Add ensure*/
				// ensure(AmountToSpawn >= 0)
				__this->CachedMMRSpawningInfo.CachedTotalAmountToSpawnComputed += AmountToSpawn;
			}

			if (__this->CachedMMRSpawningInfo.CachedTotalAmountToSpawnComputed < __this->CachedAmountOfBotsToSpawn())
			{
				for (FMMRSpawningInfo& SpawningInfo : __this->CachedMMRSpawningInfo.SpawningInfos)
				{
					SpawningInfo.NumBotsToSpawn++;

					if (++__this->CachedMMRSpawningInfo.CachedTotalAmountToSpawnComputed >= __this->CachedAmountOfBotsToSpawn())
					{
						break;
					}
				}
			}

			if (__this->CachedMMRSpawningInfo.CachedTotalAmountToSpawnComputed != __this->CachedAmountOfBotsToSpawn())
			{
				__this->CachedMMRSpawningInfo.CachedTotalAmountToSpawnComputed = 0;
				UE_LOG(LogAthenaAIServicePlayerBots, Error, TEXT("Was not able to compute the right number of player bots to spawn matching the proper MMR requested."));
			}

			__this->bSpawningAmountComputed = true;
		}
		else
		{
			UE_LOG(LogAthenaAIServicePlayerBots, Error,
				TEXT("Phoebe:UpdateSpawningInfos: Can't update spawning infos as MMR bracket is not valid!"));
		}
	}
}

void AthenaAIServicePlayerBots::UseFailSafeMMRInfo(UAthenaAIServicePlayerBots* __this, float SkillOverride)
{
	__this->CachedMMRSpawningInfo.MMRBracketLow = -1000;
	__this->CachedMMRSpawningInfo.MMRBracketHigh = 4000;
	__this->CachedMMRSpawningInfo.TotalSpawningInfoWeight = 100;
	__this->CachedMMRSpawningInfo.SpawningInfos.Free();

	FMMRSpawningInfo SpawningItem{};
	SpawningItem.BotSpawningDataInfoWeight = 100.f;
	SpawningItem.BotSpawningDataInfoTargetELO = (SkillOverride > -1.0f) ? SkillOverride : 1100.0f;
	SpawningItem.NumBotsToSpawn = 0;
	SpawningItem.AISpawnerData = __this->DefaultBotAISpawnerData.Get();
	__this->CachedMMRSpawningInfo.SpawningInfos.Add(SpawningItem);
	AthenaAIServicePlayerBots::CalculateSpawnAmountAndSkill(__this);
}

void AthenaAIServicePlayerBots::InitializeMMRInfos(UAthenaAIServicePlayerBots* __this)
{
	__this->GamePhaseToStartSpawning = EAthenaGamePhase::Warmup;
	if (__this->GetWorld())
	{
		AFortGameStateAthena* GameState = Cast<AFortGameStateAthena>(__this->GetWorld()->GameState);
		if (GameState)
		{
			int32 AverageMMR = 2200; /*TODO: HOOK GetAvgPlayerMMR*/
			const UFortPlaylistAthena* Playlist = GameState->CurrentPlaylistInfo.BasePlaylist;
			const bool bPlaylistDoesNotSupportSBMM = (AverageMMR == 0) && Playlist && !Playlist->bUsePlayerRating;

			if (bPlaylistDoesNotSupportSBMM)
			{
				UE_LOG(LogAthenaAIServicePlayerBots, Warning, TEXT("Phoebe:InitializeMMRInfos: fallback to failsafe MMR Spawning info, reason: %s, MMR: %d"),
					bPlaylistDoesNotSupportSBMM ? TEXT("Playlist does not support SBMM") : TEXT("MMR is Invalid"), AverageMMR);

				UseFailSafeMMRInfo(__this, -1.0f);
				return;
			}

			__this->MMRSpawnTable.ObjectID.AssetPathName = UKismetStringLibrary::Conv_StringToName(L"/Game/Athena/AI/Phoebe/PhoebeSpawningTables/PhoebeMMRSpawning.PhoebeMMRSpawning");
			UDataTable* MMRSpawnSpecTable = __this->StaticLoadObject<UDataTable>(UKismetStringLibrary::Conv_NameToString(__this->MMRSpawnTable.ObjectID.AssetPathName).ToString());
			if (!MMRSpawnSpecTable)
			{
				UE_LOG(LogAthenaAIServicePlayerBots, Error,
					TEXT("Phoebe:InitializeMMRInfos: Invalid Game average MMR: %d!, fallback to failsafe MMR spawning info"),
					AverageMMR);

				UseFailSafeMMRInfo(__this, -1.0f);
				return;
			}

			MMRSpawnSpecTable->AddToRoot();

			const FMMRSpawningBracketBotDataTableRow* FoundRow = nullptr;
			for (const TPair<FName, uint8*>& RowPair : MMRSpawnSpecTable->RowMap)
			{
				const FMMRSpawningBracketBotDataTableRow* Row = reinterpret_cast<const FMMRSpawningBracketBotDataTableRow*>(RowPair.Value());

				const bool bValidBracket = Row->MMRBracketLow > -99999
					&& Row->MMRBracketLow < Row->MMRBracketHigh;

				if (bValidBracket && AverageMMR >= Row->MMRBracketLow && AverageMMR <= Row->MMRBracketHigh)
				{
					FoundRow = Row;
					break;
				}
			}

			if (!FoundRow)
			{
				UE_LOG(LogAthenaAIServicePlayerBots, Error,
					TEXT("Phoebe:InitializeMMRInfos: Invalid Average MMR (can't retrieve Table Entry) Bracket: %d, %d !, fallback to failsafe MMR spawning info"),
					__this->CachedMMRSpawningInfo.MMRBracketLow, __this->CachedMMRSpawningInfo.MMRBracketHigh);

				UseFailSafeMMRInfo(__this, -1.0f);
				return;
			}

			UE_LOG(LogFort, Display, TEXT("Phoebe:InitializeMMRInfos: Average MMR used : %d!"), AverageMMR);

			__this->CachedMMRSpawningInfo.MMRBracketLow = FoundRow->MMRBracketLow;
			__this->CachedMMRSpawningInfo.MMRBracketHigh = FoundRow->MMRBracketHigh;

			if (__this->CachedMMRSpawningInfo.MMRBracketLow <= -99999
				|| __this->CachedMMRSpawningInfo.MMRBracketLow >= __this->CachedMMRSpawningInfo.MMRBracketHigh
				|| FoundRow->PhaseSpawningItems.Num() == 0)
			{
				UE_LOG(LogAthenaAIServicePlayerBots, Error,
					TEXT("Phoebe:InitializeMMRInfos: Invalid Average MMR Bracket: %d, %d !, fallback to failsafe MMR spawning info"),
					__this->CachedMMRSpawningInfo.MMRBracketLow, __this->CachedMMRSpawningInfo.MMRBracketHigh);

				UseFailSafeMMRInfo(__this, -1.0f);
				return;
			}

			UE_LOG(LogFort, Display, TEXT("Phoebe:InitializeMMRInfos: MMR Bracket used: %d, %d !"),
				__this->CachedMMRSpawningInfo.MMRBracketLow, __this->CachedMMRSpawningInfo.MMRBracketHigh);

			__this->CachedMMRSpawningInfo.SpawningInfos.Free();
			__this->CachedMMRSpawningInfo.TotalSpawningInfoWeight = 0;

			const FMMRPhaseSpawningDataTableInfo& FirstPhase = FoundRow->PhaseSpawningItems[0];

			for (FMMRSpawningDataTableInfo& SpawningItem : FirstPhase.SpawningItems)
			{
				FMMRSpawningInfo SpawnInfo{};
				SpawnInfo.AISpawnerData = SpawningItem.AISpawnerData;
				SpawningItem.AISpawnerData.Get()->AddToRoot();
				SpawnInfo.BotSpawningDataInfoTargetELO = SpawningItem.Skill;
				SpawnInfo.BotSpawningDataInfoWeight = SpawningItem.Weight;

				__this->CachedMMRSpawningInfo.SpawningInfos.Add(SpawnInfo);

				__this->CachedMMRSpawningInfo.TotalSpawningInfoWeight = static_cast<int32>(static_cast<float>(__this->CachedMMRSpawningInfo.TotalSpawningInfoWeight) + SpawningItem.Weight);
			}

			if (__this->CachedMMRSpawningInfo.MMRBracketLow <= -99999
				|| __this->CachedMMRSpawningInfo.MMRBracketLow >= __this->CachedMMRSpawningInfo.MMRBracketHigh
				|| __this->CachedMMRSpawningInfo.SpawningInfos.Num() <= 0)
			{
				UE_LOG(LogAthenaAIServicePlayerBots, Error,
					TEXT("Phoebe:InitializeMMRInfos: Invalid ELO Spawn infos for MMR bracket %d, %d !, fallback to failsafe MMR spawning info"),
					__this->CachedMMRSpawningInfo.MMRBracketLow, __this->CachedMMRSpawningInfo.MMRBracketHigh);

				UseFailSafeMMRInfo(__this, -1.0f);
				return;
			}

			CalculateSpawnAmountAndSkill(__this);
		}
	}
}

void AthenaAIServicePlayerBots::LoadModule()
{
	FMemoryLibrary::Rel32Swap_Lea(0x5EE0504, InitializeMMRInfos);
}