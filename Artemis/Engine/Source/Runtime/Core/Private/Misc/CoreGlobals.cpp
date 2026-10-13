#include "pch.h"
#include "../../Public/CoreGlobals.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

DEFINE_LOG_CATEGORY(LogTemp);
DEFINE_LOG_CATEGORY(LogArtemis);

void CoreGlobals::LoadModule()
{
	FMemoryLibrary::Set<bool>(0xB30CF9F, false); //GIsClient
	FMemoryLibrary::Set<bool>(0xB30CF9F + 1, true); //GISServer
}