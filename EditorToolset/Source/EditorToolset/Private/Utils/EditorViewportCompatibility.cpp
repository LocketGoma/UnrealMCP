// Copyright Epic Games, Inc. All Rights Reserved.

#include "Utils/EditorViewportCompatibility.h"

#include "Editor.h"
#include "LevelEditorViewport.h"
#include "SceneView.h"
#include "Templates/SharedPointer.h"
#include "UnrealClient.h"

namespace UE::EditorToolset::Compatibility
{

FLevelEditorViewportClient* GetViewportClient()
{
	if (!GEditor)
	{
		return nullptr;
	}

	for (FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
	{
		if (LevelVC && LevelVC->IsPerspective())
		{
			return LevelVC;
		}
	}
	return nullptr;
}

bool WorldToScreen(const FVector& WorldPosition, FVector2D& ScreenPosition)
{
	ScreenPosition = FVector2D::ZeroVector;
	FLevelEditorViewportClient* LevelVC = GetViewportClient();
	if (!LevelVC || !LevelVC->Viewport)
	{
		return false;
	}

	const FIntPoint Size = LevelVC->Viewport->GetSizeXY();
	if (Size.X <= 0 || Size.Y <= 0)
	{
		return false;
	}

	FSceneViewFamily ViewFamily(FSceneViewFamily::ConstructionValues(
		LevelVC->Viewport, LevelVC->GetScene(), LevelVC->EngineShowFlags)
		.SetRealtimeUpdate(LevelVC->IsRealtime()));
	const TSharedPtr<FSceneView> SceneView = MakeShareable<>(LevelVC->CalcSceneView(&ViewFamily));
	if (!SceneView.IsValid())
	{
		return false;
	}

	// UE 5.7 equivalent of GetWorldToClip(); preserve the default projection result.
	if (!FSceneView::ProjectWorldToScreen(WorldPosition, FIntRect(FIntPoint(0, 0), Size),
		SceneView->ViewMatrices.GetViewProjectionMatrix(), ScreenPosition))
	{
		ScreenPosition = FVector2D::ZeroVector;
		return false;
	}
	return true;
}

bool ScreenToWorld(const FVector2D& ScreenPosition, FVector& WorldPosition, FVector& WorldDirection)
{
	WorldPosition = FVector::ZeroVector;
	WorldDirection = FVector::ZeroVector;
	FLevelEditorViewportClient* LevelVC = GetViewportClient();
	if (!LevelVC || !LevelVC->Viewport)
	{
		return false;
	}

	const FIntPoint Size = LevelVC->Viewport->GetSizeXY();
	if (Size.X <= 0 || Size.Y <= 0)
	{
		return false;
	}

	FSceneViewFamily ViewFamily(FSceneViewFamily::ConstructionValues(
		LevelVC->Viewport, LevelVC->GetScene(), LevelVC->EngineShowFlags)
		.SetRealtimeUpdate(LevelVC->IsRealtime()));
	const TSharedPtr<FSceneView> SceneView = MakeShareable<>(LevelVC->CalcSceneView(&ViewFamily));
	if (!SceneView.IsValid())
	{
		return false;
	}

	// UE 5.7 equivalent of GetClipToWorld(). Deprojection has no boolean result.
	const FMatrix InvViewProjectionMatrix = SceneView->ViewMatrices.GetInvViewProjectionMatrix();
	FSceneView::DeprojectScreenToWorld(ScreenPosition, FIntRect(FIntPoint(0, 0), Size),
		InvViewProjectionMatrix, WorldPosition, WorldDirection);
	return true;
}

bool GetLevelViewportSize(FIntPoint& Size)
{
	Size = FIntPoint::ZeroValue;
	FLevelEditorViewportClient* LevelVC = GetViewportClient();
	if (!LevelVC || !LevelVC->Viewport)
	{
		return false;
	}

	Size = LevelVC->Viewport->GetSizeXY();
	return true;
}

}
