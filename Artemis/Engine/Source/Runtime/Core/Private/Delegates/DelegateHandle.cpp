#include "pch.h"
#include "../../Public/Delegates/IDelegateInstance.h"

namespace UE4Delegates_Private
{
	constexpr uintptr_t GNextID = 0xAE34278;
}

uint64 FDelegateHandle::GenerateNewID()
{
	volatile __int64* NextID = reinterpret_cast<volatile __int64*>(InSDKUtils::GetImageBase() + UE4Delegates_Private::GNextID);

	uint64 Result = _InterlockedIncrement64(NextID);

	if (Result == 0)
	{
		Result = _InterlockedIncrement64(NextID);
	}

	return Result;
}