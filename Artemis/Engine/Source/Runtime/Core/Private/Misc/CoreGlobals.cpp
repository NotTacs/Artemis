#include "pch.h"
#include "../../Public/CoreGlobals.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

DEFINE_LOG_CATEGORY(LogTemp);
DEFINE_LOG_CATEGORY(LogArtemis);
DEFINE_LOG_CATEGORY(LogInventory);
DEFINE_LOG_CATEGORY(LogAthenaAIServicePlayerBots);
DEFINE_LOG_CATEGORY(LogFort);
DEFINE_LOG_CATEGORY(LogModuleManager);

void CoreGlobals::LoadModule()
{
	FMemoryLibrary::Set<bool>(0xB30CF9F, false); //GIsClient
	FMemoryLibrary::Set<bool>(0xB30CF9D, true); //GIsServer
}