#pragma once
#include "CoreMinimal.h"
#include "../Serialization/JsonTypes.h"

class FJsonObject;

class FJsonValue
{
public:
	void* VTable;
	EJson Type;
};