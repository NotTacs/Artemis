#include "pch.h"
#include "../Public/McpProfileGroup.h"
#include "Engine/Source/Runtime/Engine/Classes/Kismet/MemoryLibrary.h"

void McpProfileGroup::SendRequestNow(UMcpProfileGroup* __this, __int64 HttpRequest, FBaseUrlContext::EContextCredentials ContextCredentials)
{
	Originals::SendRequestNow(__this, HttpRequest, FBaseUrlContext::CXC_Public);
}

void McpProfileGroup::LoadModule()
{
	FMemoryLibrary::DetourHook(0x1674270, SendRequestNow, Originals::SendRequestNow);
}