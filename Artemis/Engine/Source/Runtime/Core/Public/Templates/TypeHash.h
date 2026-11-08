// Copyright Epic Games, Inc. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"

/**
 * Combines two hash values to get a third.
 * Note - this function is not commutative.
 *
 * WARNING!  This function is subject to change and should only be used for creating
 *           combined hash values which don't leave the running process,
 *           e.g. GetTypeHash() overloads.
 */
inline uint32 HashCombineFast(uint32 A, uint32 B)
{
	// Currently call HashCombine because it is exists, but only as a placeholder until it
	// can be replaced with something better
	return HashCombine(A, B);
}
