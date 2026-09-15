// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MCPCompatibility : ModuleRules
{
	public MCPCompatibility(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.Add("Core");
	}
}
