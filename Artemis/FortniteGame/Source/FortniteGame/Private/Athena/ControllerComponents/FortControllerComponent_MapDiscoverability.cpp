#include "pch.h"
#include "../../../Public/Athena/ControllerComponents/FortControllerComponent_MapDiscoverability.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

namespace Originals
{
	static void (*InitializeDiscoverability)(UFortControllerComponent_MapDiscoverability*);
}

bool FortControllerComponent_MapDiscoverability::ComponentHasAuthority(UFortControllerComponent_MapDiscoverability* __this)
{
	AActor* Owner = __this->GetOwner();
	return Owner && Owner->Role == ENetRole::ROLE_Authority;
}

void FortControllerComponent_MapDiscoverability::LoadModule()
{
	FMemoryLibrary::DetourHook(0x1BE1DB8, ComponentHasAuthority);
}
