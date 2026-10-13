#include "pch.h"
#include "../../Public/Components/FortControllerComponent_Aircraft.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void FortControllerComponent_Aircraft::ServerAttemptAircraftJump_Implementation(UFortControllerComponent_Aircraft* __this, const FRotator& ClientRotation)
{
	__this->bIsAttemptingAircraftJump() = true;
	AFortPlayerControllerAthena* Owner = Cast<AFortPlayerControllerAthena>(__this->GetOwner());

	if (Owner->IsInAircraft())
	{
		Owner->SetControlRotation(ClientRotation);
		Owner->AircraftJumpRotation() = ClientRotation;
		void (*AFortPlayerControllerZone__ServerRestartPlayer)(AFortPlayerControllerZone*) = decltype(AFortPlayerControllerZone__ServerRestartPlayer)(AFortPlayerControllerZone::GetDefaultObj()->VTable[0x11A]);
		AFortPlayerControllerZone__ServerRestartPlayer(Owner);
	}

	__this->bIsAttemptingAircraftJump() = false;
}

void FortControllerComponent_Aircraft::LoadModule()
{
	FMemoryLibrary::VFTHook(UFortControllerComponent_Aircraft::GetDefaultObj()->VTable, 0x9E, ServerAttemptAircraftJump_Implementation);
}