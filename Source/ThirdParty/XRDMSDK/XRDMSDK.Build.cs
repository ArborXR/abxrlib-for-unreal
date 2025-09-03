using UnrealBuildTool;
using System.IO;

public class XRDMSDK : ModuleRules
{
    public XRDMSDK(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string ModulePath = ModuleDirectory;
            string AndroidPath = Path.Combine(ModulePath, "Android");
            string AARPath = Path.Combine(AndroidPath, "XRDM.SDK.External.Native.aar");
            string UPLPath = Path.Combine(AndroidPath, "AbxrLib_XRDMSDK_UPL.xml");
            
            // Add the AAR file
            if (File.Exists(AARPath))
            {
                PublicAdditionalLibraries.Add(AARPath);
                
                // Add Android dependencies
                AdditionalPropertiesForReceipt.Add("AndroidPlugin", AARPath);
                
                // Add UPL file for proper Android integration
                if (File.Exists(UPLPath))
                {
                    AdditionalPropertiesForReceipt.Add("AndroidPlugin", UPLPath);
                }
                
                // Add required Java classes to the build
                string BuildGradleAdditions = @"
                    dependencies {
                        implementation files('XRDM.SDK.External.Native.aar')
                    }
                ";
                AdditionalPropertiesForReceipt.Add("AndroidPluginBuildGradleAdditions", BuildGradleAdditions);
            }
        }
    }
}
