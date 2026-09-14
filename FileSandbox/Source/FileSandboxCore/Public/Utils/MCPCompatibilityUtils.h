#pragma once

#include "CoreMinimal.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "Logging/LogMacros.h"

/*
 * Common utilities used by sandbox-related modules.
 *
 * This header also provides compatibility helpers for Unreal Engine versions
 * prior to UE 5.8.
 */

DEFINE_LOG_CATEGORY_STATIC(LogMCPSandboxUtils, Log, All);


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

namespace UE::MCP::Compatibility
{
	inline void RaiseScriptError(const FString& ErrorMessage)
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 8

		UE::MCP::Compatibility::RaiseScriptError(ErrorMessage);

#else

#if !(UE_BUILD_TEST || UE_BUILD_SHIPPING) || WITH_EDITOR
		FFrame* TopFrame = FFrame::GetThreadLocalTopStackFrame();
		if (TopFrame)
		{
#if WITH_EDITOR
			const FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::UserRaisedError,
				FText::FromString(ErrorMessage));

			FBlueprintCoreDelegates::ThrowScriptException(
				TopFrame->Object,
				*TopFrame,
				ExceptionInfo);
#else
			// 필요하면 MCP 쪽 LogCategory 사용
			UE_LOG(
				LogTemp,
				Error,
				TEXT("%s:\n%s"),
				*ErrorMessage,
				*TopFrame->GetStackTrace());
#endif
		}
#endif

#endif
	}
}