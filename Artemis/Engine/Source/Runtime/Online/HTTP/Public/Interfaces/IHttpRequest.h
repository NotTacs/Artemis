#pragma once
#include "CoreMinimal.h"
#include "../../../../Core/Public/Templates/SharedPointer.h"

class IHttpBase
{
	void** VTable;
};

class IHttpRequest;

namespace EHttpRequestStatus
{
	/**
	 * Enumerates the current state of an Http request
	 */
	enum Type
	{
		/** Has not been started via ProcessRequest() */
		NotStarted,
		/** Currently being ticked and processed */
		Processing,
		/** Finished but failed */
		Failed,
		/** Failed because it was unable to connect (safe to retry) */
		Failed_ConnectionError,
		/** Finished and was successful */
		Succeeded
	};

	/** @return the stringified version of the enum passed in */
	inline const TCHAR* ToString(EHttpRequestStatus::Type EnumVal)
	{
		switch (EnumVal)
		{
		case NotStarted:
		{
			return TEXT("NotStarted");
		}
		case Processing:
		{
			return TEXT("Processing");
		}
		case Failed:
		{
			return TEXT("Failed");
		}
		case Failed_ConnectionError:
		{
			return TEXT("ConnectionError");
		}
		case Succeeded:
		{
			return TEXT("Succeeded");
		}
		}
		return TEXT("");
	}

	inline bool IsFinished(const EHttpRequestStatus::Type Value)
	{
		return Value == Failed || Value == Failed_ConnectionError || Value == Succeeded;
	}
}

class IHttpRequest;

typedef TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> FHttpRequestPtr;
typedef TSharedRef<IHttpRequest, ESPMode::ThreadSafe> FHttpRequestRef;

class IHttpRequest :
	public IHttpBase, public TSharedFromThis<IHttpRequest, ESPMode::ThreadSafe>
{
public:

};