#pragma once
#include "CoreMinimal.h"

class FortGameStateAthena
{
private:
	class Originals
	{
	public:
		static inline void (*PlacePlayersInAircraft)(AFortGameStateAthena* __this);
	};
public:
	static void PlacePlayersInAircraft(AFortGameStateAthena* __this);
	static void LoadModule();
};