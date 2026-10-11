#include "pch.h"
#include "../Classes/Engine/NetDriver.h"
#include "../Classes/Kismet/MemoryLibrary.h"

void NetDriver::TickFlush(UNetDriver* __this, float DeltaSeconds)
{
	if (__this->ReplicationDriver)
		__this->ReplicationDriver->ServerReplicateActors();
	return Originals::TickFlush(__this, DeltaSeconds);
}
void NetDriver::LoadModule()
{
	FMemoryLibrary::DetourHook(0xbc72c0, TickFlush, Originals::TickFlush);
}