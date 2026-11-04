#pragma once
#include "CoreMinimal.h"
#include "UnrealTemplate.h"

/**
 * Invokes a callable with a set of arguments (trimmed from the engine's Templates/Invoke.h: only the functor /
 * function pointer form, which is all TDeleterHolder needs).
 */
template <typename FuncType, typename... ArgTypes>
FORCEINLINE auto Invoke(FuncType&& Func, ArgTypes&&... Args)
	-> decltype(Forward<FuncType>(Func)(Forward<ArgTypes>(Args)...))
{
	return Forward<FuncType>(Func)(Forward<ArgTypes>(Args)...);
}
