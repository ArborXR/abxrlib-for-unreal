using UnrealBuildTool;

public class AbxrLib : ModuleRules
{
    public AbxrLib(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange([
            "Core",
            "CoreUObject",
            "Engine",
            "HTTP",
            "Json",
            "JsonUtilities",
            "OpenSSL",
            "DeveloperSettings",
            "Projects",
            "UMG",
            "Slate",
            "SlateCore",
            "InputCore",
            "ApplicationCore"
        ]);

        // Add XRDM SDK for Android platforms
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PublicDependencyModuleNames.Add("XRDMSDK");
            PublicDependencyModuleNames.Add("Launch"); // Required for JNI
        }
        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicSystemLibraries.Add("Advapi32.lib");
        }
    }
}
