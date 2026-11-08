#pragma once

#include "CoreMinimal.h"
#include "Script.h"
#include "../../../Core/Public/Misc/OutputDevice.h"
#include <cstring>

DECLARE_LOG_CATEGORY_EXTERN(LogScriptFrame, Warning, All);

/**
 * Property data type enums.
 * @warning: if values in this enum are modified, you must update:
 * - FPropertyBase::GetSize() hardcodes the sizes for each property type
 */
enum EPropertyType
{
	CPT_None,
	CPT_Byte,
	CPT_UInt16,
	CPT_UInt32,
	CPT_UInt64,
	CPT_Int8,
	CPT_Int16,
	CPT_Int,
	CPT_Int64,
	CPT_Bool,
	CPT_Bool8,
	CPT_Bool16,
	CPT_Bool32,
	CPT_Bool64,
	CPT_Float,
	CPT_ObjectReference,
	CPT_Name,
	CPT_Delegate,
	CPT_Interface,
	CPT_Unused_Index_19,
	CPT_Struct,
	CPT_Unused_Index_21,
	CPT_Unused_Index_22,
	CPT_String,
	CPT_Text,
	CPT_MulticastDelegate,
	CPT_WeakObjectReference,
	CPT_LazyObjectReference,
	CPT_ObjectPtrReference,
	CPT_SoftObjectReference,
	CPT_Double,
	CPT_Map,
	CPT_Set,
	CPT_FieldPath,
	CPT_FLargeWorldCoordinatesReal,

	CPT_MAX
};

/** The engine's native thunk signature (UFunction::Func / GNatives entries). */
typedef void (*FNativeFuncPtr)(UObject* Context, FFrame& TheStack, RESULT_DECL);

/** The engine's opcode table (GNatives[EX_Max]), indexed by EExprToken. */
FORCEINLINE FNativeFuncPtr* GetGNatives()
{
	return reinterpret_cast<FNativeFuncPtr*>(InSDKUtils::GetImageBase() + 0xB30DDE0);
}

/*-----------------------------------------------------------------------------
	Execution stack helpers.
-----------------------------------------------------------------------------*/

/**
 * TArray< CodeSkipSizeType, TInlineAllocator<8> > in the engine. The SDK's TArray has no allocator parameter,
 * so this is the same layout spelled out, with the subset of the TArray API the VM uses on FlowStack.
 */
struct FlowStackType
{
	using ElementType = CodeSkipSizeType;

	UC::ContainerImpl::TInlineAllocator<8>::ForElementType<ElementType> AllocatorInstance;
	int32 ArrayNum = 0;
	int32 ArrayMax = 8;

	FORCEINLINE int32 Num() const { return ArrayNum; }
	FORCEINLINE int32 Max() const { return ArrayMax; }
	FORCEINLINE bool IsValidIndex(int32 Index) const { return Index >= 0 && Index < ArrayNum; }

	FORCEINLINE       ElementType* GetData()       { return AllocatorInstance.GetAllocation(); }
	FORCEINLINE const ElementType* GetData() const { return AllocatorInstance.GetAllocation(); }

	FORCEINLINE       ElementType& operator[](int32 Index)       { return GetData()[Index]; }
	FORCEINLINE const ElementType& operator[](int32 Index) const { return GetData()[Index]; }

	FORCEINLINE       ElementType& Top()       { return GetData()[ArrayNum - 1]; }
	FORCEINLINE const ElementType& Top() const { return GetData()[ArrayNum - 1]; }

	void Push(ElementType Item)
	{
		if (ArrayNum == ArrayMax)
		{
			const int32 NewMax = ArrayMax * 2;
			AllocatorInstance.ResizeAllocation(ArrayNum, NewMax);
			ArrayMax = NewMax;
		}
		GetData()[ArrayNum++] = Item;
	}

	ElementType Pop()
	{
		return GetData()[--ArrayNum];
	}

	void Reset() { ArrayNum = 0; }
};
static_assert(sizeof(FlowStackType) == 0x30, "FlowStackType must match TArray<uint32, TInlineAllocator<8>>");

//
// Information remembered about an Out parameter.
//
struct FOutParmRec
{
	FProperty* Property;
	uint8*      PropAddr;
	FOutParmRec* NextOutParm;
};

/** Bytecode representation of an FName (WITH_CASE_PRESERVING_NAME is off in the game, so DisplayIndex == ComparisonIndex). */
struct FScriptName
{
	FNameEntryId ComparisonIndex;
	FNameEntryId DisplayIndex;
	uint32       Number;
};

FORCEINLINE FName ScriptNameToName(const FScriptName& InName)
{
	return FName(InName.ComparisonIndex, InName.Number);
}

//
// Information about script execution at one stack level.
//

struct FFrame : public FOutputDevice
{
public:
	// Variables.
	UFunction* Node;
	UObject* Object;
	uint8* Code;
	uint8* Locals;

	FProperty* MostRecentProperty;
	uint8* MostRecentPropertyAddress;

	/** The execution flow stack for compiled Kismet code */
	FlowStackType FlowStack;

	/** Previous frame on the stack */
	FFrame* PreviousFrame;

	/** contains information on any out parameters */
	FOutParmRec* OutParms;

	/** If a class is compiled in then this is set to the property chain for compiled-in functions. In that case, we follow the links to setup the args instead of executing by code. */
	FField* PropertyChainForCompiledIn;

	/** Currently executed native function */
	UFunction* CurrentNativeFunction;

	bool bArrayContextFailed;

public:

	// Constructors.
	FFrame( UObject* InObject, UFunction* InNode, void* InLocals, FFrame* InPreviousFrame = NULL, FField* InPropertyChainForCompiledIn = NULL );

	virtual ~FFrame()
	{
	}

	// Functions.
	void Step(UObject* Context, RESULT_DECL);

	/** Convenience function that calls Step, but also returns true if both MostRecentProperty and MostRecentPropertyAddress are non-null. */
	FORCEINLINE bool StepAndCheckMostRecentProperty(UObject* Context, RESULT_DECL);

	/** Replacement for Step that uses an explicitly specified property to unpack arguments **/
	void StepExplicitProperty(void*const Result, FProperty* Property);

	/** Replacement for Step that checks the for byte code, and if none exists, then PropertyChainForCompiledIn is used. Also, makes an effort to verify that the params are in the correct order and the types are compatible. **/
	template<class TProperty>
	FORCEINLINE void StepCompiledIn(void* Result);
	FORCEINLINE void StepCompiledIn(void* Result, const FFieldClass* ExpectedPropertyType);

	/** Replacement for Step that checks the for byte code, and if none exists, then PropertyChainForCompiledIn is used. Also, makes an effort to verify that the params are in the correct order and the types are compatible. **/
	template<class TProperty, typename TNativeType>
	FORCEINLINE TNativeType& StepCompiledInRef(void*const TemporaryBuffer);

	virtual void Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category ) override;

	static void KismetExecutionMessage(const TCHAR* Message, ELogVerbosity::Type Verbosity, FName WarningId = FName());

	/** Returns the current script op code */
	const uint8 PeekCode() const { return *Code; }

	/** Skips over the number of op codes specified by NumOps */
	void SkipCode(const int32 NumOps) { Code += NumOps; }

	template<typename T>
	T Read();
	template<typename TNumericType>
	TNumericType ReadInt();
	float ReadFloat();
	double ReadDouble();
	FName ReadName();
	UObject* ReadObject();
	int32 ReadWord();
	FProperty* ReadProperty();

	/** May return null */
	FProperty* ReadPropertyUnchecked();

	/**
	 * Reads a value from the bytestream, which represents the number of bytes to advance
	 * the code pointer for certain expressions.
	 */
	CodeSkipSizeType ReadCodeSkipCount();

	/**
	 * Reads a value from the bytestream which represents the number of bytes that should be zero'd out if a NULL context
	 * is encountered
	 *
	 * @param	ExpressionField		receives a pointer to the field representing the expression; used by various execs
	 *								to drive VM logic
	 */
	VariableSizeType ReadVariableSize(FProperty** ExpressionField);

	/**
	 * This will return the StackTrace of the current callstack from the last native entry point
	 * (walks PreviousFrame; the engine walks FBlueprintContextTracker, which the game does not keep in shipping).
	 **/
	FAllocatedString GetStackTrace() const; // FString in the engine; owning string here (SDK FString(const TCHAR*) does not copy)

	/**
	 * This will return a string of the form "ScopeName.FunctionName" associated with this stack frame:
	 */
	FAllocatedString GetStackDescription() const; // FString in the engine
};

static_assert(sizeof(FFrame) == 0x98, "FFrame must match the 19.10 engine layout");
static_assert(offsetof(FFrame, Node) == 0x10);
static_assert(offsetof(FFrame, Code) == 0x20);
static_assert(offsetof(FFrame, MostRecentPropertyAddress) == 0x38);
static_assert(offsetof(FFrame, FlowStack) == 0x40);
static_assert(offsetof(FFrame, PreviousFrame) == 0x70);
static_assert(offsetof(FFrame, OutParms) == 0x78);
static_assert(offsetof(FFrame, PropertyChainForCompiledIn) == 0x80);
static_assert(offsetof(FFrame, CurrentNativeFunction) == 0x88);
static_assert(offsetof(FFrame, bArrayContextFailed) == 0x90);


/*-----------------------------------------------------------------------------
	FFrame implementation.
-----------------------------------------------------------------------------*/

inline FFrame::FFrame( UObject* InObject, UFunction* InNode, void* InLocals, FFrame* InPreviousFrame, FField* InPropertyChainForCompiledIn )
	: Node(InNode)
	, Object(InObject)
	, Code(const_cast<uint8*>(InNode->Script.GetDataPtr()))
	, Locals((uint8*)InLocals)
	, MostRecentProperty(NULL)
	, MostRecentPropertyAddress(NULL)
	, PreviousFrame(InPreviousFrame)
	, OutParms(NULL)
	, PropertyChainForCompiledIn(InPropertyChainForCompiledIn)
	, CurrentNativeFunction(NULL)
	, bArrayContextFailed(false)
{
}

inline void FFrame::Step(UObject* Context, RESULT_DECL)
{
	// Engine: int32 B = *Code++; (GNatives[B])(Context, *this, RESULT_PARAM);
	static void (*StepImpl)(FFrame*, UObject*, void*) = decltype(StepImpl)(InSDKUtils::GetImageBase() + 0xCCB6B8);
	StepImpl(this, Context, RESULT_PARAM);
}

inline void FFrame::StepExplicitProperty(void*const Result, FProperty* Property)
{
	static void (*StepExplicitPropertyImpl)(FFrame*, void*, FProperty*) = decltype(StepExplicitPropertyImpl)(InSDKUtils::GetImageBase() + 0xCC9C90);
	StepExplicitPropertyImpl(this, Result, Property);
}

template<typename T>
inline T FFrame::Read()
{
	T Result;
	memcpy(&Result, Code, sizeof(T)); // FPlatformMemory::ReadUnaligned
	Code += sizeof(T);
	return Result;
}

template<typename TNumericType>
inline TNumericType FFrame::ReadInt()
{
	return Read<TNumericType>();
}

inline UObject* FFrame::ReadObject()
{
	// we always pull 64-bits of data out, which is really a UObject* in some representation (depending on platform)
	ScriptPointerType TempCode;
	memcpy(&TempCode, Code, sizeof(ScriptPointerType));

	// turn that uint32 into a UObject pointer
	UObject* Result = (UObject*)(TempCode);
	Code += sizeof(ScriptPointerType);
	return Result;
}

inline FProperty* FFrame::ReadProperty()
{
	FProperty* Result = (FProperty*)ReadObject();
	MostRecentProperty = Result;

	// Callers don't check for NULL; this method is expected to succeed.
	return Result;
}

inline FProperty* FFrame::ReadPropertyUnchecked()
{
	FProperty* Result = (FProperty*)ReadObject();
	MostRecentProperty = Result;
	return Result;
}

inline float FFrame::ReadFloat()
{
	return Read<float>();
}

inline double FFrame::ReadDouble()
{
	return Read<double>();
}

inline int32 FFrame::ReadWord()
{
	return Read<uint16>();
}

/**
 * Reads a value from the bytestream, which represents the number of bytes to advance
 * the code pointer for certain expressions.
 */
inline CodeSkipSizeType FFrame::ReadCodeSkipCount()
{
	return Read<CodeSkipSizeType>();
}

inline VariableSizeType FFrame::ReadVariableSize( FProperty** ExpressionField )
{
	VariableSizeType Result=0;

	FField* Field = (FField*)ReadObject(); // Is it safe to assume it's an FField?
	// CastField<FProperty>(Field)
	FProperty* Property = (Field && Field->ClassPrivate && (Field->ClassPrivate->CastFlags & uint64(EClassCastFlags::Property))) ? (FProperty*)Field : nullptr;
	if (Property)
	{
		Result = (VariableSizeType)(Property->ArrayDim * Property->ElementSize); // FProperty::GetSize()
	}

	if (ExpressionField != nullptr)
	{
		*ExpressionField = Property;
	}

	return Result;
}

inline FName FFrame::ReadName()
{
	return ScriptNameToName(Read<FScriptName>());
}

FORCEINLINE bool FFrame::StepAndCheckMostRecentProperty(UObject* Context, RESULT_DECL)
{
	Step(Context, RESULT_PARAM);

	return (MostRecentProperty && MostRecentPropertyAddress);
}

/**
 * Replacement for Step that checks the for byte code, and if none exists, then PropertyChainForCompiledIn is used.
 * Also makes an effort to verify that the params are in the correct order and the types are compatible.
 **/
template<class TProperty>
FORCEINLINE void FFrame::StepCompiledIn(void* Result)
{
	// The engine passes TProperty::StaticClass() here, but only uses it in checkSlow (compiled out in the game).
	StepCompiledIn(Result, nullptr);
}

FORCEINLINE void FFrame::StepCompiledIn(void* Result, const FFieldClass* ExpectedPropertyType)
{
	if (Code)
	{
		Step(Object, Result);
	}
	else
	{
		FProperty* Property = (FProperty*)PropertyChainForCompiledIn;
		PropertyChainForCompiledIn = Property->Next;
		StepExplicitProperty(Result, Property);
	}
}

template<class TProperty, typename TNativeType>
FORCEINLINE TNativeType& FFrame::StepCompiledInRef(void*const TemporaryBuffer)
{
	MostRecentPropertyAddress = NULL;

	if (Code)
	{
		Step(Object, TemporaryBuffer);
	}
	else
	{
		TProperty* Property = (TProperty*)PropertyChainForCompiledIn;
		PropertyChainForCompiledIn = Property->Next;
		StepExplicitProperty(TemporaryBuffer, Property);
	}

	return (MostRecentPropertyAddress != NULL) ? *(TNativeType*)(MostRecentPropertyAddress) : *(TNativeType*)TemporaryBuffer;
}
