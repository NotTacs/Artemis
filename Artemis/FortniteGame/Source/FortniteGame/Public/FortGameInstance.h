#pragma once
#include "CoreMinimal.h"

class FortGameInstance
{
private:
	class Originals
	{
	public:
		static inline uint8 (*GetClientFeatureAvailability)(void* __this, uint8 Feature);
	};
public:
	static uint8 GetClientFeatureAvailability(void* __this, uint8 Feature);
	static void LoadModule();
};
