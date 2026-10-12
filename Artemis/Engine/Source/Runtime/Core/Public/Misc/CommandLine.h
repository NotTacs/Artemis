#pragma once
#include "CoreMinimal.h"

class FCommandLine
{
private:
	class Originals
	{
	public:
		static inline const wchar_t* (*Get)();
	};
public:
	static const wchar_t* Get();
	static void WhitelistCommandLines();
	static void LoadModule();
};