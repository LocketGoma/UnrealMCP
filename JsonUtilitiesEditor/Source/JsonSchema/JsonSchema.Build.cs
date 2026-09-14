// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JsonSchema : ModuleRules
{
	public JsonSchema(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Json",
				"JsonUtilities",
			}
		);
	}
}
