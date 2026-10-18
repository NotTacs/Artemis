#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Core/Public/Delegates/IDelegateInstance.h"
#include "Engine/Source/Runtime/Core/Public/Delegates/DelegateBase.h"

/**
 * TArray<TDelegateBase<UserPolicy>, FHeapAllocator> with the engine's layout ({ Data, ArrayNum, ArrayMax }).
 * Engine-owned multicast delegates grow/shrink this same buffer, so it always goes through GMalloc.
 */
template<typename ElementType>
class TInvocationList
{
public:
	TInvocationList()
		: Data(nullptr)
		, ArrayNum(0)
		, ArrayMax(0)
	{
	}

	~TInvocationList()
	{
		for (int32 Index = 0; Index < ArrayNum; Index++)
		{
			Data[Index].~ElementType();
		}
		ArrayNum = 0;
		ResizeTo(0);
	}

	FORCEINLINE int32 Num() const { return ArrayNum; }
	FORCEINLINE ElementType& operator[](int32 Index) { return Data[Index]; }
	FORCEINLINE const ElementType& operator[](int32 Index) const { return Data[Index]; }

	FORCEINLINE ElementType* begin() { return Data; }
	FORCEINLINE ElementType* end() { return Data + ArrayNum; }
	FORCEINLINE const ElementType* begin() const { return Data; }
	FORCEINLINE const ElementType* end() const { return Data + ArrayNum; }

	void Add(ElementType&& Item)
	{
		if (ArrayNum + 1 > ArrayMax)
		{
			// FMemory::QuantizeSize isn't worth porting, any growth is fine for the engine
			ResizeTo(ArrayMax < 4 ? 4 : ArrayMax * 2);
		}
		new (&Data[ArrayNum]) ElementType(static_cast<ElementType&&>(Item));
		ArrayNum++;
	}

	/** Delegates are bitwise relocatable, same as in UE */
	void RemoveAtSwap(int32 Index)
	{
		Data[Index].~ElementType();
		const int32 Last = ArrayNum - 1;
		if (Index != Last)
		{
			memcpy((void*)&Data[Index], (void*)&Data[Last], sizeof(ElementType));
		}
		ArrayNum--;
	}

	void Shrink()
	{
		if (ArrayMax != ArrayNum)
		{
			ResizeTo(ArrayNum);
		}
	}

private:
	void ResizeTo(int32 NewMax)
	{
		if (Data || NewMax)
		{
			Data = (ElementType*)FMemory::Realloc(Data, uint64(NewMax) * sizeof(ElementType), 0);
		}
		ArrayMax = NewMax;
	}

	ElementType* Data;
	int32 ArrayNum;
	int32 ArrayMax;
};

/**
 * Abstract base class for multicast delegates.
 * Layout verified against TMulticastDelegate::Broadcast in 19.10 (sub_14405B4):
 *   +0x00 InvocationList.Data, +0x08 Num, +0x0C Max, +0x10 CompactionThreshold, +0x14 InvocationListLockCount
 */
template<typename UserPolicy>
class TMulticastDelegateBase
{
protected:
	using InvocationListType = TInvocationList<TDelegateBase<UserPolicy>>;

public:
	/** Removes all functions from this delegate's invocation list. */
	void Clear()
	{
		for (TDelegateBase<UserPolicy>& DelegateBaseRef : InvocationList)
		{
			DelegateBaseRef.Unbind();
		}

		CompactInvocationList(false);
	}

	/**
	 * Checks to see if any functions are bound to this multi-cast delegate.
	 */
	inline bool IsBound() const
	{
		for (const TDelegateBase<UserPolicy>& DelegateBaseRef : InvocationList)
		{
			if (DelegateBaseRef.GetDelegateInstanceProtected())
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * Checks to see if any functions are bound to the given user object.
	 */
	inline bool IsBoundToObject(void const* InUserObject) const
	{
		for (const TDelegateBase<UserPolicy>& DelegateBaseRef : InvocationList)
		{
			IDelegateInstance* DelegateInstance = DelegateBaseRef.GetDelegateInstanceProtected();
			if ((DelegateInstance != nullptr) && DelegateInstance->HasSameObject(InUserObject))
			{
				return true;
			}
		}

		return false;
	}

	/**
	 * Removes all functions from this multi-cast delegate's invocation list that are bound to the specified UserObject.
	 * Note that the order of the delegates may not be preserved!
	 */
	int32 RemoveAll(const void* InUserObject)
	{
		int32 Result = 0;
		if (InvocationListLockCount > 0)
		{
			for (TDelegateBase<UserPolicy>& DelegateBaseRef : InvocationList)
			{
				IDelegateInstance* DelegateInstance = DelegateBaseRef.GetDelegateInstanceProtected();
				if ((DelegateInstance != nullptr) && DelegateInstance->HasSameObject(InUserObject))
				{
					// Manually unbind the delegate here so the compaction will find and remove it.
					DelegateBaseRef.Unbind();
					++Result;
				}
			}

			// can't compact at the moment, but set out threshold to zero so the next add will do it
			if (Result > 0)
			{
				CompactionThreshold = 0;
			}
		}
		else
		{
			// compact us while shuffling in later delegates to fill holes
			for (int32 InvocationListIndex = 0; InvocationListIndex < InvocationList.Num();)
			{
				TDelegateBase<UserPolicy>& DelegateBaseRef = InvocationList[InvocationListIndex];

				IDelegateInstance* DelegateInstance = DelegateBaseRef.GetDelegateInstanceProtected();
				if (DelegateInstance == nullptr
					|| DelegateInstance->HasSameObject(InUserObject)
					|| DelegateInstance->IsCompactable())
				{
					InvocationList.RemoveAtSwap(InvocationListIndex);
					++Result;
				}
				else
				{
					InvocationListIndex++;
				}
			}

			CompactionThreshold = (2 * InvocationList.Num()) > 2 ? (2 * InvocationList.Num()) : 2;

			InvocationList.Shrink();
		}

		return Result;
	}

protected:
	/** Hidden default constructor. */
	inline TMulticastDelegateBase()
		: CompactionThreshold(2)
		, InvocationListLockCount(0)
	{
	}

protected:
	/**
	 * Adds the given delegate instance to the invocation list.
	 */
	inline FDelegateHandle AddDelegateInstance(TDelegateBase<UserPolicy>&& NewDelegateBaseRef)
	{
		// compact but obey threshold of when this will trigger
		CompactInvocationList(true);
		FDelegateHandle Result = NewDelegateBaseRef.GetHandle();
		InvocationList.Add(MoveTemp(NewDelegateBaseRef));
		return Result;
	}

	/**
	 * Removes a function from this multi-cast delegate's invocation list (performance is O(N)).
	 */
	bool RemoveDelegateInstance(FDelegateHandle Handle)
	{
		for (int32 InvocationListIndex = 0; InvocationListIndex < InvocationList.Num(); ++InvocationListIndex)
		{
			TDelegateBase<UserPolicy>& DelegateBase = InvocationList[InvocationListIndex];

			IDelegateInstance* DelegateInstance = DelegateBase.GetDelegateInstanceProtected();
			if (DelegateInstance && DelegateInstance->GetHandle() == Handle)
			{
				DelegateBase.Unbind();
				CompactInvocationList();
				return true; // each delegate binding has a unique handle, so once we find it, we can stop
			}
		}

		return false;
	}

	/**
	 * Removes any expired or deleted functions from the invocation list.
	 */
	void CompactInvocationList(bool CheckThreshold = false)
	{
		// if locked and no object, just return
		if (InvocationListLockCount > 0)
		{
			return;
		}

		// if checking threshold, obey but decay. This is to ensure that even infrequently called delegates will
		// eventually compact during an Add()
		if (CheckThreshold && --CompactionThreshold > InvocationList.Num())
		{
			return;
		}

		int32 OldNumItems = InvocationList.Num();

		// Find anything null or compactable and remove it
		for (int32 InvocationListIndex = 0; InvocationListIndex < InvocationList.Num();)
		{
			auto& DelegateBaseRef = InvocationList[InvocationListIndex];

			IDelegateInstance* DelegateInstance = DelegateBaseRef.GetDelegateInstanceProtected();
			if (DelegateInstance == nullptr || DelegateInstance->IsCompactable())
			{
				InvocationList.RemoveAtSwap(InvocationListIndex);
			}
			else
			{
				InvocationListIndex++;
			}
		}

		CompactionThreshold = (2 * InvocationList.Num()) > 2 ? (2 * InvocationList.Num()) : 2;

		if (OldNumItems > CompactionThreshold)
		{
			// would be nice to shrink down to threshold, but reserve only grows..?
			InvocationList.Shrink();
		}
	}

	inline InvocationListType& GetInvocationList()
	{
		return InvocationList;
	}

	inline const InvocationListType& GetInvocationList() const
	{
		return InvocationList;
	}

	/** Increments the lock counter for the invocation list. */
	inline void LockInvocationList() const
	{
		++InvocationListLockCount;
	}

	/** Decrements the lock counter for the invocation list. */
	inline void UnlockInvocationList() const
	{
		--InvocationListLockCount;
	}

	/** Returns the lock counter for the invocation list. */
	inline int32 GetInvocationListLockCount() const
	{
		return InvocationListLockCount;
	}

	/**
	 * Helper function for derived classes of TMulticastDelegateBase to get at the delegate instance.
	 */
	template <typename DelegateType>
	static FORCEINLINE auto* GetDelegateInstanceProtectedHelper(const DelegateType& Base)
	{
		return Base.GetDelegateInstanceProtected();
	}

private:
	/** Holds the collection of delegate instances to invoke. */
	InvocationListType InvocationList;

	/** Used to determine when a compaction should happen. */
	int32 CompactionThreshold;

	/** Holds a lock counter for the invocation list. */
	mutable int32 InvocationListLockCount;
};
