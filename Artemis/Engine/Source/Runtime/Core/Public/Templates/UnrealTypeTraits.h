#pragma once
#include "CoreMinimal.h"
/**
 * TIsReferenceType
 */
template<typename T> struct TIsReferenceType { enum { Value = false }; };
template<typename T> struct TIsReferenceType<T&> { enum { Value = true }; };
template<typename T> struct TIsReferenceType<T&&> { enum { Value = true }; };

/**
 * TIsLValueReferenceType
 */
template<typename T> struct TIsLValueReferenceType { enum { Value = false }; };
template<typename T> struct TIsLValueReferenceType<T&> { enum { Value = true }; };

/**
 * TIsRValueReferenceType
 */
template<typename T> struct TIsRValueReferenceType { enum { Value = false }; };
template<typename T> struct TIsRValueReferenceType<T&&> { enum { Value = true }; };