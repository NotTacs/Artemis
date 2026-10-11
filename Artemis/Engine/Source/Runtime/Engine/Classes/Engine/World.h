#pragma once
#include "CoreMinimal.h"

class World
{
public:
	static bool Listen(UWorld* __this, FURL& InURL);
	static void LoadModule();
};