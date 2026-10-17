#include "pch.h"
#include "../../Public/Dom/JsonValue.h"
#include "../../../Core/Public/Templates/SharedPointer.h"

double FJsonValue::AsNumber() const
{
	static double (*AsNumberInternal)(const FJsonValue*) = decltype(AsNumberInternal)(InSDKUtils::GetImageBase() + 0xd2ed04);
	return AsNumberInternal(this);
}


FAllocatedString FJsonValue::AsString() const
{
	static FAllocatedString (*AsStringInternal)(const FJsonValue*) = decltype(AsStringInternal)(InSDKUtils::GetImageBase() + 0xd302a8);
	FAllocatedString RawString(1024);
	RawString = AsStringInternal(this);
	return RawString;
}


bool FJsonValue::AsBool() const
{
	static bool (*AsBoolInternal)(const FJsonValue*) = decltype(AsBoolInternal)(InSDKUtils::GetImageBase() + 0xd2e57c);
	return AsBoolInternal(this);
}


const TArray< TSharedPtr<FJsonValue> >& FJsonValue::AsArray() const
{
	static const TArray< TSharedPtr<FJsonValue> >& (*AsArrayInternal)(const FJsonValue*) = decltype(AsArrayInternal)(InSDKUtils::GetImageBase() + 0x13710e8);
	return AsArrayInternal(this);
}


const TSharedPtr<FJsonObject>& FJsonValue::AsObject() const
{
	static TSharedPtr<FJsonObject>& (*AsObjectInternal)(const FJsonValue*) = decltype(AsObjectInternal)(InSDKUtils::GetImageBase() + 0x128330c);
	return AsObjectInternal(this);
}