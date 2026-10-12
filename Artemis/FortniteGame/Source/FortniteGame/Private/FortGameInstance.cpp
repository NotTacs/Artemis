#include "pch.h"
#include "../Public/FortGameInstance.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"
uint8 FortGameInstance::GetClientFeatureAvailability(void* __this, uint8 Feature)
{
	constexpr uint8 ClientFeature_DedicatedServer = 1;
	constexpr uint8 ClientFeature_Frontend = 6;
	constexpr uint8 FeatureAvailability_Ready = 8;

	if (Feature == ClientFeature_Frontend)
		printf("[FortGameInstance] Frontend availability queried from 0x%llX\n", uintptr_t(_ReturnAddress()) - InSDKUtils::GetImageBase());

	if (Feature == ClientFeature_DedicatedServer || Feature == ClientFeature_Frontend)
		return FeatureAvailability_Ready;

	return Originals::GetClientFeatureAvailability(__this, Feature);
}
void FortGameInstance::LoadServerGameFeatures()
{
	const uintptr_t Base = InSDKUtils::GetImageBase();
	auto GetGameFeaturesSubsystem = reinterpret_cast<uint8* (*)()>(Base + 0xC963A0);
	auto LoadBuiltInGameFeatures = reinterpret_cast<void (*)(uint8* Policy, bool bHotfixable)>(Base + 0x19898A4);
	auto ActivateHotfixableGameFeatures = reinterpret_cast<void (*)()>(Base + 0x19897A4);

	uint8* Subsystem = GetGameFeaturesSubsystem();
	uint8* Policy = Subsystem ? *reinterpret_cast<uint8**>(Subsystem + 0xE0) : nullptr;
	if (!Policy)
	{
		printf("[FortGameInstance] GameFeatures policy not created yet\n");
		return;
	}

	printf("[FortGameInstance] GameFeatures policy pending flags: non-hotfixable=%d hotfixable=%d\n", Policy[40], Policy[41]);

	if (Policy[40])
	{
		Policy[40] = 0;
		LoadBuiltInGameFeatures(Policy, false);
	}
	Policy[41] = 0;
	Policy[42] = 1;
	printf("[FortGameInstance] Loading hotfixable GameFeatures\n");
	LoadBuiltInGameFeatures(Policy, true);
	ActivateHotfixableGameFeatures();
}

struct Test
{
	uint8_t pad[0x198];
};

void FortGameInstance::LoadModule()
{
	//TMap<FName, FS12_CineTransitionData> Data;
	//auto& Pair = Data[0];
	//TPair<int, Test> *P;
	//ContainerImpl::SetElement<TPair<int, Test>>* Y;

	FMemoryLibrary::DetourHook(0x14E3650, GetClientFeatureAvailability, Originals::GetClientFeatureAvailability);
	FMemoryLibrary::SetTrue(0x17dd630); //UGameInstance::IsDedicatedServerInstance
}
