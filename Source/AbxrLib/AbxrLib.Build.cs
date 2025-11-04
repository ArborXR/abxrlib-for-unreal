using System.IO;
using UnrealBuildTool;

public class AbxrLib : ModuleRules
{
    public AbxrLib(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
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
            "ApplicationCore",
            "HeadMountedDisplay",
            "XRBase"
        });
        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicSystemLibraries.Add("Advapi32.lib");
        }
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PrivateDependencyModuleNames.AddRange(new string[] {
                "Launch"
            });
			
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "..", "..", "AbxrLib_UPL.xml"));
        }
    }
}
