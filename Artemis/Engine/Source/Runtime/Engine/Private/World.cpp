#include "pch.h"
#include "../Classes/Engine/World.h"
#include "../Classes/Kismet/MemoryLibrary.h"

bool World::Listen(UWorld* __this, FURL& InURL)
{
	printf(__FUNCTION__"\n");
	if (__this->NetDriver)
	{
		printf("???\n");
		UEngine::GetEngine()->BroadcastNetworkFailure(__this, __this->NetDriver, ENetworkFailure::NetDriverAlreadyExists);
		return false;
	}

	FName NetDriverDefinition = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");

	if (UEngine::GetEngine()->CreateNamedNetDriver(__this, NetDriverDefinition, NetDriverDefinition))
	{
		__this->NetDriver = UEngine::GetEngine()->FindNamedNetDriver(__this, NetDriverDefinition);

		if (__this->NetDriver == nullptr)
		{
			printf("Cant Create\n");
			UEngine::GetEngine()->BroadcastNetworkFailure(__this, NULL, ENetworkFailure::NetDriverCreateFailure);
			return false;
		}

		__this->NetDriver->World = __this;
		__this->NetDriver->NetDriverName = NetDriverDefinition;

		AFortWorldSettings* WorldSettings = Cast<AFortWorldSettings>(__this->K2_GetWorldSettings());
		const bool bReuseAddressAndPort = false;
		FString Error;
		if (!__this->NetDriver->InitListen(__this, InURL, bReuseAddressAndPort, Error))
		{
			printf("Failed to listen\n");
			UEngine::GetEngine()->BroadcastNetworkFailure(__this, __this->NetDriver, ENetworkFailure::NetDriverListenFailure, Error);
			//UEngine::GetEngine()->DestroyNamedNetDriver(__this, __this->NetDriver->NetDriverName);
			__this->NetDriver = nullptr;
			FLevelCollection* SourceCollection = __this->FindCollectionByType(ELevelCollectionType::DynamicSourceLevels);
			if (SourceCollection)
			{
				SourceCollection->SetNetDriver(nullptr);
			}
			FLevelCollection* StaticCollection = __this->FindCollectionByType(ELevelCollectionType::StaticLevels);
			if (StaticCollection)
			{
				StaticCollection->SetNetDriver(nullptr);
			}
			return false;
		}
		__this->NetDriver->SetWorld(__this);
		FLevelCollection* const SourceCollection = __this->FindCollectionByType(ELevelCollectionType::DynamicSourceLevels);
		if (SourceCollection)
		{
			SourceCollection->SetNetDriver(__this->NetDriver);
		}
		FLevelCollection* const StaticCollection = __this->FindCollectionByType(ELevelCollectionType::StaticLevels);
		if (StaticCollection)
		{
			StaticCollection->SetNetDriver(__this->NetDriver);
		}
	}

	static const bool bLanPlay = false;
	const bool bLanSpeed = bLanPlay;
	if (!bLanSpeed && (__this->NetDriver->MaxInternetClientRate < __this->NetDriver->MaxClientRate) && (__this->NetDriver->MaxInternetClientRate > 2500))
	{
		__this->NetDriver->MaxClientRate = __this->NetDriver->MaxInternetClientRate;
	}
}

void World::LoadModule()
{
	FMemoryLibrary::SetTrue(0xC9EEBC); //UWorld::GetNetMode
}