// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LiquidGravity_Dev : ModuleRules
{
	public LiquidGravity_Dev(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"EnhancedInput",
			"UMG",
			"GameplayTags",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"LiquidMath",
			"AIModule",
			"Niagara",
			"NiagaraCore",
			"NiagaraShader",
		});
		
#if WITH_EDITOR
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"WaypointVisualizer"
			});
		}
#endif
		
	}
}