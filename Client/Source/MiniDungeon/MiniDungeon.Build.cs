// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MiniDungeon : ModuleRules
{
	public MiniDungeon(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTasks", "NavigationSystem", "AIModule", "UMG", "Sockets", "Networking", "Slate", "SlateCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore", "Slate", "SlateCore" });

        PrivateIncludePaths.AddRange(new string[]
        {
            "MiniDungeon/",
            "MiniDungeon/Network/",
        });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
