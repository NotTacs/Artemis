#pragma once
#include "CoreMinimal.h"

#include <atomic>

namespace BackendVars::PlayerBots
{
	inline std::atomic<int32> CachedHumanPlayersCount{ 0 };

	inline std::atomic<bool> bHumanPlayersCountReceived{ false };
}
