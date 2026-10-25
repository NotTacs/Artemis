#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Core/Public/Templates/UnrealTemplate.h"
#include "Engine/Source/Runtime/Core/Public/Delegates/IDelegateInstance.h"
#include <array>
#include <memory>
#include <tuple>
#include <utility>

class FDelegateBase;

template <typename FuncType, typename UserPolicy>
struct IBaseDelegateInstance;

template <typename RetType, typename... ArgTypes, typename UserPolicy>
struct IBaseDelegateInstance<RetType(ArgTypes...), UserPolicy> : public UserPolicy::FDelegateInstanceExtras
{
	/**
	 * Emplaces a copy of the delegate instance into the FDelegateBase.
	 */
	FORCEINLINE void CreateCopy(FDelegateBase& Base)
	{
		InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<void (*)(IBaseDelegateInstance*, FDelegateBase&)>(this, 8), this, Base);
	}

	/**
	 * Execute the delegate.  If the function pointer is not valid, an error will occur.
	 */
	FORCEINLINE RetType Execute(ArgTypes... Args) const
	{
		if constexpr (std::is_class_v<RetType> || std::is_union_v<RetType>)
		{
			// Member functions return class types through a hidden pointer that comes after this, so it's an explicit out param here
			alignas(RetType) uint8 ResultStorage[sizeof(RetType)];
			RetType* Result = InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<RetType * (*)(const IBaseDelegateInstance*, RetType*, ArgTypes...)>(this, 9), this, reinterpret_cast<RetType*>(ResultStorage), Args...);
			RetType ReturnValue = MoveTemp(*Result);
			std::destroy_at(Result);
			return ReturnValue;
		}
		else
		{
			return InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<RetType(*)(const IBaseDelegateInstance*, ArgTypes...)>(this, 9), this, Args...);
		}
	}

	/**
	 * Execute the delegate, but only if the function pointer is still valid
	 *
	 * @return  Returns true if the function was executed
	 */
	 // NOTE: Currently only delegates with no return value support ExecuteIfSafe()
	FORCEINLINE bool ExecuteIfSafe(ArgTypes... Args) const
	{
		return InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<bool (*)(const IBaseDelegateInstance*, ArgTypes...)>(this, 10), this, Args...);
	}
};

template <bool Const, typename Class, typename FuncType>
struct TMemFunPtrType;

template <typename Class, typename RetType, typename... ArgTypes>
struct TMemFunPtrType<false, Class, RetType(ArgTypes...)>
{
	typedef RetType(Class::* Type)(ArgTypes...);
};

template <typename Class, typename RetType, typename... ArgTypes>
struct TMemFunPtrType<true, Class, RetType(ArgTypes...)>
{
	typedef RetType(Class::* Type)(ArgTypes...) const;
};

template <typename... Types>
struct TFlatParms
{
private:
	static constexpr size_t AlignUp(size_t Value, size_t Alignment)
	{
		return (Value + Alignment - 1) & ~(Alignment - 1);
	}

	static constexpr size_t MaxAlign()
	{
		size_t Result = 1;
		((Result = alignof(Types) > Result ? alignof(Types) : Result), ...);
		return Result;
	}

	static constexpr auto ComputeOffsets()
	{
		std::array<size_t, sizeof...(Types) + 1> Result{};
		size_t Offset = 0;
		size_t Index = 0;
		((Offset = AlignUp(Offset, alignof(Types)), Result[Index++] = Offset, Offset += sizeof(Types)), ...);
		Result[Index] = AlignUp(Offset, MaxAlign());
		return Result;
	}

	static constexpr auto Offsets = ComputeOffsets();

	template <size_t Index>
	using TElement = std::tuple_element_t<Index, std::tuple<Types...>>;

	template <size_t... Indices, typename... ArgTypes>
	void Construct(std::index_sequence<Indices...>, ArgTypes&&... Args)
	{
		(new (Bytes + Offsets[Indices]) TElement<Indices>(Forward<ArgTypes>(Args)), ...);
	}

	template <size_t... Indices>
	void Destruct(std::index_sequence<Indices...>)
	{
		(std::destroy_at(&Get<Indices>()), ...);
	}

public:
	template <typename... ArgTypes>
	explicit TFlatParms(ArgTypes&&... Args)
	{
		Construct(std::index_sequence_for<Types...>(), Forward<ArgTypes>(Args)...);
	}

	TFlatParms(const TFlatParms&) = delete;
	TFlatParms& operator=(const TFlatParms&) = delete;

	~TFlatParms()
	{
		Destruct(std::index_sequence_for<Types...>());
	}

	template <size_t Index>
	TElement<Index>& Get()
	{
		return *reinterpret_cast<TElement<Index>*>(Bytes + Offsets[Index]);
	}

private:
	alignas(MaxAlign()) uint8 Bytes[Offsets[sizeof...(Types)] ? Offsets[sizeof...(Types)] : 1];
};

template <typename FuncType>
struct TPayload;

/** Parameter block handed to ProcessEvent for UFunction bindings: params, payload, then the return value */
template <typename Ret, typename... Types>
struct TPayload<Ret(Types...)>
{
	TFlatParms<Types..., Ret> Values;

	template <typename... ArgTypes>
	explicit TPayload(ArgTypes&&... Args)
		: Values(Forward<ArgTypes>(Args)..., Ret())
	{
	}

	Ret& GetResult()
	{
		return Values.template Get<sizeof...(Types)>();
	}
};

template <typename... Types>
struct TPayload<void(Types...)>
{
	TFlatParms<Types...> Values;

	template <typename... ArgTypes>
	explicit TPayload(ArgTypes&&... Args)
		: Values(Forward<ArgTypes>(Args)...)
	{
	}

	void GetResult()
	{
	}
};