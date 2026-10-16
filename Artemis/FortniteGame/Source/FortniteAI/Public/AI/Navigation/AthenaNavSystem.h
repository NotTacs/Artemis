#pragma once
#include "CoreMinimal.h"

class AthenaNavSystemConfig
{
private:
	class Originals
	{
	public:
		static inline void (*CreateAndConfigureNavSystem)(UAthenaNavSystemConfig* __this, UWorld* InWorld);
	};
public:
	static void CreateAndConfigureNavSystem(UAthenaNavSystemConfig* __this, UWorld* InWorld);
	static void LoadModule();
};