using UnrealBuildTool;

public class AbxrLib : ModuleRules
{
    public AbxrLib(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange([
            "Core", "CoreUObject", "Engine", "HTTP", "Json", "JsonUtilities", "OpenSSL", "DeveloperSettings"
        ]);
        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicSystemLibraries.Add("Advapi32.lib");
        }
    }
}
