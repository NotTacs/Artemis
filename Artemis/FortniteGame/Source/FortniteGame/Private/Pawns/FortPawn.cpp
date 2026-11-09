#include "pch.h"
#include "../../Public/Pawns/FortPawn.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void FortPawn::MovingEmoteStopped(AFortPawn* __this)
{
	if (!__this)
		return;
	__this->bMovingEmote = false;
	__this->bMovingEmoteForwardOnly = false;
	__this->bMovingEmoteFollowingOnly = false;
	__this->bMovingEmoteSkipLandingFX = false;
	__this->EmoteWalkSpeed = 0.0f;
}

DEFINE_FUNCTION(FortPawn::execMovingEmoteStopped)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	FortPawn::MovingEmoteStopped(Cast<AFortPawn>(Context));
	P_NATIVE_END;
}

void FortPawn::LoadModule()
{
	FMemoryLibrary::ExecHook(AFortPawn::StaticClass()->FindFunctionByName(FName(L"MovingEmoteStopped", EFindName::FNAME_Add)), execMovingEmoteStopped);
}