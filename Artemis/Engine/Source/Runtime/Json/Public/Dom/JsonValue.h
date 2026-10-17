#pragma once
#include "CoreMinimal.h"
#include "../Serialization/JsonTypes.h"
#include "../../../Core/Public/Templates/SharedPointer.h"

class FJsonObject;

class FJsonValue
{
public:
	void* VTable;
	EJson Type;

public:
	/** Returns this value as a double, logging an error and returning zero if this is not an Json Number */
	double AsNumber() const;

	/** Returns this value as a string, logging an error and returning an empty string if not possible */
	FAllocatedString AsString() const;

	/** Returns this value as a boolean, logging an error and returning false if not possible */
	bool AsBool() const;

	/** Returns this value as an array, logging an error and returning an empty array reference if not possible */
	const TArray< TSharedPtr<FJsonValue> >& AsArray() const;

	/** Returns this value as an object, throwing an error if this is not an Json Object */
	virtual const TSharedPtr<FJsonObject>& AsObject() const;
};