// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE5_CppServer : ModuleRules
{
	public UE5_CppServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Sockets", "Networking",
															/*"AIModule", "GameplayTasks","NavigationSystem", "StateTreeModule",  "UMG"*/});


		PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore" });

		PrivateIncludePaths.AddRange(new string[]
        {
            "UE5_CppServer/",
			"UE5_CppServer/Protobuf/",
			"UE5_CppServer/Network/",
			"UE5_CppServer/Game/",
			"UE5_CppServer/Manager/",
        });
    }
}
