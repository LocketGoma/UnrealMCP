// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ToolsetRegistry : ModuleRules
{
	public ToolsetRegistry(ReadOnlyTargetRules Target) : base(Target)
	{
		// For IWYU audits..
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs; // ..keep
		IWYUSupport = IWYUSupport.Full;  // ..keep, can be None/Minimal/Full
		bUseUnity = true; // ..toggle as needed, use false for IWYU audit


		PublicDefinitions.Add("WITH_AIASSISTANT_EPIC_INTERNAL=1");

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AssetRegistry",
				"CoreUObject",
				"DeveloperSettings",
				"EditorFramework",
				"EditorScriptingUtilities",
				"EditorSubsystem",
				"Engine",
				"Json",
				"JsonUtilities",
				"JsonUtilitiesEditor",
				"Kismet",
				"PythonScriptPlugin",
				"UnrealEd",
				"FileSandboxCore",
				"JsonSchema"
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
