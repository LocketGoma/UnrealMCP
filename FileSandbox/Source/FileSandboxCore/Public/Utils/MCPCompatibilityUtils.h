#pragma once

#include "CoreMinimal.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "MCPLogCompatibility.h"

/*
 * Common utilities used by sandbox-related modules.
 *
 * This header also provides compatibility helpers for Unreal Engine versions
 * prior to UE 5.8.
 */

DEFINE_LOG_CATEGORY_STATIC(LogMCPSandboxUtils, Log, All);


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