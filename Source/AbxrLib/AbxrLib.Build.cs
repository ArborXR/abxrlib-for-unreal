using UnrealBuildTool;

public class AbxrLib : ModuleRules
{
    public AbxrLib(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange([
            "Core", "CoreUObject", "Engine", "HTTP", "Json", "JsonUtilities"
        ]);
    }
}
