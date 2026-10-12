#pragma once
#include "CoreMinimal.h"

class Engine
{
private:
	class Originals
	{
	public:
		static inline void (*Init)(UEngine* __this, void* InEngineLoop);
		static inline void (*RequestExitWithStatus)(bool Force, uint8 ReturnCode);
	};
public:
	static void RequestExitWithStatus(bool Force, uint8 ReturnCode);
	static void QueueClientOnlyShutdown(UObject* Context, UWorld* World);
	static bool LoadMap(__int64 a1, unsigned __int8* a2, __int64 a3, wchar_t* a4, __int64 a5);
	static void Init(UEngine* __this, void* InEngineLoop);
	static void InitEngine();
};