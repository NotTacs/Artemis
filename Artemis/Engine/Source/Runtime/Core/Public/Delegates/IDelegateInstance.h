#pragma once
#include "CoreMinimal.h"

/**
 * Class representing a handle to a specific object/function pair bound to a delegate.
 */
class FDelegateHandle
{
public:
	enum EGenerateNewHandleType
	{
		GenerateNewHandle
	};

	/** Creates an initially unset handle */
	FDelegateHandle()
		: ID(0)
	{
	}

	/** Creates a handle pointing to a new instance */
	explicit FDelegateHandle(EGenerateNewHandleType)
		: ID(GenerateNewID())
	{
	}

	/** Returns true if this was ever bound to a delegate, but you need to check with the owning delegate to confirm it is still valid */
	bool IsValid() const
	{
		return ID != 0;
	}

	/** Clear handle to indicate it is no longer bound */
	void Reset()
	{
		ID = 0;
	}

private:
	friend bool operator==(const FDelegateHandle& Lhs, const FDelegateHandle& Rhs)
	{
		return Lhs.ID == Rhs.ID;
	}

	friend bool operator!=(const FDelegateHandle& Lhs, const FDelegateHandle& Rhs)
	{
		return Lhs.ID != Rhs.ID;
	}

	/**
	 * Generates a new ID for use the delegate handle.
	 * Uses the game's own counter so our handles never collide with ones the game hands out.
	 */
	static uint64 GenerateNewID();

	uint64 ID;
};

class IDelegateInstance
{
protected:
	IDelegateInstance() = default;
	IDelegateInstance(const IDelegateInstance&) = default;
	IDelegateInstance& operator=(const IDelegateInstance&) = default;

public:
	/** ~IDelegateInstance(), destroys the instance without freeing it (FDelegateBase owns the memory) */
	FORCEINLINE void Destroy()
	{
		InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<void* (*)(IDelegateInstance*, uint32)>(this, 0), this, 0);
	}

	/**
	 * Returns the UObject that this delegate instance is bound to.
	 *
	 * @return Pointer to the UObject, or nullptr if not bound to a UObject.
	 */
	FORCEINLINE UObject* GetUObject() const
	{
		return InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<UObject * (*)(const IDelegateInstance*)>(this, 1), this);
	}

	/**
	 * Returns a pointer to an object bound to this delegate instance, intended for quick lookup in the timer manager,
	 *
	 * @return A pointer to an object referenced by the delegate instance.
	 */
	FORCEINLINE const void* GetObjectForTimerManager() const
	{
		return InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<const void* (*)(const IDelegateInstance*)>(this, 2), this);
	}

	/**
	 * Returns the address of the method pointer which can be used to learn the address of the function that will be executed.
	 * Returns nullptr if this delegate type does not directly invoke a function pointer.
	 */
	FORCEINLINE uint64 GetBoundProgramCounterForTimerManager() const
	{
		return InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<uint64(*)(const IDelegateInstance*)>(this, 3), this);
	}

	/**
	 * Returns true if this delegate is bound to the specified UserObject,
	 */
	FORCEINLINE bool HasSameObject(const void* InUserObject) const
	{
		return InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<bool (*)(const IDelegateInstance*, const void*)>(this, 4), this, InUserObject);
	}

	/**
	 * Checks to see if the user object bound to this delegate can ever be valid again.
	 * used to compact multicast delegate arrays so they don't expand without limit.
	 */
	FORCEINLINE bool IsCompactable() const
	{
		return InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<bool (*)(const IDelegateInstance*)>(this, 5), this);
	}

	/**
	 * Checks to see if the user object bound to this delegate is still valid
	 */
	FORCEINLINE bool IsSafeToExecute() const
	{
		return InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<bool (*)(const IDelegateInstance*)>(this, 6), this);
	}

	/**
	 * Returns a handle for the delegate.
	 */
	FORCEINLINE FDelegateHandle GetHandle() const
	{
		// Member functions return class types through a hidden pointer that comes after this, so it's an explicit out param here
		FDelegateHandle Result;
		InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<FDelegateHandle * (*)(const IDelegateInstance*, FDelegateHandle*)>(this, 7), this, &Result);
		return Result;
	}

protected:
	void** VTable;
};
static_assert(sizeof(IDelegateInstance) == 0x8, "IDelegateInstance must only be a vtable pointer");