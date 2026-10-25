#pragma once
#include "CoreMinimal.h"
#include "Engine/Plugins/Online/Experimental/OnlineServices/Source/OnlineServicesInterface/Public/Online/OnlineError.h"
#include "Engine/Plugins/Online/OnlineSubsystem/Source/Public/OnlineNotification.h"
class FMcpQueryResult : public FOnlineError
{
public:
	FOnlineNotification PrimaryNotification;
	int HttpResult;
	int NumericErrorCode;
	TSharedPtr<FJsonValue> JsonResult;
	bool bHasBeenHandled;
};