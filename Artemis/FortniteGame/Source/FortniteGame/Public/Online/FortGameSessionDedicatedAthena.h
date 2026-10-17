#pragma once
#include "CoreMinimal.h"

class FortGameSessionDedicatedAthena
{
private:
	class Originals
	{
	public:
		static inline void (*OnMatchmakingSessionAssignMatchRequestV2)(AFortGameSessionDedicatedAthena* __this, void* MatchAssignment);
	};
public:
	static void OnMatchmakingSessionAssignMatchRequestV2(AFortGameSessionDedicatedAthena* __this, void* MatchAssignment);
	static void LoadModule();
};