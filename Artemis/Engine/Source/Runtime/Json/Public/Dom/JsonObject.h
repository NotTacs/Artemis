#pragma once
#include "CoreMinimal.h"
#include "JsonValue.h"
#include "../../../Core/Public/Templates/SharedPointer.h"

class FJsonObject
{
public:

	TMap<FString, TSharedPtr<FJsonValue>> Values;
};