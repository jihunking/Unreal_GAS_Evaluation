// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Unreal_GAS : ModuleRules
{
	public Unreal_GAS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
            "GameplayAbilities",
		    "GameplayTags",
			"GameplayTasks"
        });

		PublicIncludePaths.AddRange(new string[] {
			"Unreal_GAS",
			"Unreal_GAS/Variant_Platforming",
			"Unreal_GAS/Variant_Platforming/Animation",
			"Unreal_GAS/Variant_Combat",
			"Unreal_GAS/Variant_Combat/AI",
			"Unreal_GAS/Variant_Combat/Animation",
			"Unreal_GAS/Variant_Combat/Gameplay",
			"Unreal_GAS/Variant_Combat/Interfaces",
			"Unreal_GAS/Variant_Combat/UI",
			"Unreal_GAS/Variant_SideScrolling",
			"Unreal_GAS/Variant_SideScrolling/AI",
			"Unreal_GAS/Variant_SideScrolling/Gameplay",
			"Unreal_GAS/Variant_SideScrolling/Interfaces",
			"Unreal_GAS/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
