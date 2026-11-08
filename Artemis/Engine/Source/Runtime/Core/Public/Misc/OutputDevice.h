// Copyright Epic Games, Inc. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "../Logging/LogVerbosity.h"
#include <cstdarg>
#include <cwchar>
#include <string>

class FArchive;

/**
 * An output device.
 */
class FOutputDevice
{
public:
	FOutputDevice()
		 : bSuppressEventTag      (false)
		 , bAutoEmitLineTerminator(true)
	{}

	FOutputDevice(FOutputDevice&&) = default;
	FOutputDevice(const FOutputDevice&) = default;
	FOutputDevice& operator=(FOutputDevice&&) = default;
	FOutputDevice& operator=(const FOutputDevice&) = default;

	virtual ~FOutputDevice() = default;

	// FOutputDevice interface.
	virtual void Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category ) = 0;
	virtual void Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category, const double Time )
	{
		Serialize( V, Verbosity, Category );
	}

	virtual void Flush()
	{
	}

	/**
	 * Closes output device and cleans up. This can't happen in the destructor
	 * as we might have to call "delete" which cannot be done for static/ global
	 * objects.
	 */
	virtual void TearDown()
	{
	}

	void SetSuppressEventTag(bool bInSuppressEventTag)
	{
		bSuppressEventTag = bInSuppressEventTag;
	}
	FORCEINLINE bool GetSuppressEventTag() const	{	return bSuppressEventTag;	}
	void SetAutoEmitLineTerminator(bool bInAutoEmitLineTerminator)
	{
		bAutoEmitLineTerminator = bInAutoEmitLineTerminator;
	}
	FORCEINLINE bool GetAutoEmitLineTerminator() const	{	return bAutoEmitLineTerminator;	}

	/**
	 * Dumps the contents of this output device's buffer to an archive (supported by output device that have a memory buffer)
	 * @param Ar Archive to dump the buffer to
	 */
	virtual void Dump(class FArchive& Ar)
	{
	}

	/**
	 * @return whether this output device is a memory-only device
	 */
	virtual bool IsMemoryOnly() const
	{
		return false;
	}

	/**
	 * @return whether this output device can be used on any thread.
	 */
	virtual bool CanBeUsedOnAnyThread() const
	{
		return false;
	}

	/**
	 * @return whether this output device can be used from multiple threads simultaneously without any locking
	 */
	virtual bool CanBeUsedOnMultipleThreads() const
	{
		return false;
	}

	// Simple text printing.
	void Log( const TCHAR* S )                                                              { Serialize( S, ELogVerbosity::Log, FName() ); }
	void Log( ELogVerbosity::Type Verbosity, const TCHAR* S )                               { Serialize( S, Verbosity, FName() ); }
	void Log( const FName& Category, ELogVerbosity::Type Verbosity, const TCHAR* Str )      { Serialize( Str, Verbosity, Category ); }
	void Log( const FString& S )                                                            { Log( *S ); }
	void Log( ELogVerbosity::Type Verbosity, const FString& S )                             { Log( Verbosity, *S ); }
	void Log( const FName& Category, ELogVerbosity::Type Verbosity, const FString& S )      { Log( Category, Verbosity, *S ); }

	template <typename... Types>
	void Logf(const TCHAR* Fmt, Types... Args)
	{
		Serialize( FormatImpl(Fmt, Args...).c_str(), ELogVerbosity::Log, FName() );
	}

	template <typename... Types>
	void Logf(ELogVerbosity::Type Verbosity, const TCHAR* Fmt, Types... Args)
	{
		Serialize( FormatImpl(Fmt, Args...).c_str(), Verbosity, FName() );
	}

	template <typename... Types>
	void CategorizedLogf(const FName& Category, ELogVerbosity::Type Verbosity, const TCHAR* Fmt, Types... Args)
	{
		Serialize( FormatImpl(Fmt, Args...).c_str(), Verbosity, Category );
	}

private:
	static std::wstring FormatImpl(const TCHAR* Fmt, ...)
	{
		va_list Args;
		va_start(Args, Fmt);
		va_list Copy;
		va_copy(Copy, Args);
		const int Len = _vscwprintf(Fmt, Copy);
		va_end(Copy);
		std::wstring Out(Len > 0 ? Len : 0, L'\0');
		if (Len > 0)
		{
			vswprintf(Out.data(), Out.size() + 1, Fmt, Args);
		}
		va_end(Args);
		return Out;
	}

protected:
	/** Whether to output the 'Log: ' type front... */
	bool bSuppressEventTag;
	/** Whether to output a line-terminator after each log call... */
	bool bAutoEmitLineTerminator;
};

static_assert(sizeof(FOutputDevice) == 0x10, "FOutputDevice must match the engine layout");
