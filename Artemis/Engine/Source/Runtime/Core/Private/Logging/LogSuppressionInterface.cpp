#include "pch.h"
#include "Engine/Source/Runtime/Core/Public/Logging/LogCategory.h"

FLogCategoryBase::FLogCategoryBase(const TCHAR* CategoryName, ELogVerbosity::Type InDefaultVerbosity, ELogVerbosity::Type InCompileTimeVerbosity)
	: DefaultVerbosity(InDefaultVerbosity)
	, CompileTimeVerbosity(InCompileTimeVerbosity)
	, CategoryNameString(CategoryName)
{
	ResetFromDefault();
}

const FName& FLogCategoryBase::GetCategoryName() const
{
	if (!bCategoryFNameResolved)
	{
		CategoryFName = FName(CategoryNameString, EFindName::FNAME_Add);
		bCategoryFNameResolved = true;
	}
	return CategoryFName;
}

void FLogCategoryBase::SetVerbosity(ELogVerbosity::Type NewVerbosity)
{
	Verbosity = ELogVerbosity::Type(min(CompileTimeVerbosity, NewVerbosity & ELogVerbosity::VerbosityMask));
	DebugBreakOnLog = !!(NewVerbosity & ELogVerbosity::BreakOnLog);
}

void FLogCategoryBase::ResetFromDefault()
{
	SetVerbosity(ELogVerbosity::Type(DefaultVerbosity));
}

void FLogCategoryBase::PostTrigger(ELogVerbosity::Type VerbosityLevel)
{
	if (DebugBreakOnLog)
	{
		DebugBreakOnLog = false;
		if (IsDebuggerPresent())
			__debugbreak();
	}
}