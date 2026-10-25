#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Core/Public/Templates/UnrealTemplate.h"
#include "Engine/Source/Runtime/Core/Public/Templates/SharedPointer.h"
#include "Engine/Source/Runtime/Core/Public/Delegates/DelegateBase.h"
#include "Engine/Source/Runtime/Core/Public/Delegates/DelegateInstanceInterface.h"
#include <tuple>
#include <functional>
#include <type_traits>

namespace UE4Delegates_Private
{
	constexpr bool IsUObjectPtr(const volatile UObject*) { return true; }
	constexpr bool IsUObjectPtr(...) { return false; }

	inline UFunction* FindFunctionChecked(const UObject* Object, const FName& InFunctionName)
	{
		for (const UStruct* Struct = Object->Class; Struct; Struct = Struct->Super)
		{
			for (UField* Field = Struct->Children; Field; Field = Field->Next)
			{
				if (Field->Name == InFunctionName && Field->HasTypeFlag(EClassCastFlags::Function))
				{
					return static_cast<UFunction*>(Field);
				}
			}
		}

		return nullptr;
	}

	template <typename T>
	struct TMemberPointerClass;

	template <typename RetType, typename ClassType>
	struct TMemberPointerClass<RetType ClassType::*>
	{
		using Type = ClassType;
	};

	template <typename ClassType>
	struct TIsDelegateInterface : std::false_type {};

	template <>
	struct TIsDelegateInterface<IDelegateInstance> : std::true_type {};

	template <typename FuncType, typename UserPolicy>
	struct TIsDelegateInterface<IBaseDelegateInstance<FuncType, UserPolicy>> : std::true_type {};

	template <typename MemberFunctionType>
	FORCEINLINE void* ToVTableEntry(MemberFunctionType Function)
	{
		static_assert(sizeof(MemberFunctionType) == sizeof(void*), "Delegate instance member functions must be single inheritance");
		static_assert(!TIsDelegateInterface<typename TMemberPointerClass<MemberFunctionType>::Type>::value,
			"Binding is missing an implementation, the slot would point back at the IDelegateInstance dispatcher");

		void* Result;
		memcpy(&Result, &Function, sizeof(void*));
		return Result;
	}
}

/**
 * Builds the game layout vtable for a binding type (19.10 shipping IDelegateInstance, 11 slots: 0x83D2BB8 / 0x8469F58 in the game).
 */
template <typename Self>
struct TDelegateInstanceVTable
{
	/** Slot 0 - same as the game's: run the destructor, only free when asked to. `this` is really a Self. */
	void* ScalarDeletingDestructor(uint32 Flags)
	{
		Self* Instance = reinterpret_cast<Self*>(this);
		Instance->~Self();
		if (Flags & 1)
		{
			FMemory::Free(Instance);
		}
		return Instance;
	}

	static void** Get()
	{
		using namespace UE4Delegates_Private;

		static void* VTable[11] =
		{
			ToVTableEntry(&TDelegateInstanceVTable::ScalarDeletingDestructor), // ScalarDeletingDestructor
			ToVTableEntry(&Self::GetUObject),                                  // GetUObject
			ToVTableEntry(&Self::GetObjectForTimerManager),                    // GetObjectForTimerManager
			ToVTableEntry(&Self::GetBoundProgramCounterForTimerManager),       // GetBoundProgramCounterForTimerManager
			ToVTableEntry(&Self::HasSameObject),                               // HasSameObject
			ToVTableEntry(&Self::IsCompactable),                               // IsCompactable
			ToVTableEntry(&Self::IsSafeToExecute),                             // IsSafeToExecute
			ToVTableEntry(&Self::GetHandle),                                   // GetHandle
			ToVTableEntry(&Self::CreateCopy),                                  // CreateCopy
			ToVTableEntry(&Self::Execute),                                     // Execute
			ToVTableEntry(&Self::ExecuteIfSafe),                               // ExecuteIfSafe
		};
		return VTable;
	}
};

/** TTuple<VarTypes...> Payload with ApplyAfter, payload values are passed after the call parameters */
template <typename... VarTypes>
struct TDelegatePayload
{
	mutable std::tuple<VarTypes...> Values;

	explicit TDelegatePayload(VarTypes... Vars)
		: Values(Vars...)
	{
	}

	template <typename FuncType, typename... ParamTypes>
	decltype(auto) ApplyAfter(FuncType&& Func, ParamTypes&&... Params) const
	{
		return std::apply([&](auto&... Vars) -> decltype(auto)
			{
				return std::invoke(Forward<FuncType>(Func), Forward<ParamTypes>(Params)..., Vars...);
			}, Values);
	}
};

/**
 * Common state for all delegate bindings: the payload and the handle.
 * Also provides the IsCompactable and GetHandle implementations every binding shares (slots 5 and 7).
 */
template <typename FuncType, typename UserPolicy, typename... VarTypes>
class TCommonDelegateInstanceState;

template <typename InRetValType, typename... ParamTypes, typename UserPolicy, typename... VarTypes>
class TCommonDelegateInstanceState<InRetValType(ParamTypes...), UserPolicy, VarTypes...> : public IBaseDelegateInstance<InRetValType(ParamTypes...), UserPolicy>
{
public:
	using RetValType = InRetValType;

public:
	explicit TCommonDelegateInstanceState(VarTypes... Vars)
		: Payload(Vars...)
		, Handle(FDelegateHandle::GenerateNewHandle)
	{
	}

	/** Default IsCompactable, like the game's (sub_C930C4): goes back through the vtable for IsSafeToExecute */
	bool IsCompactable() const
	{
		return !IDelegateInstance::IsSafeToExecute();
	}

	FDelegateHandle GetHandle() const
	{
		return Handle;
	}

protected:
	// Payload member variables (if any).
	TDelegatePayload<VarTypes...> Payload;

	// The handle of this delegate
	FDelegateHandle Handle;
};

/**
 * Implements a delegate binding for UFunctions.
 */
template <class UserClass, typename FuncType, typename UserPolicy, typename... VarTypes>
class TBaseUFunctionDelegateInstance;

template <class UserClass, typename WrappedRetValType, typename... ParamTypes, typename UserPolicy, typename... VarTypes>
class TBaseUFunctionDelegateInstance<UserClass, WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...> : public TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>
{
private:
	using Super = TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>;
	using RetValType = typename Super::RetValType;
	using UnwrappedThisType = TBaseUFunctionDelegateInstance<UserClass, RetValType(ParamTypes...), UserPolicy, VarTypes...>;
	using FParmsWithPayload = TPayload<RetValType(std::decay_t<ParamTypes>..., std::decay_t<VarTypes>...)>;

	static_assert(UE4Delegates_Private::IsUObjectPtr((UserClass*)nullptr), "You cannot use UFunction delegates with non UObject classes.");

public:
	TBaseUFunctionDelegateInstance(UserClass* InUserObject, const FName& InFunctionName, VarTypes... Vars)
		: Super(Vars...)
		, CachedFunction(nullptr)
		, FunctionName(InFunctionName)
	{
		this->VTable = TDelegateInstanceVTable<UnwrappedThisType>::Get();
		UserObjectPtr = InUserObject;

		if (InUserObject != nullptr)
		{
			CachedFunction = UE4Delegates_Private::FindFunctionChecked(InUserObject, InFunctionName);
		}
	}

	// IDelegateInstance interface

	UObject* GetUObject() const
	{
		return UserObjectPtr.Get();
	}

	const void* GetObjectForTimerManager() const
	{
		return UserObjectPtr.Get();
	}

	uint64 GetBoundProgramCounterForTimerManager() const
	{
		return 0;
	}

	bool HasSameObject(const void* InUserObject) const
	{
		return UserObjectPtr.Get() == InUserObject;
	}

	bool IsCompactable() const
	{
		return !UserObjectPtr.Get(true);
	}

	bool IsSafeToExecute() const
	{
		return CachedFunction && UserObjectPtr.IsValid();
	}

public:
	// IBaseDelegateInstance interface

	void CreateCopy(FDelegateBase& Base)
	{
		new (Base) UnwrappedThisType(*(UnwrappedThisType*)this);
	}

	RetValType Execute(ParamTypes... Params) const
	{
		return CallProcessEvent(UserObjectPtr.Get(), Params...);
	}

	bool ExecuteIfSafe(ParamTypes... Params) const
	{
		if (UserClass* ActualUserObject = UserObjectPtr.Get())
		{
			if (CachedFunction)
			{
				(void)CallProcessEvent(ActualUserObject, Params...);
				return true;
			}
		}

		return false;
	}

private:
	RetValType CallProcessEvent(UserClass* ActualUserObject, ParamTypes... Params) const
	{
		return std::apply([&](auto&... Vars) -> RetValType
			{
				FParmsWithPayload PayloadAndParams(Params..., Vars...);
				ActualUserObject->ProcessEvent(CachedFunction, &PayloadAndParams);
				return PayloadAndParams.GetResult();
			}, this->Payload.Values);
	}

public:
	FORCEINLINE static void Create(FDelegateBase& Base, UserClass* InUserObject, const FName& InFunctionName, VarTypes... Vars)
	{
		new (Base) UnwrappedThisType(InUserObject, InFunctionName, Vars...);
	}

public:
	// Holds the cached UFunction to call.
	UFunction* CachedFunction;

	// Holds the name of the function to call.
	FName FunctionName;

	// The user object to call the function on.
	TWeakObjectPtr<UserClass> UserObjectPtr;
};

/**
 * Implements a delegate binding for shared pointer member functions.
 */
template <bool bConst, class UserClass, ESPMode SPMode, typename FuncType, typename UserPolicy, typename... VarTypes>
class TBaseSPMethodDelegateInstance;

template <bool bConst, class UserClass, ESPMode SPMode, typename WrappedRetValType, typename... ParamTypes, typename UserPolicy, typename... VarTypes>
class TBaseSPMethodDelegateInstance<bConst, UserClass, SPMode, WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...> : public TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>
{
private:
	using Super = TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>;
	using RetValType = typename Super::RetValType;
	using UnwrappedThisType = TBaseSPMethodDelegateInstance<bConst, UserClass, SPMode, RetValType(ParamTypes...), UserPolicy, VarTypes...>;

public:
	using FMethodPtr = typename TMemFunPtrType<bConst, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type;

	TBaseSPMethodDelegateInstance(const TSharedPtr<UserClass, SPMode>& InUserObject, FMethodPtr InMethodPtr, VarTypes... Vars)
		: Super(Vars...)
		, UserObject(InUserObject)
		, MethodPtr(InMethodPtr)
	{
		this->VTable = TDelegateInstanceVTable<UnwrappedThisType>::Get();
	}

	// IDelegateInstance interface

	UObject* GetUObject() const
	{
		return nullptr;
	}

	const void* GetObjectForTimerManager() const
	{
		return UserObject.Pin().Get();
	}

	uint64 GetBoundProgramCounterForTimerManager() const
	{
		return *((uint64*)&MethodPtr);
	}

	// Deprecated
	bool HasSameObject(const void* InUserObject) const
	{
		return UserObject.HasSameObject(InUserObject);
	}

	bool IsSafeToExecute() const
	{
		return UserObject.IsValid();
	}

public:
	// IBaseDelegateInstance interface

	void CreateCopy(FDelegateBase& Base)
	{
		new (Base) UnwrappedThisType(*(UnwrappedThisType*)this);
	}

	RetValType Execute(ParamTypes... Params) const
	{
		using MutableUserClass = std::remove_const_t<UserClass>;

		// Verify that the user object is still valid.  We only have a weak reference to it.
		TSharedPtr<UserClass, SPMode> SharedUserObject = UserObject.Pin();

		MutableUserClass* MutableUserObject = const_cast<MutableUserClass*>(SharedUserObject.Get());

		return this->Payload.ApplyAfter(MethodPtr, MutableUserObject, Params...);
	}

	bool ExecuteIfSafe(ParamTypes... Params) const
	{
		// Verify that the user object is still valid.  We only have a weak reference to it.
		if (TSharedPtr<UserClass, SPMode> SharedUserObject = this->UserObject.Pin())
		{
			using MutableUserClass = std::remove_const_t<UserClass>;

			MutableUserClass* MutableUserObject = const_cast<MutableUserClass*>(SharedUserObject.Get());

			(void)this->Payload.ApplyAfter(MethodPtr, MutableUserObject, Params...);

			return true;
		}

		return false;
	}

public:
	FORCEINLINE static void Create(FDelegateBase& Base, const TSharedPtr<UserClass, SPMode>& InUserObjectRef, FMethodPtr InFunc, VarTypes... Vars)
	{
		new (Base) UnwrappedThisType(InUserObjectRef, InFunc, Vars...);
	}

	FORCEINLINE static void Create(FDelegateBase& Base, UserClass* InUserObject, FMethodPtr InFunc, VarTypes... Vars)
	{
		// We expect the incoming InUserObject to derived from TSharedFromThis.
		TSharedRef<UserClass, SPMode> UserObjectRef = StaticCastSharedRef<UserClass>(InUserObject->AsShared());
		Create(Base, UserObjectRef, InFunc, Vars...);
	}

protected:
	// Weak reference to an instance of the user's class which contains a method we would like to call.
	TWeakPtr<UserClass, SPMode> UserObject;

	// C++ member function pointer.
	FMethodPtr MethodPtr;
};

/**
 * Implements a delegate binding for C++ member functions.
 */
template <bool bConst, class UserClass, typename FuncType, typename UserPolicy, typename... VarTypes>
class TBaseRawMethodDelegateInstance;

template <bool bConst, class UserClass, typename WrappedRetValType, typename... ParamTypes, typename UserPolicy, typename... VarTypes>
class TBaseRawMethodDelegateInstance<bConst, UserClass, WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...> : public TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>
{
private:
	static_assert(!UE4Delegates_Private::IsUObjectPtr((UserClass*)nullptr), "You cannot use raw method delegates with UObjects.");

	using Super = TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>;
	using RetValType = typename Super::RetValType;
	using UnwrappedThisType = TBaseRawMethodDelegateInstance<bConst, UserClass, RetValType(ParamTypes...), UserPolicy, VarTypes...>;

public:
	using FMethodPtr = typename TMemFunPtrType<bConst, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type;

	TBaseRawMethodDelegateInstance(UserClass* InUserObject, FMethodPtr InMethodPtr, VarTypes... Vars)
		: Super(Vars...)
		, UserObject(InUserObject)
		, MethodPtr(InMethodPtr)
	{
		this->VTable = TDelegateInstanceVTable<UnwrappedThisType>::Get();
	}

	// IDelegateInstance interface

	UObject* GetUObject() const
	{
		return nullptr;
	}

	const void* GetObjectForTimerManager() const
	{
		return UserObject;
	}

	uint64 GetBoundProgramCounterForTimerManager() const
	{
		return *((uint64*)&MethodPtr);
	}

	// Deprecated
	bool HasSameObject(const void* InUserObject) const
	{
		return UserObject == InUserObject;
	}

	bool IsSafeToExecute() const
	{
		// We never know whether or not it is safe to deference a C++ pointer, but we have to
		// trust the user in this case.  Prefer using a shared-pointer based delegate type instead!
		return true;
	}

public:
	// IBaseDelegateInstance interface

	void CreateCopy(FDelegateBase& Base)
	{
		new (Base) UnwrappedThisType(*(UnwrappedThisType*)this);
	}

	RetValType Execute(ParamTypes... Params) const
	{
		using MutableUserClass = std::remove_const_t<UserClass>;

		// Safely remove const to work around a compiler issue with instantiating template permutations for
		// overloaded functions that take a function pointer typedef as a member of a templated class.  In
		// all cases where this code is actually invoked, the UserClass will already be a const pointer.
		MutableUserClass* MutableUserObject = const_cast<MutableUserClass*>(UserObject);

		return this->Payload.ApplyAfter(MethodPtr, MutableUserObject, Params...);
	}

	bool ExecuteIfSafe(ParamTypes... Params) const
	{
		using MutableUserClass = std::remove_const_t<UserClass>;

		MutableUserClass* MutableUserObject = const_cast<MutableUserClass*>(UserObject);

		(void)this->Payload.ApplyAfter(MethodPtr, MutableUserObject, Params...);

		return true;
	}

public:
	FORCEINLINE static void Create(FDelegateBase& Base, UserClass* InUserObject, FMethodPtr InFunc, VarTypes... Vars)
	{
		new (Base) UnwrappedThisType(InUserObject, InFunc, Vars...);
	}

protected:
	// Pointer to the user's class which contains a method we would like to call.
	UserClass* UserObject;

	// C++ member function pointer.
	FMethodPtr MethodPtr;
};

/**
 * Implements a delegate binding for UObject methods.
 */
template <bool bConst, class UserClass, typename FuncType, typename UserPolicy, typename... VarTypes>
class TBaseUObjectMethodDelegateInstance;

template <bool bConst, class UserClass, typename WrappedRetValType, typename... ParamTypes, typename UserPolicy, typename... VarTypes>
class TBaseUObjectMethodDelegateInstance<bConst, UserClass, WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...> : public TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>
{
private:
	using Super = TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>;
	using RetValType = typename Super::RetValType;
	using UnwrappedThisType = TBaseUObjectMethodDelegateInstance<bConst, UserClass, RetValType(ParamTypes...), UserPolicy, VarTypes...>;

	static_assert(UE4Delegates_Private::IsUObjectPtr((UserClass*)nullptr), "You cannot use UObject method delegates with raw pointers.");

public:
	using FMethodPtr = typename TMemFunPtrType<bConst, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type;

	TBaseUObjectMethodDelegateInstance(UserClass* InUserObject, FMethodPtr InMethodPtr, VarTypes... Vars)
		: Super(Vars...)
		, MethodPtr(InMethodPtr)
	{
		this->VTable = TDelegateInstanceVTable<UnwrappedThisType>::Get();
		UserObject = InUserObject;
	}

	// IDelegateInstance interface

	UObject* GetUObject() const
	{
		return (UObject*)UserObject.Get();
	}

	const void* GetObjectForTimerManager() const
	{
		return UserObject.Get();
	}

	uint64 GetBoundProgramCounterForTimerManager() const
	{
		return *((uint64*)&MethodPtr);
	}

	// Deprecated
	bool HasSameObject(const void* InUserObject) const
	{
		return (UserObject.Get() == InUserObject);
	}

	bool IsCompactable() const
	{
		return !UserObject.Get(true);
	}

	bool IsSafeToExecute() const
	{
		return !!UserObject.Get();
	}

public:
	// IBaseDelegateInstance interface

	void CreateCopy(FDelegateBase& Base)
	{
		new (Base) UnwrappedThisType(*(UnwrappedThisType*)this);
	}

	RetValType Execute(ParamTypes... Params) const
	{
		using MutableUserClass = std::remove_const_t<UserClass>;

		MutableUserClass* MutableUserObject = const_cast<MutableUserClass*>(UserObject.Get());

		return this->Payload.ApplyAfter(MethodPtr, MutableUserObject, Params...);
	}

	bool ExecuteIfSafe(ParamTypes... Params) const
	{
		// Verify that the user object is still valid.  We only have a weak reference to it.
		if (UserClass* ActualUserObject = this->UserObject.Get())
		{
			using MutableUserClass = std::remove_const_t<UserClass>;

			MutableUserClass* MutableUserObject = const_cast<MutableUserClass*>(ActualUserObject);

			(void)this->Payload.ApplyAfter(MethodPtr, MutableUserObject, Params...);

			return true;
		}
		return false;
	}

public:
	FORCEINLINE static void Create(FDelegateBase& Base, UserClass* InUserObject, FMethodPtr InFunc, VarTypes... Vars)
	{
		new (Base) UnwrappedThisType(InUserObject, InFunc, Vars...);
	}

protected:
	// Pointer to the user's class which contains a method we would like to call.
	TWeakObjectPtr<std::remove_const_t<UserClass>> UserObject;

	// C++ member function pointer.
	FMethodPtr MethodPtr;
};

/**
 * Implements a delegate binding for regular C++ functions.
 */
template <typename FuncType, typename UserPolicy, typename... VarTypes>
class TBaseStaticDelegateInstance;

template <typename WrappedRetValType, typename... ParamTypes, typename UserPolicy, typename... VarTypes>
class TBaseStaticDelegateInstance<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...> : public TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>
{
private:
	using Super = TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>;
	using RetValType = typename Super::RetValType;
	using UnwrappedThisType = TBaseStaticDelegateInstance<RetValType(ParamTypes...), UserPolicy, VarTypes...>;

public:
	using FFuncPtr = RetValType(*)(ParamTypes..., VarTypes...);

	TBaseStaticDelegateInstance(FFuncPtr InStaticFuncPtr, VarTypes... Vars)
		: Super(Vars...)
		, StaticFuncPtr(InStaticFuncPtr)
	{
		this->VTable = TDelegateInstanceVTable<UnwrappedThisType>::Get();
	}

	// IDelegateInstance interface

	UObject* GetUObject() const
	{
		return nullptr;
	}

	const void* GetObjectForTimerManager() const
	{
		return nullptr;
	}

	uint64 GetBoundProgramCounterForTimerManager() const
	{
		return *((uint64*)&StaticFuncPtr);
	}

	// Deprecated
	bool HasSameObject(const void* UserObject) const
	{
		// Raw Delegates aren't bound to an object so they can never match
		return false;
	}

	bool IsSafeToExecute() const
	{
		// Static functions are always safe to execute!
		return true;
	}

public:
	// IBaseDelegateInstance interface

	void CreateCopy(FDelegateBase& Base)
	{
		new (Base) UnwrappedThisType(*(UnwrappedThisType*)this);
	}

	RetValType Execute(ParamTypes... Params) const
	{
		return this->Payload.ApplyAfter(StaticFuncPtr, Params...);
	}

	bool ExecuteIfSafe(ParamTypes... Params) const
	{
		(void)this->Payload.ApplyAfter(StaticFuncPtr, Params...);

		return true;
	}

public:
	FORCEINLINE static void Create(FDelegateBase& Base, FFuncPtr InFunc, VarTypes... Vars)
	{
		new (Base) UnwrappedThisType(InFunc, Vars...);
	}

private:
	// C++ function pointer.
	FFuncPtr StaticFuncPtr;
};

/**
 * Implements a delegate binding for C++ functors, e.g. lambdas.
 */
template <typename FuncType, typename UserPolicy, typename FunctorType, typename... VarTypes>
class TBaseFunctorDelegateInstance;

template <typename WrappedRetValType, typename... ParamTypes, typename UserPolicy, typename FunctorType, typename... VarTypes>
class TBaseFunctorDelegateInstance<WrappedRetValType(ParamTypes...), UserPolicy, FunctorType, VarTypes...> : public TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>
{
private:
	static_assert(std::is_same_v<FunctorType, std::remove_reference_t<FunctorType>>, "FunctorType cannot be a reference");

	using Super = TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>;
	using RetValType = typename Super::RetValType;
	using UnwrappedThisType = TBaseFunctorDelegateInstance<RetValType(ParamTypes...), UserPolicy, FunctorType, VarTypes...>;

public:
	TBaseFunctorDelegateInstance(const FunctorType& InFunctor, VarTypes... Vars)
		: Super(Vars...)
		, Functor(InFunctor)
	{
		this->VTable = TDelegateInstanceVTable<UnwrappedThisType>::Get();
	}

	TBaseFunctorDelegateInstance(FunctorType&& InFunctor, VarTypes... Vars)
		: Super(Vars...)
		, Functor(MoveTemp(InFunctor))
	{
		this->VTable = TDelegateInstanceVTable<UnwrappedThisType>::Get();
	}

	// IDelegateInstance interface

	UObject* GetUObject() const
	{
		return nullptr;
	}

	const void* GetObjectForTimerManager() const
	{
		return nullptr;
	}

	uint64 GetBoundProgramCounterForTimerManager() const
	{
		return 0;
	}

	// Deprecated
	bool HasSameObject(const void* UserObject) const
	{
		// Functor Delegates aren't bound to a user object so they can never match
		return false;
	}

	bool IsSafeToExecute() const
	{
		// Functors are always considered safe to execute!
		return true;
	}

public:
	// IBaseDelegateInstance interface

	void CreateCopy(FDelegateBase& Base)
	{
		new (Base) UnwrappedThisType(*(UnwrappedThisType*)this);
	}

	RetValType Execute(ParamTypes... Params) const
	{
		return this->Payload.ApplyAfter(Functor, Params...);
	}

	bool ExecuteIfSafe(ParamTypes... Params) const
	{
		// Functors are always considered safe to execute!
		(void)this->Payload.ApplyAfter(Functor, Params...);

		return true;
	}

public:
	FORCEINLINE static void Create(FDelegateBase& Base, const FunctorType& InFunctor, VarTypes... Vars)
	{
		new (Base) UnwrappedThisType(InFunctor, Vars...);
	}

	FORCEINLINE static void Create(FDelegateBase& Base, FunctorType&& InFunctor, VarTypes... Vars)
	{
		new (Base) UnwrappedThisType(MoveTemp(InFunctor), Vars...);
	}

private:
	// C++ functor
	// We make this mutable to allow mutable lambdas to be bound and executed.
	mutable std::remove_const_t<FunctorType> Functor;
};

/**
 * Implements a weak object delegate binding for C++ functors, e.g. lambdas.
 */
template <typename UserClass, typename FuncType, typename UserPolicy, typename FunctorType, typename... VarTypes>
class TWeakBaseFunctorDelegateInstance;

template <typename UserClass, typename WrappedRetValType, typename... ParamTypes, typename UserPolicy, typename FunctorType, typename... VarTypes>
class TWeakBaseFunctorDelegateInstance<UserClass, WrappedRetValType(ParamTypes...), UserPolicy, FunctorType, VarTypes...> : public TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>
{
private:
	static_assert(std::is_same_v<FunctorType, std::remove_reference_t<FunctorType>>, "FunctorType cannot be a reference");

	using Super = TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>;
	using RetValType = typename Super::RetValType;
	using UnwrappedThisType = TWeakBaseFunctorDelegateInstance<UserClass, RetValType(ParamTypes...), UserPolicy, FunctorType, VarTypes...>;

public:
	TWeakBaseFunctorDelegateInstance(UserClass* InContextObject, const FunctorType& InFunctor, VarTypes... Vars)
		: Super(Vars...)
		, Functor(InFunctor)
	{
		this->VTable = TDelegateInstanceVTable<UnwrappedThisType>::Get();
		ContextObject = InContextObject;
	}

	TWeakBaseFunctorDelegateInstance(UserClass* InContextObject, FunctorType&& InFunctor, VarTypes... Vars)
		: Super(Vars...)
		, Functor(MoveTemp(InFunctor))
	{
		this->VTable = TDelegateInstanceVTable<UnwrappedThisType>::Get();
		ContextObject = InContextObject;
	}

	// IDelegateInstance interface

	UObject* GetUObject() const
	{
		return (UObject*)ContextObject.Get();
	}

	const void* GetObjectForTimerManager() const
	{
		return ContextObject.Get();
	}

	uint64 GetBoundProgramCounterForTimerManager() const
	{
		return 0;
	}

	// Deprecated
	bool HasSameObject(const void* InContextObject) const
	{
		return GetUObject() == InContextObject;
	}

	bool IsCompactable() const
	{
		return !ContextObject.Get(true);
	}

	bool IsSafeToExecute() const
	{
		return ContextObject.IsValid();
	}

public:
	// IBaseDelegateInstance interface

	void CreateCopy(FDelegateBase& Base)
	{
		new (Base) UnwrappedThisType(*(UnwrappedThisType*)this);
	}

	RetValType Execute(ParamTypes... Params) const
	{
		return this->Payload.ApplyAfter(Functor, Params...);
	}

	bool ExecuteIfSafe(ParamTypes... Params) const
	{
		if (ContextObject.IsValid())
		{
			(void)this->Payload.ApplyAfter(Functor, Params...);
			return true;
		}

		return false;
	}

public:
	FORCEINLINE static void Create(FDelegateBase& Base, UserClass* InContextObject, const FunctorType& InFunctor, VarTypes... Vars)
	{
		new (Base) UnwrappedThisType(InContextObject, InFunctor, Vars...);
	}

	FORCEINLINE static void Create(FDelegateBase& Base, UserClass* InContextObject, FunctorType&& InFunctor, VarTypes... Vars)
	{
		new (Base) UnwrappedThisType(InContextObject, MoveTemp(InFunctor), Vars...);
	}

private:
	// Context object - the validity of this object controls the validity of the lambda
	TWeakObjectPtr<std::remove_const_t<UserClass>> ContextObject;

	// C++ functor
	mutable std::remove_const_t<FunctorType> Functor;
};