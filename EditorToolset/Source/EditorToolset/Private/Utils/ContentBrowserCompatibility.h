// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AssetRegistry/AssetData.h"
#include "Containers/Array.h"

namespace UE::EditorToolset::Compatibility
{

// Appends unique selections from existing standard Content Browser tabs (1-4).
// Unlike UE 5.8's internal browser registry, custom browser instances are not covered.
// The separate Content Drawer is excluded, matching UE 5.8 GetAllSelectedAssets.
void GetAllSelectedAssets(TArray<FAssetData>& SelectedAssets);

}
