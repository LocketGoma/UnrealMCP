// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "HttpResultCallback.h"

class FModelContextProtocolServer;
class FJsonObject;

namespace UE::ModelContextProtocol
{
	// Test-only accessor for `FModelContextProtocolServer` privates. Friended by the production class.
	// Implementations live in the production module so they can touch private state without DLL exports
	// leaking onto the production class itself.
	class FServerAccessor
	{
	public:
		// Dispatch a tool call in an initialized session without opening a network listener.
		static MODELCONTEXTPROTOCOL_API bool ProcessToolCall(FModelContextProtocolServer& Server, const TSharedPtr<FJsonObject>& Params, const FHttpResultCallback& OnComplete);

		// Inject one Initialized session that owns a single ActiveRequest whose EventStreamWrite is the given callback.
		static MODELCONTEXTPROTOCOL_API void AddInitializedSessionWithEventStream(FModelContextProtocolServer& Server, const FHttpResultCallback& StreamWrite);

		// Drive the server's per-frame tick once.
		static MODELCONTEXTPROTOCOL_API void Tick(FModelContextProtocolServer& Server, float DeltaTime);

		// Peek whether a `tools/list_changed` broadcast is currently scheduled but not yet drained.
		static MODELCONTEXTPROTOCOL_API bool IsBroadcastScheduled(const FModelContextProtocolServer& Server);
	};
}
