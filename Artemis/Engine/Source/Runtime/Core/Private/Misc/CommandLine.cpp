#include "pch.h"
#include "../../Public/Misc/CommandLine.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

const wchar_t* FCommandLine::Get()
{
	static auto cmdLine = std::wstring(Originals::Get()) + L" -server -McpRegion=NAE -EnableMMS -OverrideMap=/Game/Athena/Artemis/Maps/Artemis_Terrain -ServesPlaylists=Playlist_DefaultSolo -Playlist=Playlist_DefaultSolo";
	return cmdLine.c_str();
}

void FCommandLine::LoadModule()
{
	FMemoryLibrary::DetourHook(0xb71d9c, FCommandLine::Get, Originals::Get);
}