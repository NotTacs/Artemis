#include "pch.h"
#include "../../Public/UObject/Object.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

bool Object::NeedsLoadForClient(UObject* Template)
{
	bool (*NeedsLoadForServer)(UObject * Template) = decltype(NeedsLoadForServer)(Template->VTable[0x21]);

	/* WE HAVE TO FUCKING REDIRECT NEEDSLOADFORCLIENT TO NEEDSLOADFORSERVER BECASUE FN IS GAY*/
	return NeedsLoadForServer(Template);
}

void Object::LoadModule()
{
	for (int i = 0; i < UObject::GObjects->Num(); i++)
	{
		UObject* Gobject = UObject::GObjects->GetByIndex(i);
		if (Gobject)
		{
			FMemoryLibrary::VFTHook(Gobject->VTable, 0x20, NeedsLoadForClient);
		}
	}
}