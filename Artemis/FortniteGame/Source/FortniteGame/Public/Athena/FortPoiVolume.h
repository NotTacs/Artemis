#pragma once
#include "CoreMinimal.h"

class FortPoiVolume
{
	class Originals
	{
	public:
		static inline void (*PostInitializeComponents)(AFortPoiVolume* __this);
	};
public:
	static void PostInitializeComponents(AFortPoiVolume* __this);
	static void LoadModule();
};