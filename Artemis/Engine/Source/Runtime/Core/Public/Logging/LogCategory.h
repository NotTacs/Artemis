#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Core/Public/Logging/LogVerbosity.h"

/** Base class for all log categories. */
struct FLogCategoryBase
{
	FLogCategoryBase(const TCHAR* CategoryName, ELogVerbosity::Type InDefaultVerbosity, ELogVerbosity::Type InCompileTimeVerbosity);

	/** Should not generally be used directly. Tests the runtime verbosity and maybe triggers a debug break, etc. */
	inline bool IsSuppressed(ELogVerbosity::Type VerbosityLevel) const
	{
		return !((VerbosityLevel & ELogVerbosity::VerbosityMask) <= Verbosity);
	}

	/** Called just after a logging statement being allow to print. Checks a few things and maybe breaks into the debugger. */
	void PostTrigger(ELogVerbosity::Type VerbosityLevel);

	/**
	 * The FName is resolved through the game's name table on first use instead of at construction:
	 * categories are static objects built while the DLL is loading, which can be before the engine's FName pool exists.
	 */
	const FName& GetCategoryName() const;

	/** Gets the working verbosity **/
	inline ELogVerbosity::Type GetVerbosity() const
	{
		return Verbosity;
	}

	/** Sets up the working verbosity and clamps to the compile time verbosity. **/
	void SetVerbosity(ELogVerbosity::Type Verbosity);

	/** Resets the working verbosity back to the default. **/
	void ResetFromDefault();

private:
	/** Holds the current suppression state **/
	ELogVerbosity::Type Verbosity;
	/** Holds the break flag **/
	bool DebugBreakOnLog;
	/** Holds default suppression **/
	uint8 DefaultVerbosity;
	/** Holds compile time suppression **/
	uint8 CompileTimeVerbosity;
	/** Name for this category **/
	const TCHAR* CategoryNameString;
	mutable FName CategoryFName{};
	mutable bool bCategoryFNameResolved = false;
};

/** Template for log categories that transfers the compile-time constant default and compile time verbosity to the FLogCategoryBase constructor. */
template <ELogVerbosity::Type InDefaultVerbosity, ELogVerbosity::Type InCompileTimeVerbosity>
struct FLogCategory : public FLogCategoryBase
{
	static_assert((InDefaultVerbosity& ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity, "Bogus default verbosity.");
	static_assert(InCompileTimeVerbosity < ELogVerbosity::NumVerbosity, "Bogus compile time verbosity.");
	static constexpr ELogVerbosity::Type CompileTimeVerbosity = InCompileTimeVerbosity;

	inline FLogCategory(const TCHAR* InCategoryName)
		: FLogCategoryBase(InCategoryName, InDefaultVerbosity, CompileTimeVerbosity)
	{
	}
};