#pragma once
#include "CoreMinimal.h"

class Object
{
public:
	static bool NeedsLoadForClient(UObject* Template);
	static void LoadModule();
};