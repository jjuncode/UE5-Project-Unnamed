// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE5_CppServer : ModuleRules
{
	public UE5_CppServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Sockets", "Networking",
															"GameplayTags",  "AnimGraphRuntime"});


		PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore" });

		PrivateIncludePaths.AddRange(new string[]
        {
            "UE5_CppServer/",
			"UE5_CppServer/Protobuf/",
			"UE5_CppServer/Network/",
			"UE5_CppServer/Game/",
			"UE5_CppServer/Game/Interface/",
			"UE5_CppServer/Game/Creature",
			"UE5_CppServer/Game/Controller/",
			"UE5_CppServer/Manager/",
			"UE5_CppServer/Asset/",
			"UE5_CppServer/Asset/Anim",
        });
    }
}
