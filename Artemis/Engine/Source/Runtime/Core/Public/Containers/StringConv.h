#pragma once

#include "CoreMinimal.h"
#include <cstring>
#include <cwchar>
#include <type_traits>

// An ANSI character. 8-bit fixed-width representation of 7-bit characters.
typedef char        ANSICHAR;
// A wide character. In-memory only. Same as TCHAR on Windows.
typedef wchar_t     WIDECHAR;
// An 8-bit UTF-8 code unit. A distinct type, as in the engine, so overloads can tell it apart from ANSICHAR.
enum UTF8CHAR : unsigned char {};
// UTF-16 / UTF-32 code units.
typedef char16_t    UTF16CHAR;
typedef char32_t    UTF32CHAR;
// A 16-bit UCS-2 code unit.
typedef uint16      UCS2CHAR;

static_assert(sizeof(TCHAR) == 2, "This port assumes the Windows (UTF-16) TCHAR");

#define DEFAULT_STRING_CONVERSION_SIZE 128u
#define UNICODE_BOGUS_CHAR_CODEPOINT '?'

namespace UE::Core::Private::StringConv
{
	template <typename CharType>
	FORCEINLINE int32 Strlen(const CharType* Str)
	{
		int32 Len = 0;
		while (Str[Len] != CharType(0))
		{
			++Len;
		}
		return Len;
	}

	/** Appends one codepoint as UTF-8. Writes nothing (but still counts) when Dest is null. */
	FORCEINLINE int32 WriteUTF8(UTF8CHAR* Dest, uint32 Codepoint)
	{
		if (Codepoint > 0x10FFFF || (Codepoint >= 0xD800 && Codepoint <= 0xDFFF))
		{
			Codepoint = UNICODE_BOGUS_CHAR_CODEPOINT;
		}

		if (Codepoint < 0x80)
		{
			if (Dest) { Dest[0] = (UTF8CHAR)Codepoint; }
			return 1;
		}
		if (Codepoint < 0x800)
		{
			if (Dest) { Dest[0] = (UTF8CHAR)(0xC0 | (Codepoint >> 6)); Dest[1] = (UTF8CHAR)(0x80 | (Codepoint & 0x3F)); }
			return 2;
		}
		if (Codepoint < 0x10000)
		{
			if (Dest) { Dest[0] = (UTF8CHAR)(0xE0 | (Codepoint >> 12)); Dest[1] = (UTF8CHAR)(0x80 | ((Codepoint >> 6) & 0x3F)); Dest[2] = (UTF8CHAR)(0x80 | (Codepoint & 0x3F)); }
			return 3;
		}
		if (Dest) { Dest[0] = (UTF8CHAR)(0xF0 | (Codepoint >> 18)); Dest[1] = (UTF8CHAR)(0x80 | ((Codepoint >> 12) & 0x3F)); Dest[2] = (UTF8CHAR)(0x80 | ((Codepoint >> 6) & 0x3F)); Dest[3] = (UTF8CHAR)(0x80 | (Codepoint & 0x3F)); }
		return 4;
	}

	/** Appends one codepoint as UTF-16. Writes nothing (but still counts) when Dest is null. */
	template <typename Char16Type>
	FORCEINLINE int32 WriteUTF16(Char16Type* Dest, uint32 Codepoint)
	{
		if (Codepoint > 0x10FFFF || (Codepoint >= 0xD800 && Codepoint <= 0xDFFF))
		{
			Codepoint = UNICODE_BOGUS_CHAR_CODEPOINT;
		}

		if (Codepoint < 0x10000)
		{
			if (Dest) { Dest[0] = (Char16Type)Codepoint; }
			return 1;
		}
		Codepoint -= 0x10000;
		if (Dest) { Dest[0] = (Char16Type)(0xD800 + (Codepoint >> 10)); Dest[1] = (Char16Type)(0xDC00 + (Codepoint & 0x3FF)); }
		return 2;
	}

	/** Reads one codepoint from UTF-16, advancing Index. Unpaired surrogates decode to '?'. */
	template <typename Char16Type>
	FORCEINLINE uint32 ReadUTF16(const Char16Type* Src, int32 SrcLen, int32& Index)
	{
		const uint32 High = (uint16)Src[Index++];
		if (High >= 0xD800 && High <= 0xDBFF)
		{
			if (Index < SrcLen)
			{
				const uint32 Low = (uint16)Src[Index];
				if (Low >= 0xDC00 && Low <= 0xDFFF)
				{
					++Index;
					return 0x10000 + ((High - 0xD800) << 10) + (Low - 0xDC00);
				}
			}
			return UNICODE_BOGUS_CHAR_CODEPOINT;
		}
		if (High >= 0xDC00 && High <= 0xDFFF)
		{
			return UNICODE_BOGUS_CHAR_CODEPOINT;
		}
		return High;
	}

	/** Reads one codepoint from UTF-8, advancing Index. Malformed / overlong / truncated sequences decode to '?' (one byte consumed). */
	FORCEINLINE uint32 ReadUTF8(const UTF8CHAR* Src, int32 SrcLen, int32& Index)
	{
		const uint8 Lead = (uint8)Src[Index];
		int32 Extra;
		uint32 Codepoint;
		uint32 Min;
		if      (Lead < 0x80)           { ++Index; return Lead; }
		else if ((Lead & 0xE0) == 0xC0) { Extra = 1; Codepoint = Lead & 0x1F; Min = 0x80; }
		else if ((Lead & 0xF0) == 0xE0) { Extra = 2; Codepoint = Lead & 0x0F; Min = 0x800; }
		else if ((Lead & 0xF8) == 0xF0) { Extra = 3; Codepoint = Lead & 0x07; Min = 0x10000; }
		else                            { ++Index; return UNICODE_BOGUS_CHAR_CODEPOINT; }

		if (Index + Extra >= SrcLen) // truncated sequence
		{
			++Index;
			return UNICODE_BOGUS_CHAR_CODEPOINT;
		}
		for (int32 i = 1; i <= Extra; ++i)
		{
			const uint8 Cont = (uint8)Src[Index + i];
			if ((Cont & 0xC0) != 0x80)
			{
				++Index;
				return UNICODE_BOGUS_CHAR_CODEPOINT;
			}
			Codepoint = (Codepoint << 6) | (Cont & 0x3F);
		}
		Index += Extra + 1;
		if (Codepoint < Min || Codepoint > 0x10FFFF || (Codepoint >= 0xD800 && Codepoint <= 0xDFFF))
		{
			return UNICODE_BOGUS_CHAR_CODEPOINT;
		}
		return Codepoint;
	}
}

/*-----------------------------------------------------------------------------
	Converters (the engine's TStringConvert<From, To> / FTCHARToUTF8_Convert / ...).
	Each one provides FromType, ToType, ConvertedLength() and Convert(), as the engine's do.
-----------------------------------------------------------------------------*/

/** Fixed-width conversions, i.e. ANSICHAR <-> TCHAR (FPlatformString::Convert rules). */
template <typename From, typename To>
class TStringConvert
{
public:
	typedef From FromType;
	typedef To   ToType;

	static int32 ConvertedLength(const FromType* Source, int32 SourceLen)
	{
		return SourceLen;
	}

	static void Convert(ToType* Dest, int32 DestLen, const FromType* Source, int32 SourceLen)
	{
		const int32 Size = DestLen < SourceLen ? DestLen : SourceLen;
		for (int32 i = 0; i < Size; ++i)
		{
			const FromType SrcCh = Source[i];
			if constexpr (sizeof(ToType) == 1 && sizeof(FromType) > 1)
			{
				// TCHAR -> ANSICHAR: CanConvertCodepoint<ANSICHAR> is 7-bit only.
				Dest[i] = ((uint32)SrcCh <= 0x7F) ? (ToType)SrcCh : (ToType)UNICODE_BOGUS_CHAR_CODEPOINT;
			}
			else
			{
				// ANSICHAR -> TCHAR: IsCharEncodingSimplyConvertibleTo, a plain cast.
				Dest[i] = (ToType)SrcCh;
			}
		}
	}
};

/** TCHAR (UTF-16) -> UTF-8 */
class FTCHARToUTF8_Convert
{
public:
	typedef TCHAR    FromType;
	typedef UTF8CHAR ToType;

	static int32 ConvertedLength(const FromType* Source, int32 SourceLen)
	{
		int32 Len = 0;
		for (int32 Index = 0; Index < SourceLen;)
		{
			Len += UE::Core::Private::StringConv::WriteUTF8(nullptr, UE::Core::Private::StringConv::ReadUTF16(Source, SourceLen, Index));
		}
		return Len;
	}

	static void Convert(ToType* Dest, int32 DestLen, const FromType* Source, int32 SourceLen)
	{
		int32 Written = 0;
		for (int32 Index = 0; Index < SourceLen;)
		{
			const uint32 Codepoint = UE::Core::Private::StringConv::ReadUTF16(Source, SourceLen, Index);
			if (Written + UE::Core::Private::StringConv::WriteUTF8(nullptr, Codepoint) > DestLen)
			{
				break;
			}
			Written += UE::Core::Private::StringConv::WriteUTF8(Dest + Written, Codepoint);
		}
	}
};

/** UTF-8 -> TCHAR (UTF-16) */
class FUTF8ToTCHAR_Convert
{
public:
	typedef UTF8CHAR FromType;
	typedef TCHAR    ToType;

	static int32 ConvertedLength(const FromType* Source, int32 SourceLen)
	{
		int32 Len = 0;
		for (int32 Index = 0; Index < SourceLen;)
		{
			Len += UE::Core::Private::StringConv::WriteUTF16<TCHAR>(nullptr, UE::Core::Private::StringConv::ReadUTF8(Source, SourceLen, Index));
		}
		return Len;
	}

	static void Convert(ToType* Dest, int32 DestLen, const FromType* Source, int32 SourceLen)
	{
		int32 Written = 0;
		for (int32 Index = 0; Index < SourceLen;)
		{
			const uint32 Codepoint = UE::Core::Private::StringConv::ReadUTF8(Source, SourceLen, Index);
			if (Written + UE::Core::Private::StringConv::WriteUTF16<TCHAR>(nullptr, Codepoint) > DestLen)
			{
				break;
			}
			Written += UE::Core::Private::StringConv::WriteUTF16<TCHAR>(Dest + Written, Codepoint);
		}
	}
};

/** UTF-16 -> UTF-32 */
template <typename InFromType, typename InToType>
class TUTF16ToUTF32_Convert
{
public:
	typedef InFromType FromType;
	typedef InToType   ToType;

	static int32 ConvertedLength(const FromType* Source, int32 SourceLen)
	{
		int32 Len = 0;
		for (int32 Index = 0; Index < SourceLen; ++Len)
		{
			UE::Core::Private::StringConv::ReadUTF16(Source, SourceLen, Index);
		}
		return Len;
	}

	static void Convert(ToType* Dest, int32 DestLen, const FromType* Source, int32 SourceLen)
	{
		int32 Written = 0;
		for (int32 Index = 0; Index < SourceLen && Written < DestLen; ++Written)
		{
			Dest[Written] = (ToType)UE::Core::Private::StringConv::ReadUTF16(Source, SourceLen, Index);
		}
	}
};

/** UTF-32 -> UTF-16 */
template <typename InFromType, typename InToType>
class TUTF32ToUTF16_Convert
{
public:
	typedef InFromType FromType;
	typedef InToType   ToType;

	static int32 ConvertedLength(const FromType* Source, int32 SourceLen)
	{
		int32 Len = 0;
		for (int32 Index = 0; Index < SourceLen; ++Index)
		{
			Len += UE::Core::Private::StringConv::WriteUTF16<ToType>(nullptr, (uint32)Source[Index]);
		}
		return Len;
	}

	static void Convert(ToType* Dest, int32 DestLen, const FromType* Source, int32 SourceLen)
	{
		int32 Written = 0;
		for (int32 Index = 0; Index < SourceLen; ++Index)
		{
			if (Written + UE::Core::Private::StringConv::WriteUTF16<ToType>(nullptr, (uint32)Source[Index]) > DestLen)
			{
				break;
			}
			Written += UE::Core::Private::StringConv::WriteUTF16<ToType>(Dest + Written, (uint32)Source[Index]);
		}
	}
};

/*-----------------------------------------------------------------------------
	TStringConversion / TStringPointer.
-----------------------------------------------------------------------------*/

/**
 * Class takes one type of string and converts it to another. The class includes a
 * chunk of presized memory of the destination type. If the presized array is
 * too small, it mallocs the memory needed and frees on the class's destruction.
 */
template <typename Converter, int32 DefaultConversionSize = DEFAULT_STRING_CONVERSION_SIZE>
class TStringConversion : private Converter
{
public:
	typedef typename Converter::FromType FromType;
	typedef typename Converter::ToType   ToType;

	/** Converts the null-terminated Source string. */
	explicit TStringConversion(const FromType* Source)
	{
		if (Source)
		{
			Init(Source, UE::Core::Private::StringConv::Strlen(Source));
		}
	}

	/** Converts SourceLen elements of Source (not necessarily null-terminated). The result is always null-terminated. */
	TStringConversion(const FromType* Source, int32 SourceLen)
	{
		if (Source)
		{
			Init(Source, SourceLen);
		}
	}

	/** Same-width source of another char type (e.g. a plain const char* into FUTF8ToTCHAR), as the engine allows. */
	template <typename SrcType, std::enable_if_t<!std::is_same_v<SrcType, FromType> && sizeof(SrcType) == sizeof(FromType), int> = 0>
	explicit TStringConversion(const SrcType* Source)
		: TStringConversion((const FromType*)Source)
	{
	}

	template <typename SrcType, std::enable_if_t<!std::is_same_v<SrcType, FromType> && sizeof(SrcType) == sizeof(FromType), int> = 0>
	TStringConversion(const SrcType* Source, int32 SourceLen)
		: TStringConversion((const FromType*)Source, SourceLen)
	{
	}

	~TStringConversion()
	{
		if (Ptr != InlineBuffer)
		{
			delete[] Ptr;
		}
	}

	TStringConversion(const TStringConversion&) = delete;
	TStringConversion& operator=(const TStringConversion&) = delete;
	TStringConversion& operator=(TStringConversion&&) = delete;

	TStringConversion(TStringConversion&& Other)
		: StringLength(Other.StringLength)
	{
		if (!Other.Ptr)
		{
			Ptr = nullptr;
		}
		else if (Other.Ptr == Other.InlineBuffer)
		{
			memcpy(InlineBuffer, Other.InlineBuffer, sizeof(InlineBuffer));
			Ptr = InlineBuffer;
		}
		else
		{
			Ptr = Other.Ptr;
		}
		Other.Ptr = nullptr;
		Other.StringLength = 0;
	}

	/** Accessor for the converted string (null when the source was null). */
	FORCEINLINE const ToType* Get() const
	{
		return Ptr;
	}

	/** Length of the converted string, excluding the null terminator. */
	FORCEINLINE int32 Length() const
	{
		return StringLength;
	}

private:
	void Init(const FromType* Source, int32 SourceLen)
	{
		StringLength = Converter::ConvertedLength(Source, SourceLen);

		const int32 BufferSize = StringLength + 1;
		Ptr = BufferSize <= (int32)DefaultConversionSize ? InlineBuffer : new ToType[BufferSize];

		Converter::Convert(Ptr, StringLength, Source, SourceLen);
		Ptr[StringLength] = (ToType)0;
	}

	ToType  InlineBuffer[DefaultConversionSize];
	ToType* Ptr = nullptr;
	int32   StringLength = 0;
};

/**
 * Pointer-like wrapper over a string that is already in the right representation (no copy).
 * Returned by StringCast when the encodings are compatible, like the engine.
 */
template <typename FromType, typename ToType = FromType>
class TStringPointer
{
	static_assert(sizeof(FromType) == sizeof(ToType), "FromType must be the same size as ToType");

public:
	explicit TStringPointer(const FromType* InPtr)
		: Ptr(InPtr)
		, StringLength(-1)
	{
	}

	TStringPointer(const FromType* InPtr, int32 Len)
		: Ptr(InPtr)
		, StringLength(Len)
	{
	}

	/** Accessor for the string. Note: not null-terminated if a length was given that does not cover it. */
	FORCEINLINE const ToType* Get() const
	{
		return (const ToType*)Ptr;
	}

	/** Length of the string, excluding a null terminator. */
	FORCEINLINE int32 Length() const
	{
		if (StringLength == -1)
		{
			StringLength = Ptr ? UE::Core::Private::StringConv::Strlen(Ptr) : 0;
		}
		return StringLength;
	}

private:
	const FromType* Ptr;
	mutable int32   StringLength;
};

namespace UE::Core::Private::StringConv
{
	/** FPlatformString::IsCharEncodingCompatibleWith: same width and same representation (no conversion needed). */
	template <typename From, typename To>
	constexpr bool IsCharEncodingCompatibleWith()
	{
		if constexpr (std::is_same_v<std::remove_cv_t<From>, std::remove_cv_t<To>>)
		{
			return true;
		}
		else if constexpr (sizeof(From) == 1 && sizeof(To) == 1)
		{
			// ANSICHAR -> UTF8CHAR is compatible (7-bit is a subset of UTF-8); UTF8CHAR -> ANSICHAR is not.
			return std::is_same_v<std::remove_cv_t<From>, ANSICHAR> && std::is_same_v<std::remove_cv_t<To>, UTF8CHAR>;
		}
		else
		{
			// wchar_t / char16_t / uint16 are all UTF-16 code units on Windows; char32_t / uint32 likewise for UTF-32.
			return sizeof(From) == sizeof(To) && sizeof(From) != 1;
		}
	}

	template <typename From, typename To>
	struct TStringConverterFor;

	template <> struct TStringConverterFor<ANSICHAR, TCHAR>    { typedef TStringConvert<ANSICHAR, TCHAR> Type; };
	template <> struct TStringConverterFor<TCHAR, ANSICHAR>    { typedef TStringConvert<TCHAR, ANSICHAR> Type; };
	template <> struct TStringConverterFor<TCHAR, UTF8CHAR>    { typedef FTCHARToUTF8_Convert Type; };
	template <> struct TStringConverterFor<UTF8CHAR, TCHAR>    { typedef FUTF8ToTCHAR_Convert Type; };
	template <> struct TStringConverterFor<TCHAR, UTF32CHAR>   { typedef TUTF16ToUTF32_Convert<TCHAR, UTF32CHAR> Type; };
	template <> struct TStringConverterFor<UTF32CHAR, TCHAR>   { typedef TUTF32ToUTF16_Convert<UTF32CHAR, TCHAR> Type; };
	template <> struct TStringConverterFor<UTF16CHAR, UTF32CHAR> { typedef TUTF16ToUTF32_Convert<UTF16CHAR, UTF32CHAR> Type; };
	template <> struct TStringConverterFor<UTF32CHAR, UTF16CHAR> { typedef TUTF32ToUTF16_Convert<UTF32CHAR, UTF16CHAR> Type; };
}

/**
 * Creates an object which acts as a source of a given string type.
 *
 * Example:
 *
 * void Func(const ANSICHAR* AnsiStr);
 * Func(StringCast<ANSICHAR>(TEXT("Hello")).Get());
 *
 * @param Str The null-terminated source string to convert.
 */
template <typename To, int32 DefaultConversionSize = DEFAULT_STRING_CONVERSION_SIZE, typename From>
FORCEINLINE auto StringCast(const From* Str)
{
	if constexpr (UE::Core::Private::StringConv::IsCharEncodingCompatibleWith<From, To>())
	{
		return TStringPointer<To>((const To*)Str);
	}
	else
	{
		return TStringConversion<typename UE::Core::Private::StringConv::TStringConverterFor<From, To>::Type, DefaultConversionSize>(Str);
	}
}

/**
 * Creates an object which acts as a source of a given string type.  See example above.
 *
 * @param Str The source string to convert, not necessarily null-terminated.
 * @param Len The number of From elements in Str.
 */
template <typename To, int32 DefaultConversionSize = DEFAULT_STRING_CONVERSION_SIZE, typename From>
FORCEINLINE auto StringCast(const From* Str, int32 Len)
{
	if constexpr (UE::Core::Private::StringConv::IsCharEncodingCompatibleWith<From, To>())
	{
		return TStringPointer<To>((const To*)Str, Len);
	}
	else
	{
		return TStringConversion<typename UE::Core::Private::StringConv::TStringConverterFor<From, To>::Type, DefaultConversionSize>(Str, Len);
	}
}

/**
 * Casts one fixed-width char type into another.
 *
 * @param Ch The character to convert.
 * @return The converted character.
 */
template <typename To, typename From>
FORCEINLINE To CharCast(From Ch)
{
	To Result;
	TStringConvert<From, To>::Convert(&Result, 1, &Ch, 1);
	return Result;
}

/*-----------------------------------------------------------------------------
	Typedefs and macros (verbatim engine names and expansions for a 2-byte TCHAR).
-----------------------------------------------------------------------------*/

typedef TStringConversion<FTCHARToUTF8_Convert> FTCHARToUTF8;
typedef TStringConversion<FUTF8ToTCHAR_Convert> FUTF8ToTCHAR;

typedef TStringPointer<TCHAR, UTF16CHAR> FTCHARToUTF16;
typedef TStringPointer<UTF16CHAR, TCHAR> FUTF16ToTCHAR;

typedef TStringConversion<TUTF16ToUTF32_Convert<TCHAR, UTF32CHAR>> FTCHARToUTF32;
typedef TStringConversion<TUTF32ToUTF16_Convert<UTF32CHAR, TCHAR>> FUTF32ToTCHAR;

typedef TStringPointer<TCHAR, wchar_t> FTCHARToWChar;
typedef TStringPointer<wchar_t, TCHAR> FWCharToTCHAR;

// Usage of these should be replaced with StringCasts.
#define TCHAR_TO_ANSI(str) (ANSICHAR*)StringCast<ANSICHAR>(static_cast<const TCHAR*>(str)).Get()
#define ANSI_TO_TCHAR(str) (TCHAR*)StringCast<TCHAR>(static_cast<const ANSICHAR*>(str)).Get()
#define TCHAR_TO_UTF8(str) (ANSICHAR*)FTCHARToUTF8((const TCHAR*)str).Get()
#define UTF8_TO_TCHAR(str) (TCHAR*)FUTF8ToTCHAR((const ANSICHAR*)str).Get()

#define TCHAR_TO_UTF16(str) (UTF16CHAR*)(str)
#define UTF16_TO_TCHAR(str) (TCHAR*)(str)

#define TCHAR_TO_UTF32(str) (UTF32CHAR*)FTCHARToUTF32((const TCHAR*)str).Get()
#define UTF32_TO_TCHAR(str) (TCHAR*)FUTF32ToTCHAR((const UTF32CHAR*)str).Get()

#define TCHAR_TO_WCHAR(str) (wchar_t*)(str)
#define WCHAR_TO_TCHAR(str) (TCHAR*)(str)
