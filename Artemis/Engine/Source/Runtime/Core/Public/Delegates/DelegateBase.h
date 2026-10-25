#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Core/Public/Delegates/IDelegateInstance.h"

template <typename UserPolicy>
class TMulticastDelegateBase;

class FDelegateBase
{
	template <typename>
	friend class TMulticastDelegateBase;

	template <typename>
	friend class TDelegateBase;

protected:
	/** Matches TAlignedBytes<16, 16> FAlignedInlineDelegateType */
	static constexpr int32 AlignedInlineDelegateSize = 16;

	explicit FDelegateBase()
		: DelegateAllocation(nullptr)
		, DelegateSize(0)
	{
	}

	~FDelegateBase()
	{
		Unbind();
	}

	/**
	 * Move constructor.
	 */
	FDelegateBase(FDelegateBase&& Other)
	{
		DelegateAllocation = Other.DelegateAllocation;
		DelegateSize = Other.DelegateSize;
		Other.DelegateAllocation = nullptr;
		Other.DelegateSize = 0;
	}

	/**
	 * Move assignment.
	 */
	FDelegateBase& operator=(FDelegateBase&& Other)
	{
		Unbind();
		DelegateAllocation = Other.DelegateAllocation;
		DelegateSize = Other.DelegateSize;
		Other.DelegateAllocation = nullptr;
		Other.DelegateSize = 0;
		return *this;
	}

	/**
	 * Unbinds this delegate
	 */
	FORCEINLINE void Unbind()
	{
		if (IDelegateInstance* Ptr = GetDelegateInstanceProtected())
		{
			Ptr->Destroy();
			ResizeAllocation(0);
			DelegateSize = 0;
		}
	}

	/**
	 * Gets the delegate instance.  Not intended for use by user code.
	 */
	FORCEINLINE IDelegateInstance* GetDelegateInstanceProtected() const
	{
		return DelegateSize ? (IDelegateInstance*)DelegateAllocation : nullptr;
	}

private:
	friend void* operator new(size_t Size, FDelegateBase& Base);

	/** FHeapAllocator::ResizeAllocation, routed through the engine's FMemory so GMalloc owns the block */
	void ResizeAllocation(int32 NumElements)
	{
		if (DelegateAllocation || NumElements)
		{
			DelegateAllocation = FMemory::Realloc(DelegateAllocation, uint64(NumElements) * AlignedInlineDelegateSize, AlignedInlineDelegateSize);
		}
	}

	void* Allocate(int32 Size)
	{
		if (IDelegateInstance* CurrentInstance = GetDelegateInstanceProtected())
		{
			CurrentInstance->Destroy();
		}

		int32 NewDelegateSize = (Size + AlignedInlineDelegateSize - 1) / AlignedInlineDelegateSize;
		if (DelegateSize != NewDelegateSize)
		{
			ResizeAllocation(NewDelegateSize);
			DelegateSize = NewDelegateSize;
		}

		return DelegateAllocation;
	}

private:
	void* DelegateAllocation;
	int32 DelegateSize;
};
static_assert(sizeof(FDelegateBase) == 0x10, "FDelegateBase must match the engine's invocation list element size");

inline void* operator new(size_t Size, FDelegateBase& Base)
{
	return Base.Allocate((int32)Size);
}

inline void operator delete(void*, FDelegateBase&)
{
}

struct FDefaultDelegateUserPolicy
{
	using FDelegateInstanceExtras = IDelegateInstance;
	using FDelegateExtras = FDelegateBase;
	using FMulticastDelegateExtras = TMulticastDelegateBase<FDefaultDelegateUserPolicy>;
};

template <typename UserPolicy>
class TDelegateBase : public UserPolicy::FDelegateExtras
{
	template <typename>
	friend class TMulticastDelegateBase;

	using Super = typename UserPolicy::FDelegateExtras;

public:
	/**
	 * If this is a UFunction or UObject delegate, return the UObject.
	 */
	FORCEINLINE class UObject* GetUObject() const
	{
		if (IDelegateInstance* Ptr = Super::GetDelegateInstanceProtected())
		{
			return Ptr->GetUObject();
		}

		return nullptr;
	}

	/**
	 * Checks to see if the user object bound to this delegate is still valid.
	 */
	FORCEINLINE bool IsBound() const
	{
		IDelegateInstance* Ptr = Super::GetDelegateInstanceProtected();

		return Ptr && Ptr->IsSafeToExecute();
	}

	/**
	 * Returns a pointer to an object bound to this delegate, intended for quick lookup in the timer manager,
	 */
	FORCEINLINE const void* GetObjectForTimerManager() const
	{
		IDelegateInstance* Ptr = Super::GetDelegateInstanceProtected();

		const void* Result = Ptr ? Ptr->GetObjectForTimerManager() : nullptr;
		return Result;
	}

	/**
	 * Returns the address of the method pointer which can be used to learn the address of the function that will be executed.
	 */
	uint64 GetBoundProgramCounterForTimerManager() const
	{
		if (IDelegateInstance* Ptr = Super::GetDelegateInstanceProtected())
		{
			return Ptr->GetBoundProgramCounterForTimerManager();
		}

		return 0;
	}

	/**
	 * Checks to see if this delegate is bound to the given user object.
	 */
	FORCEINLINE bool IsBoundToObject(void const* InUserObject) const
	{
		if (!InUserObject)
		{
			return false;
		}

		IDelegateInstance* Ptr = Super::GetDelegateInstanceProtected();

		return Ptr && Ptr->HasSameObject(InUserObject);
	}

	/**
	 * Gets a handle to the delegate.
	 */
	FORCEINLINE FDelegateHandle GetHandle() const
	{
		FDelegateHandle Result;
		if (IDelegateInstance* Ptr = Super::GetDelegateInstanceProtected())
		{
			Result = Ptr->GetHandle();
		}

		return Result;
	}
};