// Only designed to be included directly by Delegate.h
#if !defined( __Delegate_h__ ) || !defined( FUNC_INCLUDING_INLINE_IMPL )
#error "This inline header must only be included by Delegate.h"
#endif

#pragma once
#include <type_traits>

/**
 * Unicast delegate template class.
 *
 * Use the various DECLARE_DELEGATE macros to create the actual delegate type, templated to
 * the function signature the delegate is compatible with. Then, you can create an instance
 * of that class when you want to bind a function to the delegate.
 */
template <typename DelegateSignature, typename UserPolicy = FDefaultDelegateUserPolicy>
class TDelegate
{
	static_assert(sizeof(DelegateSignature) == 0, "Expected a function signature for the delegate template parameter");
};

template <typename InRetValType, typename... ParamTypes, typename UserPolicy>
class TDelegate<InRetValType(ParamTypes...), UserPolicy> : public TDelegateBase<UserPolicy>
{
	using Super = TDelegateBase<UserPolicy>;
	using FuncType = InRetValType(ParamTypes...);
	using DelegateInstanceInterfaceType = IBaseDelegateInstance<FuncType, UserPolicy>;

	template <typename>
	friend class ::TMulticastDelegateBase;

public:
	using Super::Unbind;

private:
	using Super::GetDelegateInstanceProtected;

public:
	/** Type definition for return value type. */
	typedef InRetValType RetValType;
	typedef InRetValType TFuncType(ParamTypes...);

	/* Helper typedefs for getting a member function pointer type for the delegate with a given payload */
	template <typename UserClass, typename... VarTypes> using TMethodPtr = typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type;
	template <typename UserClass, typename... VarTypes> using TConstMethodPtr = typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type;

public:
	/**
	 * Static: Creates a raw C++ pointer global function delegate
	 */
	template <typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateStatic(typename TIdentity<RetValType(*)(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		TDelegate Result;
		TBaseStaticDelegateInstance<FuncType, UserPolicy, VarTypes...>::Create(Result, InFunc, Vars...);
		return Result;
	}

	/**
	 * Static: Creates a C++ lambda delegate
	 * technically this works for any functor types, but lambdas are the primary use case
	 */
	template<typename FunctorType, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateLambda(FunctorType&& InFunctor, VarTypes... Vars)
	{
		TDelegate Result;
		TBaseFunctorDelegateInstance<FuncType, UserPolicy, std::remove_reference_t<FunctorType>, VarTypes...>::Create(Result, Forward<FunctorType>(InFunctor), Vars...);
		return Result;
	}

	/**
	 * Static: Creates a weak object C++ lambda delegate
	 * technically this works for any functor types, but lambdas are the primary use case
	 */
	template<typename UserClass, typename FunctorType, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateWeakLambda(UserClass* InUserObject, FunctorType&& InFunctor, VarTypes... Vars)
	{
		TDelegate Result;
		TWeakBaseFunctorDelegateInstance<UserClass, FuncType, UserPolicy, std::remove_reference_t<FunctorType>, VarTypes...>::Create(Result, InUserObject, Forward<FunctorType>(InFunctor), Vars...);
		return Result;
	}

	/**
	 * Static: Creates a raw C++ pointer member function delegate.
	 *
	 * Raw pointer doesn't use any sort of reference, so may be unsafe to call if the object was
	 * deleted out from underneath your delegate. Be careful when calling Execute()!
	 */
	template <typename UserClass, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateRaw(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		TDelegate Result;
		TBaseRawMethodDelegateInstance<false, UserClass, FuncType, UserPolicy, VarTypes...>::Create(Result, InUserObject, InFunc, Vars...);
		return Result;
	}
	template <typename UserClass, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateRaw(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		TDelegate Result;
		TBaseRawMethodDelegateInstance<true, const UserClass, FuncType, UserPolicy, VarTypes...>::Create(Result, InUserObject, InFunc, Vars...);
		return Result;
	}

	/**
	 * Static: Creates a shared pointer-based member function delegate.
	 *
	 * Shared pointer delegates keep a weak reference to your object.
	 * You can use ExecuteIfBound() to call them.
	 */
	template <typename UserClass, ESPMode Mode, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateSP(const TSharedRef<UserClass, Mode>& InUserObjectRef, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		TDelegate Result;
		TBaseSPMethodDelegateInstance<false, UserClass, Mode, FuncType, UserPolicy, VarTypes...>::Create(Result, InUserObjectRef, InFunc, Vars...);
		return Result;
	}
	template <typename UserClass, ESPMode Mode, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateSP(const TSharedRef<UserClass, Mode>& InUserObjectRef, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		TDelegate Result;
		TBaseSPMethodDelegateInstance<true, const UserClass, Mode, FuncType, UserPolicy, VarTypes...>::Create(Result, InUserObjectRef, InFunc, Vars...);
		return Result;
	}

	/**
	 * Static: Creates a shared pointer-based member function delegate.
	 * The object must derive from TSharedFromThis.
	 */
	template <typename UserClass, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateSP(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		return CreateSP(StaticCastSharedRef<UserClass>(InUserObject->AsShared()), InFunc, Vars...);
	}
	template <typename UserClass, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateSP(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		return CreateSP(StaticCastSharedRef<const UserClass>(InUserObject->AsShared()), InFunc, Vars...);
	}

	/**
	 * Static: Creates a shared pointer-based (thread-safe) member function delegate.
	 */
	template <typename UserClass, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateThreadSafeSP(const TSharedRef<UserClass, ESPMode::ThreadSafe>& InUserObjectRef, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		TDelegate Result;
		TBaseSPMethodDelegateInstance<false, UserClass, ESPMode::ThreadSafe, FuncType, UserPolicy, VarTypes...>::Create(Result, InUserObjectRef, InFunc, Vars...);
		return Result;
	}
	template <typename UserClass, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateThreadSafeSP(const TSharedRef<UserClass, ESPMode::ThreadSafe>& InUserObjectRef, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		TDelegate Result;
		TBaseSPMethodDelegateInstance<true, const UserClass, ESPMode::ThreadSafe, FuncType, UserPolicy, VarTypes...>::Create(Result, InUserObjectRef, InFunc, Vars...);
		return Result;
	}

	/**
	 * Static: Creates a UFunction-based member function delegate.
	 *
	 * UFunction delegates keep a weak reference to your object.
	 * You can use ExecuteIfBound() to call them.
	 */
	template <typename UObjectTemplate, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateUFunction(UObjectTemplate* InUserObject, const FName& InFunctionName, VarTypes... Vars)
	{
		TDelegate Result;
		TBaseUFunctionDelegateInstance<UObjectTemplate, FuncType, UserPolicy, VarTypes...>::Create(Result, InUserObject, InFunctionName, Vars...);
		return Result;
	}

	/**
	 * Static: Creates a UObject-based member function delegate.
	 *
	 * UObject delegates keep a weak reference to your object.
	 * You can use ExecuteIfBound() to call them.
	 */
	template <typename UserClass, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateUObject(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		TDelegate Result;
		TBaseUObjectMethodDelegateInstance<false, UserClass, FuncType, UserPolicy, VarTypes...>::Create(Result, InUserObject, InFunc, Vars...);
		return Result;
	}
	template <typename UserClass, typename... VarTypes>
	[[nodiscard]] inline static TDelegate CreateUObject(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		TDelegate Result;
		TBaseUObjectMethodDelegateInstance<true, const UserClass, FuncType, UserPolicy, VarTypes...>::Create(Result, InUserObject, InFunc, Vars...);
		return Result;
	}

public:
	/**
	 * Default constructor
	 */
	inline TDelegate()
	{
	}

	/**
	 * 'Null' constructor
	 */
	inline TDelegate(std::nullptr_t)
	{
	}

	/**
	 * Destructor.
	 */
	inline ~TDelegate()
	{
		Unbind();
	}

	/**
	 * Move constructor.
	 */
	inline TDelegate(TDelegate&& Other)
	{
		*this = MoveTemp(Other);
	}

	/**
	 * Creates and initializes a new instance from an existing delegate object.
	 */
	inline TDelegate(const TDelegate& Other)
	{
		*this = Other;
	}

	/**
	 * Move assignment operator.
	 */
	inline TDelegate& operator=(TDelegate&& Other)
	{
		if (&Other != this)
		{
			// this down-cast is OK! allows for managing invocation list in the base class without requiring virtual functions
			DelegateInstanceInterfaceType* OtherInstance = Other.GetDelegateInstanceProtected();

			if (OtherInstance != nullptr)
			{
				OtherInstance->CreateCopy(*this);
			}
			else
			{
				Unbind();
			}
		}

		return *this;
	}

	/**
	 * Assignment operator.
	 */
	inline TDelegate& operator=(const TDelegate& Other)
	{
		if (&Other != this)
		{
			// this down-cast is OK! allows for managing invocation list in the base class without requiring virtual functions
			DelegateInstanceInterfaceType* OtherInstance = Other.GetDelegateInstanceProtected();

			if (OtherInstance != nullptr)
			{
				OtherInstance->CreateCopy(*this);
			}
			else
			{
				Unbind();
			}
		}

		return *this;
	}

public:
	/**
	 * Binds a raw C++ pointer global function delegate
	 */
	template <typename... VarTypes>
	inline void BindStatic(typename TBaseStaticDelegateInstance<FuncType, UserPolicy, VarTypes...>::FFuncPtr InFunc, VarTypes... Vars)
	{
		*this = CreateStatic(InFunc, Vars...);
	}

	/**
	 * Static: Binds a C++ lambda delegate
	 * technically this works for any functor types, but lambdas are the primary use case
	 */
	template<typename FunctorType, typename... VarTypes>
	inline void BindLambda(FunctorType&& InFunctor, VarTypes... Vars)
	{
		*this = CreateLambda(Forward<FunctorType>(InFunctor), Vars...);
	}

	/**
	 * Static: Binds a weak object C++ lambda delegate
	 * technically this works for any functor types, but lambdas are the primary use case
	 */
	template<typename UserClass, typename FunctorType, typename... VarTypes>
	inline void BindWeakLambda(UserClass* InUserObject, FunctorType&& InFunctor, VarTypes... Vars)
	{
		*this = CreateWeakLambda(InUserObject, Forward<FunctorType>(InFunctor), Vars...);
	}

	/**
	 * Binds a raw C++ pointer delegate.
	 *
	 * Raw pointer doesn't use any sort of reference, so may be unsafe to call if the object was
	 * deleted out from underneath your delegate. Be careful when calling Execute()!
	 */
	template <typename UserClass, typename... VarTypes>
	inline void BindRaw(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		*this = CreateRaw(InUserObject, InFunc, Vars...);
	}
	template <typename UserClass, typename... VarTypes>
	inline void BindRaw(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		*this = CreateRaw(InUserObject, InFunc, Vars...);
	}

	/**
	 * Binds a shared pointer-based member function delegate.  Shared pointer delegates keep a weak reference to your object.  You can use ExecuteIfBound() to call them.
	 */
	template <typename UserClass, ESPMode Mode, typename... VarTypes>
	inline void BindSP(const TSharedRef<UserClass, Mode>& InUserObjectRef, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		*this = CreateSP(InUserObjectRef, InFunc, Vars...);
	}
	template <typename UserClass, ESPMode Mode, typename... VarTypes>
	inline void BindSP(const TSharedRef<UserClass, Mode>& InUserObjectRef, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		*this = CreateSP(InUserObjectRef, InFunc, Vars...);
	}

	/**
	 * Binds a shared pointer-based member function delegate.
	 * The object must derive from TSharedFromThis.
	 */
	template <typename UserClass, typename... VarTypes>
	inline void BindSP(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		*this = CreateSP(InUserObject, InFunc, Vars...);
	}
	template <typename UserClass, typename... VarTypes>
	inline void BindSP(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		*this = CreateSP(InUserObject, InFunc, Vars...);
	}

	/**
	 * Binds a shared pointer-based (thread-safe) member function delegate.
	 */
	template <typename UserClass, typename... VarTypes>
	inline void BindThreadSafeSP(const TSharedRef<UserClass, ESPMode::ThreadSafe>& InUserObjectRef, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		*this = CreateThreadSafeSP(InUserObjectRef, InFunc, Vars...);
	}
	template <typename UserClass, typename... VarTypes>
	inline void BindThreadSafeSP(const TSharedRef<UserClass, ESPMode::ThreadSafe>& InUserObjectRef, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		*this = CreateThreadSafeSP(InUserObjectRef, InFunc, Vars...);
	}

	/**
	 * Binds a UFunction-based member function delegate.
	 *
	 * UFunction delegates keep a weak reference to your object.
	 * You can use ExecuteIfBound() to call them.
	 */
	template <typename UObjectTemplate, typename... VarTypes>
	inline void BindUFunction(UObjectTemplate* InUserObject, const FName& InFunctionName, VarTypes... Vars)
	{
		*this = CreateUFunction(InUserObject, InFunctionName, Vars...);
	}

	/**
	 * Binds a UObject-based member function delegate.
	 *
	 * UObject delegates keep a weak reference to your object.
	 * You can use ExecuteIfBound() to call them.
	 */
	template <typename UserClass, typename... VarTypes>
	inline void BindUObject(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		*this = CreateUObject(InUserObject, InFunc, Vars...);
	}
	template <typename UserClass, typename... VarTypes>
	inline void BindUObject(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		*this = CreateUObject(InUserObject, InFunc, Vars...);
	}

public:
	/**
	 * Execute the delegate.
	 *
	 * If the function pointer is not valid, an error will occur. Check IsBound() before
	 * calling this method or use ExecuteIfBound() instead.
	 */
	FORCEINLINE RetValType Execute(ParamTypes... Params) const
	{
		DelegateInstanceInterfaceType* LocalDelegateInstance = GetDelegateInstanceProtected();

		return LocalDelegateInstance->Execute(Params...);
	}

	/**
	 * Execute the delegate, but only if the function pointer is still valid
	 *
	 * @return  Returns true if the function was executed
	 */
	 // NOTE: Currently only delegates with no return value support ExecuteIfBound()
	template <
		typename DummyRetValType = RetValType,
		std::enable_if_t<std::is_void<DummyRetValType>::value>* = nullptr
	>
	inline bool ExecuteIfBound(ParamTypes... Params) const
	{
		if (DelegateInstanceInterfaceType* Ptr = GetDelegateInstanceProtected())
		{
			return Ptr->ExecuteIfSafe(Params...);
		}

		return false;
	}

protected:
	/**
	 * Returns a pointer to the correctly-typed delegate instance.
	 */
	FORCEINLINE DelegateInstanceInterfaceType* GetDelegateInstanceProtected() const
	{
		return (DelegateInstanceInterfaceType*)Super::GetDelegateInstanceProtected();
	}
};

/**
 * Multicast delegate template base class, used for both normal and event multicast delegates.
 *
 * This class implements the functionality of multicast delegates. It is templated to the function signature
 * that it is compatible with. Use the various DECLARE_MULTICAST_DELEGATE and DECLARE_EVENT macros to create
 * actual delegate types.
 *
 * Multicast delegates offer no guarantees for the calling order of bound functions. As bindings get added
 * and removed over time, the calling order may change. Only bindings without return values are supported.
 */
template <typename DelegateSignature, typename UserPolicy = FDefaultDelegateUserPolicy>
class TMulticastDelegate
{
	static_assert(sizeof(DelegateSignature) == 0, "Expected a function signature for the delegate template parameter");
};

template <typename RetValType, typename... ParamTypes, typename UserPolicy>
class TMulticastDelegate<RetValType(ParamTypes...), UserPolicy>
{
	static_assert(sizeof(RetValType) == 0, "The return type of a multicast delegate must be void");
};

template <typename... ParamTypes, typename UserPolicy>
class TMulticastDelegate<void(ParamTypes...), UserPolicy> : public UserPolicy::FMulticastDelegateExtras
{
	using Super = typename UserPolicy::FMulticastDelegateExtras;
	using InvocationListType = typename Super::InvocationListType;
	using DelegateInstanceInterfaceType = IBaseDelegateInstance<void(ParamTypes...), UserPolicy>;

public:
	/** Type definition for the shared interface of delegate instance types compatible with this delegate class. */
	using FDelegate = TDelegate<void(ParamTypes...), UserPolicy>;

public:
	// Make sure FDelegateBase's public functions are publicly exposed through the TMulticastDelegate API
	using Super::Clear;
	using Super::IsBound;
	using Super::IsBoundToObject;
	using Super::RemoveAll;

private:
	// Make sure FDelegateBase's protected functions are not accidentally exposed through the TMulticastDelegate API
	using Super::AddDelegateInstance;
	using Super::RemoveDelegateInstance;
	using Super::CompactInvocationList;
	using Super::GetInvocationList;
	using Super::LockInvocationList;
	using Super::UnlockInvocationList;
	using Super::GetInvocationListLockCount;
	using Super::GetDelegateInstanceProtectedHelper;

public:
	/**
	 * Adds a delegate instance to this multicast delegate's invocation list.
	 */
	FDelegateHandle Add(FDelegate&& InNewDelegate)
	{
		FDelegateHandle Result;
		if (Super::GetDelegateInstanceProtectedHelper(InNewDelegate))
		{
			Result = Super::AddDelegateInstance(MoveTemp(InNewDelegate));
		}

		return Result;
	}

	/**
	 * Adds a delegate instance to this multicast delegate's invocation list.
	 */
	FDelegateHandle Add(const FDelegate& InNewDelegate)
	{
		FDelegateHandle Result;
		if (Super::GetDelegateInstanceProtectedHelper(InNewDelegate))
		{
			Result = Super::AddDelegateInstance(CopyTemp(InNewDelegate));
		}

		return Result;
	}

	/**
	 * Adds a raw C++ pointer global function delegate
	 */
	template <typename... VarTypes>
	inline FDelegateHandle AddStatic(typename TBaseStaticDelegateInstance<void(ParamTypes...), UserPolicy, VarTypes...>::FFuncPtr InFunc, VarTypes... Vars)
	{
		return Add(FDelegate::CreateStatic(InFunc, Vars...));
	}

	/**
	 * Adds a C++ lambda delegate
	 * technically this works for any functor types, but lambdas are the primary use case
	 */
	template<typename FunctorType, typename... VarTypes>
	inline FDelegateHandle AddLambda(FunctorType&& InFunctor, VarTypes... Vars)
	{
		return Add(FDelegate::CreateLambda(Forward<FunctorType>(InFunctor), Vars...));
	}

	/**
	 * Adds a weak object C++ lambda delegate
	 * technically this works for any functor types, but lambdas are the primary use case
	 */
	template<typename UserClass, typename FunctorType, typename... VarTypes>
	inline FDelegateHandle AddWeakLambda(UserClass* InUserObject, FunctorType&& InFunctor, VarTypes... Vars)
	{
		return Add(FDelegate::CreateWeakLambda(InUserObject, Forward<FunctorType>(InFunctor), Vars...));
	}

	/**
	 * Adds a raw C++ pointer delegate.
	 *
	 * Raw pointer doesn't use any sort of reference, so may be unsafe to call if the object was
	 * deleted out from underneath your delegate. Be careful when calling Execute()!
	 */
	template <typename UserClass, typename... VarTypes>
	inline FDelegateHandle AddRaw(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		return Add(FDelegate::CreateRaw(InUserObject, InFunc, Vars...));
	}
	template <typename UserClass, typename... VarTypes>
	inline FDelegateHandle AddRaw(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		return Add(FDelegate::CreateRaw(InUserObject, InFunc, Vars...));
	}

	/**
	 * Adds a shared pointer-based member function delegate.
	 *
	 * Shared pointer delegates keep a weak reference to your object.
	 */
	template <typename UserClass, ESPMode Mode, typename... VarTypes>
	inline FDelegateHandle AddSP(const TSharedRef<UserClass, Mode>& InUserObjectRef, typename TMemFunPtrType<false, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		return Add(FDelegate::CreateSP(InUserObjectRef, InFunc, Vars...));
	}
	template <typename UserClass, ESPMode Mode, typename... VarTypes>
	inline FDelegateHandle AddSP(const TSharedRef<UserClass, Mode>& InUserObjectRef, typename TMemFunPtrType<true, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		return Add(FDelegate::CreateSP(InUserObjectRef, InFunc, Vars...));
	}

	/**
	 * Adds a shared pointer-based member function delegate.
	 * The object must derive from TSharedFromThis.
	 */
	template <typename UserClass, typename... VarTypes>
	inline FDelegateHandle AddSP(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		return Add(FDelegate::CreateSP(InUserObject, InFunc, Vars...));
	}
	template <typename UserClass, typename... VarTypes>
	inline FDelegateHandle AddSP(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		return Add(FDelegate::CreateSP(InUserObject, InFunc, Vars...));
	}

	/**
	 * Adds a shared pointer-based (thread-safe) member function delegate.
	 */
	template <typename UserClass, typename... VarTypes>
	inline FDelegateHandle AddThreadSafeSP(const TSharedRef<UserClass, ESPMode::ThreadSafe>& InUserObjectRef, typename TMemFunPtrType<false, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		return Add(FDelegate::CreateThreadSafeSP(InUserObjectRef, InFunc, Vars...));
	}
	template <typename UserClass, typename... VarTypes>
	inline FDelegateHandle AddThreadSafeSP(const TSharedRef<UserClass, ESPMode::ThreadSafe>& InUserObjectRef, typename TMemFunPtrType<true, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		return Add(FDelegate::CreateThreadSafeSP(InUserObjectRef, InFunc, Vars...));
	}

	/**
	 * Adds a UFunction-based member function delegate.
	 *
	 * UFunction delegates keep a weak reference to your object.
	 */
	template <typename UObjectTemplate, typename... VarTypes>
	inline FDelegateHandle AddUFunction(UObjectTemplate* InUserObject, const FName& InFunctionName, VarTypes... Vars)
	{
		return Add(FDelegate::CreateUFunction(InUserObject, InFunctionName, Vars...));
	}

	/**
	 * Adds a UObject-based member function delegate.
	 *
	 * UObject delegates keep a weak reference to your object.
	 */
	template <typename UserClass, typename... VarTypes>
	inline FDelegateHandle AddUObject(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		static_assert(!std::is_const_v<UserClass>, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		return Add(FDelegate::CreateUObject(InUserObject, InFunc, Vars...));
	}
	template <typename UserClass, typename... VarTypes>
	inline FDelegateHandle AddUObject(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		return Add(FDelegate::CreateUObject(InUserObject, InFunc, Vars...));
	}

public:
	/**
	 * Removes a delegate instance from this multi-cast delegate's invocation list (performance is O(N)).
	 *
	 * Note that the order of the delegate instances may not be preserved!
	 *
	 * @param Handle The handle of the delegate instance to remove.
	 * @return  true if the delegate was successfully removed.
	 */
	bool Remove(FDelegateHandle Handle)
	{
		bool bResult = false;
		if (Handle.IsValid())
		{
			bResult = RemoveDelegateInstance(Handle);
		}
		return bResult;
	}

	/** Hidden default constructor. */
	inline TMulticastDelegate() {}

	/** Hidden copy constructor (for proper deep copies). */
	TMulticastDelegate(const TMulticastDelegate& Other)
	{
		*this = Other;
	}

	/** Hidden assignment operator (for proper deep copies). */
	TMulticastDelegate& operator=(const TMulticastDelegate& Other)
	{
		if (&Other != this)
		{
			Super::Clear();

			for (const TDelegateBase<UserPolicy>& OtherDelegateRef : Other.GetInvocationList())
			{
				if (IDelegateInstance* OtherInstance = Super::GetDelegateInstanceProtectedHelper(OtherDelegateRef))
				{
					FDelegate TempDelegate;
					((DelegateInstanceInterfaceType*)OtherInstance)->CreateCopy(TempDelegate);
					Super::AddDelegateInstance(MoveTemp(TempDelegate));
				}
			}
		}

		return *this;
	}

public:
	/**
	 * Broadcasts this delegate to all bound objects, except to those that may have expired.
	 *
	 * The constness of this method is a lie, but it allows for broadcasting from const functions.
	 */
	void Broadcast(ParamTypes... Params) const
	{
		bool NeedsCompaction = false;

		Super::LockInvocationList();
		{
			const InvocationListType& LocalInvocationList = Super::GetInvocationList();

			// call bound functions in reverse order, so we ignore any instances that may be added by callees
			for (int32 InvocationListIndex = LocalInvocationList.Num() - 1; InvocationListIndex >= 0; --InvocationListIndex)
			{
				// this down-cast is OK! allows for managing invocation list in the base class without requiring virtual functions
				const FDelegate& DelegateBase = (const FDelegate&)LocalInvocationList[InvocationListIndex];

				IDelegateInstance* DelegateInstanceInterface = Super::GetDelegateInstanceProtectedHelper(DelegateBase);
				if (DelegateInstanceInterface == nullptr || !((DelegateInstanceInterfaceType*)DelegateInstanceInterface)->ExecuteIfSafe(Params...))
				{
					NeedsCompaction = true;
				}
			}
		}
		Super::UnlockInvocationList();

		if (NeedsCompaction)
		{
			const_cast<TMulticastDelegate*>(this)->CompactInvocationList();
		}
	}
};