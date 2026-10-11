#pragma once
#include "framework.h"

class NetDriver
{
private:
	class Originals
	{
	public:
		static inline void (*TickFlush)(UNetDriver* __this, float DeltaSeconds);
	};
public:
	static void TickFlush(UNetDriver* __this, float DeltaSeconds);
	static void LoadModule();
};