#pragma once
#include "CoreMinimal.h"

class OnlineBeaconClient
{
public:
	static bool InitClient(AOnlineBeaconClient* __this, FURL& URL);
	static void LoadModule();
};