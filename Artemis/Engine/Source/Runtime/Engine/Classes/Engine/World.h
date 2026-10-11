#pragma once
#include "framework.h"

class World
{
public:
	static bool Listen(UWorld* __this, FURL& InURL);
	static void LoadModule();
};