// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Renewal_BreakOut : ModuleRules
{
	public Renewal_BreakOut(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "Renewal_BreakOut" });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore" ,
            "EnhancedInput", "UMG","ProceduralMeshComponent", 
            "Niagara","GeometryCore","DynamicMesh","MovieScene"
            ,"LevelSequence"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

	}
}
