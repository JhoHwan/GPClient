// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GP : ModuleRules
{
	public GP(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Sockets", "Networking","Json", "JsonUtilities" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore" });

        PrivateIncludePaths.AddRange(new string[]
        {
            "GP/",
            "GP/Network/",
        });
    }
}
