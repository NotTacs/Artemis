#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Json/Public/Dom/JsonObject.h"

struct FUniqueNetId : TSharedFromThis<FUniqueNetId>
{
public:
	void** VTable;
};

class FOnlineNotification
{
public:
	FString TypeStr;
	TSharedPtr<FJsonObject> Payload;
	TSharedPtr<FUniqueNetId> ToUserId;
	TSharedPtr<FUniqueNetId> FromUserId;
};