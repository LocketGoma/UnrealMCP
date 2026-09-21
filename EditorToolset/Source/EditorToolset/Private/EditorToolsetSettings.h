// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"

#include "EditorToolsetSettings.generated.h"

/** Preferences for the editor tools exposed through MCP. */
UCLASS(config = EditorPerProjectUserSettings, meta = (DisplayName = "Editor Toolset"))
class UEditorToolsetSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetContainerName() const override { return FName(TEXT("Editor")); }
	virtual FName GetCategoryName() const override { return FName(TEXT("Plugins")); }

	/** Include full CVar help text in SearchCVars responses. Disabled by default to keep responses small. */
	UPROPERTY(Config, EditAnywhere, Category = "CVar Search", meta = (DisplayName = "Include CVar Help"))
	bool bIncludeCVarHelp = false;

	/** Default number of CVar matches per page. Explicit request limits override this value. */
	UPROPERTY(Config, EditAnywhere, Category = "CVar Search", meta = (DisplayName = "Default CVar Page Size", ClampMin = "1", UIMin = "1"))
	int32 DefaultCVarPageSize = 25;

	/** Default number of newest matching lines returned by GetLogEntries. Explicit request limits override this value. */
	UPROPERTY(Config, EditAnywhere, Category = "Log Queries", meta = (DisplayName = "Default Log Max Entries", ClampMin = "1", UIMin = "1"))
	int32 DefaultLogMaxEntries = 100;
};
