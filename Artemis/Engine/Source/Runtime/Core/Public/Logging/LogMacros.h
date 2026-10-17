#pragma once
#include "CoreMinimal.h"
#include "Engine/Source/Runtime/Core/Public/Logging/LogCategory.h"
#include "Engine/Source/Runtime/Core/Public/Logging/LogVerbosity.h"

/*----------------------------------------------------------------------------
	Logging
----------------------------------------------------------------------------*/

#define COMPILED_IN_MINIMUM_VERBOSITY VeryVerbose

struct FMsg
{
	/** Sends a formatted message to the game's own log (FortniteGame.log, GLog / GWarn), the same path a native UE_LOG takes. */
	static void Logf_Internal(const char* File, int32 Line, const FName& Category, ELogVerbosity::Type Verbosity, const TCHAR* Fmt, ...);
};

/**
 * A predicate that returns true if the given logging category is active (logging) at a given verbosity level
 * @param CategoryName name of the logging category
 * @param Verbosity, verbosity level to test against
**/
#define UE_LOG_ACTIVE(CategoryName, Verbosity) \
	((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) <= ELogVerbosity::COMPILED_IN_MINIMUM_VERBOSITY && \
	 (ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) <= FLogCategory##CategoryName::CompileTimeVerbosity && \
	 !CategoryName.IsSuppressed(ELogVerbosity::Verbosity))

#define UE_GET_LOG_VERBOSITY(CategoryName) \
	CategoryName.GetVerbosity()

#define UE_SET_LOG_VERBOSITY(CategoryName, Verbosity) \
	CategoryName.SetVerbosity(ELogVerbosity::Verbosity);

/**
 * A macro that outputs a formatted message to log if a given logging category is active at a given verbosity level
 * @param CategoryName name of the logging category
 * @param Verbosity, verbosity level to test against
 * @param Format, format text
 **/
#define UE_LOG(CategoryName, Verbosity, Format, ...) \
	{ \
		if constexpr ((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) <= ELogVerbosity::COMPILED_IN_MINIMUM_VERBOSITY && \
			(ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) <= FLogCategory##CategoryName::CompileTimeVerbosity) \
		{ \
			if (!CategoryName.IsSuppressed(ELogVerbosity::Verbosity)) \
			{ \
				FMsg::Logf_Internal(__FILE__, __LINE__, CategoryName.GetCategoryName(), ELogVerbosity::Verbosity, Format, ##__VA_ARGS__); \
				CategoryName.PostTrigger(ELogVerbosity::Verbosity); \
			} \
		} \
	}

 /**
  * A macro that conditionally logs a formatted message if the log category is active at the requested verbosity level.
  * @param Condition		Condition that must be true for the message to be logged
  * @param CategoryName	Name of the logging category
  * @param Verbosity		Verbosity level to test against
  * @param Format		Format string literal in the style of printf
  */
#define UE_CLOG(Condition, CategoryName, Verbosity, Format, ...) \
	{ \
		if (Condition) \
		{ \
			UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__); \
		} \
	}

  /**
   * A macro to declare a logging category as a C++ "extern", usually declared in the header and paired with DEFINE_LOG_CATEGORY in the source. Accessible by all files that include the header.
   * @param CategoryName, category to declare
   * @param DefaultVerbosity, default run time verbosity
   * @param CompileTimeVerbosity, maximum verbosity to compile into the code
   **/
#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName, DefaultVerbosity, CompileTimeVerbosity) \
	extern struct FLogCategory##CategoryName : public FLogCategory<ELogVerbosity::DefaultVerbosity, ELogVerbosity::CompileTimeVerbosity> \
	{ \
		inline FLogCategory##CategoryName() : FLogCategory(TEXT(#CategoryName)) {} \
	} CategoryName;

   /**
	* A macro to define a logging category, usually paired with DECLARE_LOG_CATEGORY_EXTERN from the header.
	* @param CategoryName, category to define
   **/
#define DEFINE_LOG_CATEGORY(CategoryName) FLogCategory##CategoryName CategoryName;

   /**
	* A macro to define a logging category as a C++ "static". This should ONLY be declared in a source file. Only accessible in that single file.
	* @param CategoryName, category to declare
	* @param DefaultVerbosity, default run time verbosity
	* @param CompileTimeVerbosity, maximum verbosity to compile into the code
   **/
#define DEFINE_LOG_CATEGORY_STATIC(CategoryName, DefaultVerbosity, CompileTimeVerbosity) \
	static struct FLogCategory##CategoryName : public FLogCategory<ELogVerbosity::DefaultVerbosity, ELogVerbosity::CompileTimeVerbosity> \
	{ \
		inline FLogCategory##CategoryName() : FLogCategory(TEXT(#CategoryName)) {} \
	} CategoryName;

   /**
	* A macro to declare a logging category as a C++ "class static"
	* @param CategoryName, category to declare
	* @param DefaultVerbosity, default run time verbosity
	* @param CompileTimeVerbosity, maximum verbosity to compile into the code
   **/
#define DECLARE_LOG_CATEGORY_CLASS(CategoryName, DefaultVerbosity, CompileTimeVerbosity) \
	DEFINE_LOG_CATEGORY_STATIC(CategoryName, DefaultVerbosity, CompileTimeVerbosity)

   /**
	* A macro to define a logging category, usually paired with DECLARE_LOG_CATEGORY_CLASS from the header.
	* @param CategoryName, category to definee
   **/
#define DEFINE_LOG_CATEGORY_CLASS(Class, CategoryName) Class::FLogCategory##CategoryName Class::CategoryName;