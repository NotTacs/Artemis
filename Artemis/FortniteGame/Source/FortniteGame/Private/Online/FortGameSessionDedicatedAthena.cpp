#include "pch.h"
#include "../../Public/Online/FortGameSessionDedicatedAthena.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void FortGameSessionDedicatedAthena::OnMatchmakingSessionAssignMatchRequestV2(AFortGameSessionDedicatedAthena* __this, void* MatchAssignment) /* SCUFFIUM*/
{
	AFortGameModeAthena* GameMode = Cast<AFortGameModeAthena>(UWorld::GetWorld()->AuthorityGameMode);
	static void (*FortGameMode__SetCurrentPlaylistName)(AFortGameMode * __this, FName) = decltype(FortGameMode__SetCurrentPlaylistName)(GameMode->VTable[0x12E]);
	FortGameMode__SetCurrentPlaylistName(GameMode, *reinterpret_cast<FName*>(__int64(MatchAssignment) + 0x28));
	Originals::OnMatchmakingSessionAssignMatchRequestV2(__this, MatchAssignment);
}
void FortGameSessionDedicatedAthena::LoadModule()
{
	FMemoryLibrary::DetourHook(0x65f6838, OnMatchmakingSessionAssignMatchRequestV2, Originals::OnMatchmakingSessionAssignMatchRequestV2);
}