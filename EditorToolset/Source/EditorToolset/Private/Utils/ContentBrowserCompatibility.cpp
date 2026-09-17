// Copyright Epic Games, Inc. All Rights Reserved.

#include "Utils/ContentBrowserCompatibility.h"

#include "Framework/Application/SlateApplication.h"
#include "Framework/Docking/TabManager.h"
#include "Layout/Children.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "SAssetView.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SWidget.h"

namespace UE::EditorToolset::Compatibility
{
namespace
{

void CollectSelectedAssets(const TSharedRef<SWidget>& Widget, TArray<FAssetData>& SelectedAssets,
	bool bInsideLegacySource = false)
{
	// These are the widget type names used by UE 5.7's SNew declarations.
	static const FName LegacySourceType(TEXT("UE::Editor::ContentBrowser::SLegacyContentSource"));
	static const FName AssetViewType(TEXT("SAssetView"));
	bInsideLegacySource |= Widget->GetType() == LegacySourceType;
	if (bInsideLegacySource && Widget->GetType() == AssetViewType)
	{
		for (FAssetData& Asset : StaticCastSharedRef<SAssetView>(Widget)->GetSelectedAssets())
		{
			SelectedAssets.AddUnique(MoveTemp(Asset));
		}
		return;
	}

	// Follow active content-source children only. Switching to Fab removes the
	// legacy asset view in UE 5.7; querying widgets never activates that source.
	if (FChildren* Children = Widget->GetChildren())
	{
		for (int32 Index = 0; Index < Children->Num(); ++Index)
		{
			CollectSelectedAssets(Children->GetChildAt(Index), SelectedAssets, bInsideLegacySource);
		}
	}
}

}

void GetAllSelectedAssets(TArray<FAssetData>& SelectedAssets)
{
	if (!FSlateApplication::IsInitialized())
	{
		return;
	}

	const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
	const FLevelEditorModule* LevelEditorModule = FModuleManager::GetModulePtr<FLevelEditorModule>("LevelEditor");
	const TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule
		? LevelEditorModule->GetLevelEditorTabManager() : nullptr;

	// UE 5.7 registers ContentBrowserTab1 through ContentBrowserTab4.
	// Keep this bounded: do not open tabs, change the primary browser, or scan
	// arbitrary widgets/windows. Custom browser instances and order may differ
	// from UE 5.8's AllContentBrowsers registry (see README).
	for (int32 BrowserIndex = 1; BrowserIndex <= 4; ++BrowserIndex)
	{
		const FTabId TabId(FName(*FString::Printf(TEXT("ContentBrowserTab%d"), BrowserIndex)));
		TSharedPtr<SDockTab> Tab = GlobalTabManager->FindExistingLiveTab(TabId);
		if (!Tab.IsValid() && LevelEditorTabManager.IsValid())
		{
			Tab = LevelEditorTabManager->FindExistingLiveTab(TabId);
		}
		if (Tab.IsValid())
		{
			// GetContent also exposes inactive tabs; FindExistingLiveTab includes
			// sidebar tabs. No visibility filter or tab activation is necessary.
			CollectSelectedAssets(Tab->GetContent(), SelectedAssets);
		}
	}
}

}
