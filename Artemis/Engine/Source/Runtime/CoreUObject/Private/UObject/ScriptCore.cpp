#include "pch.h"
#include "../../Public/UObject/Stack.h"
#include <string>

DEFINE_LOG_CATEGORY(LogScriptFrame);

namespace
{
	std::wstring Widen(const std::string& Str)
	{
		return std::wstring(Str.begin(), Str.end());
	}
}

void FFrame::Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category )
{
	const std::wstring ObjectName = Object->GetFullNameW();
	const std::wstring NodeName = Node->GetFullNameW();;
	const long long CodeOffset = (Node && Code) ? (long long)(Code - Node->Script.GetDataPtr()) : -1;
	const FAllocatedString StackTrace = GetStackTrace();

	const std::wstring Message = std::wstring(V ? V : L"") + L"\r\n\t" + ObjectName + L"\r\n\t" + NodeName + L":" + std::to_wstring(CodeOffset) + L"\r\n\t" + *StackTrace;
	KismetExecutionMessage(Message.c_str(), Verbosity);
}

void FFrame::KismetExecutionMessage(const TCHAR* Message, ELogVerbosity::Type Verbosity, FName WarningId)
{
	switch (Verbosity)
	{
	case ELogVerbosity::Fatal:
	case ELogVerbosity::Error:
		UE_LOG(LogScriptFrame, Error, L"Script Msg: %s", Message);
		break;
	case ELogVerbosity::Warning:
		UE_LOG(LogScriptFrame, Warning, L"Script Msg: %s", Message);
		break;
	default:
		UE_LOG(LogScriptFrame, Log, L"Script Msg: %s", Message);
		break;
	}
}

FAllocatedString FFrame::GetStackTrace() const
{
	std::wstring Result = L"Script call stack:\n";

	const FFrame* Frames[256];
	int32 NumFrames = 0;
	for (const FFrame* Frame = this; Frame && NumFrames < 256; Frame = Frame->PreviousFrame)
	{
		Frames[NumFrames++] = Frame;
	}

	for (int32 Index = NumFrames - 1; Index >= 0; --Index)
	{
		Result += L"\t";
		Result += *Frames[Index]->GetStackDescription();
		Result += L"\n";
	}

	return FAllocatedString(Result.c_str());
}

FAllocatedString FFrame::GetStackDescription() const
{
	if (!Node)
	{
		return FAllocatedString(L"None");
	}

	std::wstring Result = Node->Outer ? std::wstring(*Node->Outer->GetName()) : std::wstring(L"None");
	Result += L".";
	Result += *Node->GetName();
	return FAllocatedString(Result.c_str());
}
