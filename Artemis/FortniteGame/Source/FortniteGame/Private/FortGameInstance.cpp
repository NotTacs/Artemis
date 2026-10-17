#include "pch.h"
#include "../Public/FortGameInstance.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"
uint8 FortGameInstance::GetClientFeatureAvailability(void* __this, uint8 Feature)
{
	constexpr uint8 ClientFeature_DedicatedServer = 1;
	constexpr uint8 ClientFeature_Frontend = 6;
	constexpr uint8 FeatureAvailability_Ready = 8;

	if (Feature == ClientFeature_DedicatedServer || Feature == ClientFeature_Frontend)
		return FeatureAvailability_Ready;

	return Originals::GetClientFeatureAvailability(__this, Feature);
}

void FortGameInstance::LoadModule()
{
	FMemoryLibrary::DetourHook(0x14E3650, GetClientFeatureAvailability, Originals::GetClientFeatureAvailability);
	FMemoryLibrary::SetTrue(0x17dd630); //UGameInstance::IsDedicatedServerInstance
}
