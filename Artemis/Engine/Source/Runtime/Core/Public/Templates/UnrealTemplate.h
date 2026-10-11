#pragma once
#include "CoreMinimal.h"
#include "Identity.h"
#include "RemoveReference.h"
#include "UnrealTypeTraits.h"
#include "AreTypesEqual.h"

template <typename T> struct TRemovePointer { typedef T Type; };
template <typename T> struct TRemovePointer<T*> { typedef T Type; };

/**
 * MoveTemp will cast a reference to an rvalue reference.
 * This is UE's equivalent of std::move except that it will not compile when passed an rvalue or
 * const object, because we would prefer to be informed when MoveTemp will have no effect.
 */
template <typename T>
FORCEINLINE typename TRemoveReference<T>::Type&& MoveTemp(T&& Obj)
{
	typedef typename TRemoveReference<T>::Type CastType;

	// Validate that we're not being passed an rvalue or a const object - the former is redundant, the latter is almost certainly a mistake
	static_assert(TIsLValueReferenceType<T>::Value, "MoveTemp called on an rvalue");
	static_assert(!TAreTypesEqual<CastType&, const CastType&>::Value, "MoveTemp called on a const object");

	return (CastType&&)Obj;
}

template <typename T>
FORCEINLINE T ImplicitConv(typename TIdentity<T>::Type Obj)
{
    return Obj;
}