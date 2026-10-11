#pragma once
#include "framework.h"
#include "JsonValue.h"
#include "../../../Core/Public/Templates/SharedPointer.h"

class FJsonObject
{
public:

	TMap<FString, TSharedPtr<FJsonValue>> Values;
};