// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JsonUtilitiesEditor : ModuleRules
{
    public JsonUtilitiesEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Json",
                "JsonSchema",
            }
        );

        PrivateDependencyModuleNames.AddRange(
	        new string[]
	        {
		        "Engine",
		        "UnrealEd",
		        "Slate",
		        "SlateCore",
		        "JsonUtilities",
		        "BlueprintGraph",
		        "AssetTools",
	        }
        );
    }
}
