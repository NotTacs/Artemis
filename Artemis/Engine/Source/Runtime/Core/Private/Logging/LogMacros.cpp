#include "pch.h"
#include "Engine/Source/Runtime/Core/Public/Logging/LogMacros.h"



void FMsg::Logf_Internal(const char* File, int32 Line, const FName& Category, ELogVerbosity::Type Verbosity, const TCHAR* Fmt, ...)
{
	static void (*NativeLogf_Internal)(const char* File, int32 Line, const FName & Category, ELogVerbosity::Type Verbosity, const TCHAR * Fmt, ...) = decltype(NativeLogf_Internal)(InSDKUtils::GetImageBase() + 0xD34D3C);

	va_list Args;
	va_start(Args, Fmt);
	const int Length = _vscwprintf(Fmt, Args);
	va_end(Args);
	if (Length < 0)
		return;

	std::vector<TCHAR> Message(size_t(Length) + 1);
	va_start(Args, Fmt);
	vswprintf_s(Message.data(), Message.size(), Fmt, Args);
	va_end(Args);

	const ELogVerbosity::Type Level = ELogVerbosity::Type(Verbosity & ELogVerbosity::VerbosityMask);
	if (Level == ELogVerbosity::Log)
		printf("%s: %ls\n", Category.ToString().c_str(), Message.data());
	else
		printf("%s: %ls: %ls\n", Category.ToString().c_str(), ToString(Level), Message.data());

	NativeLogf_Internal(File, Line, Category, Verbosity, TEXT("%s"), Message.data());
}
