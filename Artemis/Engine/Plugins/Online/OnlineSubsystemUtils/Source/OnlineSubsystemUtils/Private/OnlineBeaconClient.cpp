#include "pch.h"
#include "../Public/OnlineBeaconClient.h"
#include "../Public/OnlineSubsystemUtils.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

struct FDelegateBase
{
	void* Data;
	int DelegateSize;
};

bool OnlineBeaconClient::InitClient(AOnlineBeaconClient* __this, FURL& URL)
{
	bool bSuccess = false;
	if (URL.Valid)
	{
		if (__this->InitBase() && __this->NetDriver)
		{
			FString Error{};
			if (__this->NetDriver->InitConnect(__this, URL, Error))
			{
				if (__this->NetDriver->ServerConnection && UWorld::GetWorld())
				{
					__this->BeaconConnection = __this->NetDriver->ServerConnection;

					if (true /*IsRunningDedicatedServer*/)
					{
						IOnlineIdentityPtr IdentityPtr;
						GetIdentityInterface(&IdentityPtr, UWorld::GetWorld(), UKismetStringLibrary::Conv_StringToName(L"None"));
						if (IdentityPtr.IsValid())
						{
							TSharedPtr<FUniqueNetIdRepl> PlayerId;
							__this->BeaconConnection->PlayerId = *IdentityPtr->GetUniquePlayerId(&PlayerId, 0);
						}
					}

					if (*reinterpret_cast<int32*>(__int64(__this) + 0x2A8) > 1)
					{
						static void (*SetEncryptionData)(void*, void*) = decltype(SetEncryptionData)(InSDKUtils::GetImageBase() + 0x79137BC);
						SetEncryptionData(__this->BeaconConnection, *reinterpret_cast<void**>(__int64(__this) + 0x280));
					}

					__this->ConnectionState = EBeaconConnectionState::Pending;

					bool bSentHandshake = false;

					if (*reinterpret_cast<void**>(__int64(__this) + 0x140))
					{
						void* Inst = FMemory::Realloc(nullptr, 0x30, 0);
						FDelegateBase Base{};
						Base.Data = reinterpret_cast<decltype(Base.Data)>(Inst);
						Base.DelegateSize = 3;
						static void (*sub_4DF7C04)(...) = decltype(sub_4DF7C04)(InSDKUtils::GetImageBase() + 0x4DF7C04);
						static void (*sub_515F89C)(...) = decltype(sub_515F89C)(InSDKUtils::GetImageBase() + 0x515F89C);
						sub_4DF7C04(Inst, __this, reinterpret_cast<void*>(&sub_515F89C));
						static void (*sub_76CEE00)(...) = decltype(sub_76CEE00)(InSDKUtils::GetImageBase() + 0x76CEE00);
						sub_76CEE00(*reinterpret_cast<void**>(__int64(__this) + 0x140), Inst);
						bSentHandshake = true;
					}

					if (__this->NetDriver)
					{
						
						__this->NetDriver->SetWorld(UWorld::GetWorld());
						FMemoryLibrary::Set<FNetworkNotify*>(__this->NetDriver, 0x208, __this);
						__this->NetDriver->InitialConnectTimeout = __this->BeaconConnectionInitialTimeout;
						__this->NetDriver->ConnectionTimeout = __this->BeaconConnectionTimeout;

						if (!bSentHandshake)
						{
							static void (*SendInitialJoin)(void*) = decltype(SendInitialJoin)(InSDKUtils::GetImageBase() + 0x515F89C);
							SendInitialJoin(__this);
						}

						bSuccess = true;
					}
					else
					{
						if (__this->ConnectionState == EBeaconConnectionState::Pending)
						{
							__this->ConnectionState = EBeaconConnectionState::Invalid;
							void (*OnFailure)(void*) = decltype(OnFailure)(__this->VTable[0xD8]);
							OnFailure(__this);
						}
					}
				}
				else
				{
					__this->ConnectionState = EBeaconConnectionState::Invalid;
					void (*OnFailure)(void*) = decltype(OnFailure)(__this->VTable[0xD8]);
					OnFailure(__this);
				}
			}
			else
			{
				__this->ConnectionState = EBeaconConnectionState::Invalid;
				void (*OnFailure)(void*) = decltype(OnFailure)(__this->VTable[0xD8]);
				OnFailure(__this);
			}
		}
	}

	return bSuccess;
}
void OnlineBeaconClient::LoadModule()
{
	FMemoryLibrary::DetourHook(0x514fbd0, OnlineBeaconClient::InitClient);
}