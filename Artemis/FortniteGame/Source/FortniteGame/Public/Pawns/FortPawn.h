#pragma once
#include "CoreMinimal.h"

class FortPawn
{
public:
	static void MovingEmoteStopped(AFortPawn* __this);
	static DEFINE_FUNCTION(execMovingEmoteStopped);
	static void LoadModule();
};