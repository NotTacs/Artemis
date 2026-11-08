// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <new>

/** FProperty::ContainerPtrToValuePtr, as a free function because FProperty lives in the SDK. */
template<typename ValueType>
FORCEINLINE ValueType* ContainerPtrToValuePtr(const FProperty* Property, void* ContainerPtr, int32 ArrayIndex = 0)
{
	return (ValueType*)((uint8*)ContainerPtr + Property->Offset + Property->ElementSize * ArrayIndex);
}

template<typename ValueType>
FORCEINLINE const ValueType* ContainerPtrToValuePtr(const FProperty* Property, const void* ContainerPtr, int32 ArrayIndex = 0)
{
	return (const ValueType*)((const uint8*)ContainerPtr + Property->Offset + Property->ElementSize * ArrayIndex);
}

/*-----------------------------------------------------------------------------
	TPropertyTypeFundamentals.
-----------------------------------------------------------------------------*/

template<typename InTCppType>
class TPropertyTypeFundamentals
{
public:
	/** Type of the CPP property **/
	typedef InTCppType TCppType;
	enum
	{
		CPPSize = sizeof(TCppType),
		CPPAlignment = alignof(TCppType)
	};

	/** Convert the address of a value of the property to the proper type */
	static FORCEINLINE TCppType const* GetPropertyValuePtr(void const* A)
	{
		return (TCppType const*)A;
	}
	/** Convert the address of a value of the property to the proper type */
	static FORCEINLINE TCppType* GetPropertyValuePtr(void* A)
	{
		return (TCppType*)A;
	}
	/** Get the value of the property from an address */
	static FORCEINLINE TCppType const& GetPropertyValue(void const* A)
	{
		return *GetPropertyValuePtr(A);
	}
	/** Get the default value of the cpp type, just the default constructor, which works even for things like in32 */
	static FORCEINLINE TCppType GetDefaultPropertyValue()
	{
		return TCppType();
	}
	/** Get the value of the property from an address, unless it is NULL, then return the default value */
	static FORCEINLINE TCppType GetOptionalPropertyValue(void const* B)
	{
		return B ? GetPropertyValue(B) : GetDefaultPropertyValue();
	}
	/** Set the value of a property at an address */
	static FORCEINLINE void SetPropertyValue(void* A, TCppType const& Value)
	{
		*GetPropertyValuePtr(A) = Value;
	}
	/** Initialize the value of a property at an address, this assumes over uninitialized memory */
	static FORCEINLINE TCppType* InitializePropertyValue(void* A)
	{
		return new (A) TCppType();
	}
	/** Destroy the value of a property at an address */
	static FORCEINLINE void DestroyPropertyValue(void* A)
	{
		GetPropertyValuePtr(A)->~TCppType();
	}
};

/*-----------------------------------------------------------------------------
	TProperty.
-----------------------------------------------------------------------------*/

template<typename InTCppType, class TInPropertyBaseClass>
class TProperty : public TInPropertyBaseClass, public TPropertyTypeFundamentals<InTCppType>
{
public:
	typedef InTCppType TCppType;
	typedef TInPropertyBaseClass Super;
	typedef TPropertyTypeFundamentals<InTCppType> TTypeFundamentals;

	TProperty() = delete; // owned by the game

	/** Convert the address of a container to the address of the property value, in the proper type */
	FORCEINLINE TCppType const* GetPropertyValuePtr_InContainer(void const* A, int32 ArrayIndex = 0) const
	{
		return TTypeFundamentals::GetPropertyValuePtr(ContainerPtrToValuePtr<void>(this, A, ArrayIndex));
	}
	/** Convert the address of a container to the address of the property value, in the proper type */
	FORCEINLINE TCppType* GetPropertyValuePtr_InContainer(void* A, int32 ArrayIndex = 0) const
	{
		return TTypeFundamentals::GetPropertyValuePtr(ContainerPtrToValuePtr<void>(this, A, ArrayIndex));
	}
	/** Get the value of the property from a container address */
	FORCEINLINE TCppType const& GetPropertyValue_InContainer(void const* A, int32 ArrayIndex = 0) const
	{
		return *GetPropertyValuePtr_InContainer(A, ArrayIndex);
	}
	/** Get the value of the property from a container address, unless it is NULL, then return the default value */
	FORCEINLINE TCppType GetOptionalPropertyValue_InContainer(void const* B, int32 ArrayIndex = 0) const
	{
		return B ? GetPropertyValue_InContainer(B, ArrayIndex) : TTypeFundamentals::GetDefaultPropertyValue();
	}
	/** Set the value of a property in a container */
	FORCEINLINE void SetPropertyValue_InContainer(void* A, TCppType const& Value, int32 ArrayIndex = 0) const
	{
		*GetPropertyValuePtr_InContainer(A, ArrayIndex) = Value;
	}
};

/*-----------------------------------------------------------------------------
	Numeric properties.
-----------------------------------------------------------------------------*/

class FNumericProperty : public FProperty
{
public:
	FNumericProperty() = delete;
};

template<typename InTCppType>
class TProperty_Numeric : public TProperty<InTCppType, FNumericProperty>
{
public:
	typedef InTCppType TCppType;
};

class FInt8Property   : public TProperty_Numeric<int8>   {};
class FInt16Property  : public TProperty_Numeric<int16>  {};
class FIntProperty    : public TProperty_Numeric<int32>  {};
class FInt64Property  : public TProperty_Numeric<int64>  {};
class FUInt16Property : public TProperty_Numeric<uint16> {};
class FUInt32Property : public TProperty_Numeric<uint32> {};
class FUInt64Property : public TProperty_Numeric<uint64> {};
class FFloatProperty  : public TProperty_Numeric<float>  {};
class FDoubleProperty : public TProperty_Numeric<double> {};
class FLargeWorldCoordinatesRealProperty : public FDoubleProperty {};

/*-----------------------------------------------------------------------------
	Name / string / text.
-----------------------------------------------------------------------------*/

class FNameProperty : public TProperty<FName, FProperty>   {};
class FStrProperty  : public TProperty<FString, FProperty> {};
class FTextProperty : public TProperty<FText, FProperty>   {};

/*-----------------------------------------------------------------------------
	Object references (FObjectPropertyBase / FClassProperty are in the SDK).
-----------------------------------------------------------------------------*/

class FObjectProperty     : public TProperty<UObject*, FObjectPropertyBase>       {};
class FObjectPtrProperty  : public FObjectProperty                                  {};
class FWeakObjectProperty : public TProperty<FWeakObjectPtr, FObjectPropertyBase> {};
class FSoftObjectProperty : public TProperty<FSoftObjectPtr, FObjectPropertyBase> {};

class FSoftClassProperty : public FSoftObjectProperty
{
public:
	class UClass* MetaClass; // 0x0080
};

class FInterfaceProperty : public TProperty<FScriptInterface, FProperty>
{
public:
	class UClass* InterfaceClass; // 0x0078
};

/*-----------------------------------------------------------------------------
	Multicast delegates (FDelegateProperty is in the SDK).
-----------------------------------------------------------------------------*/

class FMulticastDelegateProperty : public FProperty
{
public:
	FMulticastDelegateProperty() = delete;
	class UFunction* SignatureFunction; // 0x0078
};

class FMulticastInlineDelegateProperty : public FMulticastDelegateProperty {};
class FMulticastSparseDelegateProperty : public FMulticastDelegateProperty {};

/*-----------------------------------------------------------------------------
	Layout checks against the 19.10 engine.
-----------------------------------------------------------------------------*/

static_assert(sizeof(FProperty) == 0x78, "FProperty size changed");
static_assert(sizeof(FIntProperty) == 0x78 && sizeof(FDoubleProperty) == 0x78, "numeric properties add no members");
static_assert(sizeof(FNameProperty) == 0x78 && sizeof(FStrProperty) == 0x78 && sizeof(FTextProperty) == 0x78);
static_assert(sizeof(FObjectProperty) == 0x80 && sizeof(FWeakObjectProperty) == 0x80 && sizeof(FSoftObjectProperty) == 0x80);
static_assert(sizeof(FSoftClassProperty) == 0x88);
static_assert(sizeof(FInterfaceProperty) == 0x80);
static_assert(sizeof(FMulticastDelegateProperty) == 0x80);
