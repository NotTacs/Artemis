#pragma once
#include "CoreMinimal.h"

class FOnlineError
{
public:
	bool bSucceeded;
	FString ErrorRaw;
	FString ErrorCode;
	FText ErrorMessage;
	BYTE Result[1];
};