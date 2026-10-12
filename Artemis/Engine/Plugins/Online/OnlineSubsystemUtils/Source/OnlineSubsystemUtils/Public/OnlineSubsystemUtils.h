#pragma once
#include "CoreMinimal.h"
#include "../../../../OnlineSubsystem/Source/Public/OnlineSubsystem.h"


IOnlineIdentityPtr& GetIdentityInterface(IOnlineIdentityPtr* result, UWorld* World, const FName SubSystemName)
{
	static IOnlineIdentityPtr& (*GetIdentityInterfaceInternal)(IOnlineIdentityPtr * result, UWorld * World, const FName SubSystemName) = decltype(GetIdentityInterfaceInternal)(InSDKUtils::GetImageBase() + 0x505dedc);
	return GetIdentityInterfaceInternal(result, World, SubSystemName);
}