#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"

class McpProfileGroup
{
	class Originals
	{
	public:
		static inline void (*SendRequestNow)(UMcpProfileGroup* __this, const FHttpRequestRef& HttpRequest, FBaseUrlContext::EContextCredentials ContextCredentials);
	};
public:
	static void SendRequestNow(UMcpProfileGroup* __this, const FHttpRequestRef& HttpRequest, FBaseUrlContext::EContextCredentials ContextCredentials);
	static void LoadModule();
};