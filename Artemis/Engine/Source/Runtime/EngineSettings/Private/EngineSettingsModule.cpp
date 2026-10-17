#include "pch.h"
#include "../Public/EngineSettingsModule.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

FString GameMapsSettings::GetGameDefaultMap()
{
	UGameMapsSettings* ClassDefaultObject = UGameMapsSettings::GetDefaultObj();
	if (!ClassDefaultObject)
	{
		UGameMapsSettings::StaticClass()->CreateDefaultObject();
		ClassDefaultObject = (UGameMapsSettings*)UGameMapsSettings::StaticClass()->DefaultObject;
	}

	FString LeftSide;
	FString RightSide;
	UKismetStringLibrary::Split(UKismetStringLibrary::Conv_NameToString(ClassDefaultObject->ServerDefaultMap.AssetPathName), TEXT("."), &LeftSide, &RightSide, ESearchCase::CaseSensitive, ESearchDir::FromStart);
	return LeftSide;

}

FString GameMapsSettings::GetGlobalDefaultGameMode()
{
	UGameMapsSettings* ClassDefaultObject = UGameMapsSettings::GetDefaultObj();
	if (!ClassDefaultObject)
	{
		UGameMapsSettings::StaticClass()->CreateDefaultObject();
		ClassDefaultObject = (UGameMapsSettings*)UGameMapsSettings::StaticClass()->DefaultObject;
	}

	FString PackageName{};
	if (ClassDefaultObject->GlobalDefaultServerGameMode.SubPathString.Num() <= 1)
	{
		PackageName = UKismetStringLibrary::Conv_NameToString(ClassDefaultObject->GlobalDefaultServerGameMode.AssetPathName);
		return PackageName;
	}
	TArray<FString> PathParts;
	if (ClassDefaultObject->GlobalDefaultServerGameMode.AssetPathName.IsValid())
	{
		PathParts.Add(UKismetStringLibrary::Conv_NameToString(ClassDefaultObject->GlobalDefaultServerGameMode.AssetPathName));
	}
	PathParts.Add(ClassDefaultObject->GlobalDefaultServerGameMode.SubPathString);

	FString FullPathString = UKismetStringLibrary::JoinStringArray(PathParts, TEXT(":"));
	return FullPathString;
}

void GameMapsSettings::LoadModule()
{
	FMemoryLibrary::DetourHook(0x143C4BC, GetGameDefaultMap);
	FMemoryLibrary::DetourHook(0x7189044, GetGlobalDefaultGameMode);
}