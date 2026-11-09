#pragma once
#include "CoreMinimal.h"

class FortPlayerControllerAthena
{
private:
	class Originals
	{
	public:
		static inline void (*OnPossess)(AFortPlayerControllerAthena* __this, AFortPlayerPawnAthena* NewPawn);
		static inline bool (*PostLogin)(AFortPlayerControllerAthena* __this);
	};
public:
	static void ServerAcknowledgePossession_Implementation(AFortPlayerControllerAthena* __this, APawn* P);
	static void RemoveAllInventoryItems(AFortPlayerControllerAthena* __this);
	static void RemoveAllInventoryItemsThenReset(AFortPlayerControllerAthena* __this);
	static void ServerExecuteInventoryItem_Implementation(AFortPlayerControllerAthena* __this, const FGuid& ItemGuid);
	static void OnPossess(AFortPlayerControllerAthena* __this, AFortPlayerPawnAthena* NewPawn);
	static void ReservePortal(AFortPlayerControllerAthena* __this);
	static bool PostLogin(AFortPlayerControllerAthena* __this);
	static void LoadModule();
};