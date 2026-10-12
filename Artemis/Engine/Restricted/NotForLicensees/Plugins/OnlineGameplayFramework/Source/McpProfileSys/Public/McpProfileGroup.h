#pragma once
#include "CoreMinimal.h"

class McpProfileGroup
{
	class Originals
	{
	public:
		static inline void (*SendRequestNow)(UMcpProfileGroup* __this, __int64 HttpRequest, FBaseUrlContext::EContextCredentials ContextCredentials);
	};
public:
	static void SendRequestNow(UMcpProfileGroup* __this, __int64 HttpRequest, FBaseUrlContext::EContextCredentials ContextCredentials);
	static void LoadModule();
};