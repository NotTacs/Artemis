#pragma once
#include "CoreMinimal.h"

namespace ELogVerbosity
{
	enum Type : uint8
	{
		/** Not used */
		NoLogging = 0,

		/** Always prints a fatal error to console (and log file) and crashes (even if logging is disabled) */
		Fatal,

		/** Prints an error to console (and log file) */
		Error,

		/** Prints a warning to console (and log file) */
		Warning,

		/** Prints a message to console (and log file) */
		Display,

		/** Prints a message to a log file (does not print to console) */
		Log,

		/** Prints a verbose message to a log file (if Verbose logging is enabled for the given category) */
		Verbose,

		/** Prints a verbose message to a log file (if VeryVerbose logging is enabled) */
		VeryVerbose,

		All = VeryVerbose,
		NumVerbosity,
		VerbosityMask = 0xf,
		SetColor = 0x40,
		BreakOnLog = 0x80,
	};
}

/** Converts verbosity to a string */
const TCHAR* ToString(ELogVerbosity::Type Verbosity);
