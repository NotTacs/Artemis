#pragma once
#include "CoreMinimal.h"

class FortControllerComponent_Aircraft
{
public:
	static void ServerAttemptAircraftJump_Implementation(UFortControllerComponent_Aircraft* __this, const FRotator& ClientRotation);
	static void LoadModule();
};