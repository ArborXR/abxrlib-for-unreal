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
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "XRDMSDK",
                "Launch"  // Required for JNI
            });
            
            // Log Android-specific dependencies for debugging
            System.Console.WriteLine("AbxrLib: Adding Android-specific dependencies (XRDMSDK, Launch)");
        }
        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicSystemLibraries.Add("Advapi32.lib");
        }
    }
}
