// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FLevelEditorViewportClient;

namespace UE::EditorToolset::Compatibility
{

// UE 5.8 UnrealEditorSubsystem viewport utilities, kept local to this editor module.
FLevelEditorViewportClient* GetViewportClient();
bool WorldToScreen(const FVector& WorldPosition, FVector2D& ScreenPosition);
bool ScreenToWorld(const FVector2D& ScreenPosition, FVector& WorldPosition, FVector& WorldDirection);
bool GetLevelViewportSize(FIntPoint& Size);

}
