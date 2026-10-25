#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Core/Public/Templates/UnrealTemplate.h"
#include "Engine/Source/Runtime/Core/Public/Templates/SharedPointer.h"
#include "Engine/Source/Runtime/Core/Public/Delegates/MulticastDelegateBase.h"

/** Helper macro that enables passing comma-separated arguments as a single macro parameter */
#define FUNC_CONCAT( ... ) __VA_ARGS__

/**
 * Declares a delegate that can only bind to one native function at a time
 */
#define FUNC_DECLARE_DELEGATE( DelegateName, ReturnType, ... ) \
	typedef TDelegate<ReturnType(__VA_ARGS__)> DelegateName;

 /** Declares a broadcast delegate that can bind to multiple native functions simultaneously */
#define FUNC_DECLARE_MULTICAST_DELEGATE( MulticastDelegateName, ReturnType, ... ) \
	typedef TMulticastDelegate<ReturnType(__VA_ARGS__)> MulticastDelegateName;

/**
 * Declares a multicast delegate that is meant to only be activated from OwningType
 *
 * @note: This behavior is not enforced and this type should be considered deprecated for new delegates, use normal multicast instead
 */
#define FUNC_DECLARE_EVENT( OwningType, EventName, ReturnType, ... ) \
	class EventName : public TMulticastDelegate<ReturnType(__VA_ARGS__)> \
	{ \
		friend class OwningType; \
	};

 /** Declares a derived event delegate that works the same as its parent type but is intended to be used by a different owning type */
#define DECLARE_DERIVED_EVENT( OwningType, BaseTypeEvent, EventName ) \
	class EventName : public BaseTypeEvent { friend class OwningType; };

#define FUNC_INCLUDING_INLINE_IMPL
#define __Delegate_h__

#include "Engine/Source/Runtime/Core/Public/Delegates/DelegateInstancesImpl.h"
#include "Engine/Source/Runtime/Core/Public/Delegates/DelegateSignatureImpl.inl"

#undef FUNC_INCLUDING_INLINE_IMPL

#include "Engine/Source/Runtime/Core/Public/Delegates/DelegateCombinations.h"

// Simple delegate used by various utilities such as timers
DECLARE_DELEGATE(FSimpleDelegate);
DECLARE_MULTICAST_DELEGATE(FSimpleMulticastDelegate);

static_assert(sizeof(FSimpleDelegate) == 0x10, "TDelegate must be 0x10 bytes like the engine's");
static_assert(sizeof(FSimpleMulticastDelegate) == 0x18, "TMulticastDelegate must be 0x18 bytes like the engine's");