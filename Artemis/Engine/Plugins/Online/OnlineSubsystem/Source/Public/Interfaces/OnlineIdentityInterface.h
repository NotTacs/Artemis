#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Core/Public/Templates/SharedPointer.h"

class IOnlineIdentity
{
public:
	TSharedPtr<FUniqueNetIdRepl>& GetUniquePlayerId(TSharedPtr<FUniqueNetIdRepl>* result, int32 LocalUserNum)
	{
		TSharedPtr<FUniqueNetIdRepl>& (*GetUniquePlayerIdOriginal)(IOnlineIdentity*, TSharedPtr<FUniqueNetIdRepl>*result, int32 LocalUserNum) = decltype(GetUniquePlayerIdOriginal)(VTable[0xF0]);
		return GetUniquePlayerIdOriginal(this, result, LocalUserNum);
	}

	void** VTable;
};

using IOnlineIdentityPtr = TSharedPtr<IOnlineIdentity>;

