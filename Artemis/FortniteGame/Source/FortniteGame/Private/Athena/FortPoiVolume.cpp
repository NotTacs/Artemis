#include "pch.h"
#include "../../Public/Athena/FortPoiVolume.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void FortPoiVolume::PostInitializeComponents(AFortPoiVolume* __this)
{
	const FFortChallengeMapPoiData* PoiData = nullptr;
	if (__this->LocationTags.GameplayTags.Num() > 0)
	{
		for (auto& Data : UFortQuestIndicatorData::Get()->ChallengeMapPoiData)
		{
			if (Data.LocationTag.TagName == __this->LocationTags.GameplayTags[0].TagName)
			{
				PoiData = &Data;
				break;
			}
		}
	}
	if (!PoiData || !__this->Brush || __this->Brush->Nodes.Num() == 0)
	{
		UE_LOG(LogArtemis, Log, TEXT("FortPoiVolume: skipping %s (%s)"), *__this->GetName(), *__this->LocationTags.ToFString());
		return Originals::PostInitializeComponents(__this);
	}

	const FBoxSphereBounds& Bounds = __this->Brush->Bounds;
	FVector Location = PoiData->WorldLocation;
	Location.X -= Bounds.Origin.X;
	Location.Y -= Bounds.Origin.Y;
	Location.Z -= Bounds.Origin.Z;

	UBrushComponent* BrushComp = NewObject<UBrushComponent>(__this);
	__this->BrushComponent = BrushComp;
	BrushComp->AlwaysLoadOnClient = false;
	BrushComp->AlwaysLoadOnServer = true;
	BrushComp->SetCollisionProfileName(FName(L"NoCollision", EFindName::FNAME_Find), true);
	BrushComp->SetGenerateOverlapEvents(false);
	BrushComp->Brush = __this->Brush;
	__this->SetRootComponent(BrushComp);
	BrushComp->K2_SetWorldLocationAndRotation(Location, {}, false, nullptr, true);
	BrushComp->BuildSimpleBrushCollision();

	BrushComp->RegisterComponent();
	__this->PoiCollisionComp->K2_AttachToComponent(BrushComp, FName(), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);

	Originals::PostInitializeComponents(__this);
}

void FortPoiVolume::LoadModule()
{
	FMemoryLibrary::DetourHook(0xb6a380, PostInitializeComponents, Originals::PostInitializeComponents);
	FMemoryLibrary::SetByte<uint8_t>(0x602D3D4, 0x75);
}