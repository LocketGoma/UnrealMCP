// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"
#include "Runtime/Launch/Resources/Version.h"

// -----------------------------------------------------------------------------
// UE 5.8 Compatibility
// -----------------------------------------------------------------------------

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 8

#ifndef UE_LOGF

/**
 * Logs a formatted message using a UTF-8 / ANSI format string.
 *
 * Backported from Unreal Engine 5.8 for compatibility with UE 5.7 and earlier.
 *
 * @param CategoryName   Log category.
 * @param Verbosity      ELogVerbosity level.
 * @param Format         printf-style format string.
 */
#define UE_LOGF(CategoryName, Verbosity, Format, ...) \
{ \
	UE_MCP_PRIVATE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__) \
}

#endif // UE_LOGF

#ifndef UE_CLOGF

	/**
	 * A macro that conditionally logs a formatted message if the log category is active at the requested verbosity level.
	 *
	 * @note The condition is not evaluated unless the log category is active at the requested verbosity level.
	 *
	 * @param Condition      Condition that must evaluate to true in order for the message to be logged.
	 * @param CategoryName   Name of the log category as provided to DEFINE_LOG_CATEGORY.
	 * @param Verbosity      Verbosity level of this message. See ELogVerbosity.
	 * @param Format         Format string literal in the style of printf.
	 */

	#define UE_CLOGF(Condition, CategoryName, Verbosity, Format, ...) \
	{ \
		UE_CLOG(Condition, CategoryName, Verbosity, TEXT(Format), ##__VA_ARGS__) \
	}
#endif //UE_CLOGF

//Additional version from UE_PRIVATE_LOG
#define UE_MCP_PRIVATE_LOG(CategoryName, Verbosity, Format, ...) \
	{ \
		UE_LOG(CategoryName, Verbosity, TEXT(Format), ##__VA_ARGS__)\
	}


#endif // UE 5.7 or earlier

