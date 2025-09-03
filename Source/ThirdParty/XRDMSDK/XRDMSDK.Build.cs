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
            
            // Add the AAR file as AndroidPlugin (this is the primary method)
            if (File.Exists(AARPath))
            {
                AdditionalPropertiesForReceipt.Add("AndroidPlugin", AARPath);
                
                // Log the AAR file path for debugging
                System.Console.WriteLine($"XRDMSDK: Adding AAR file: {AARPath}");
            }
            else
            {
                System.Console.WriteLine($"XRDMSDK: AAR file not found at: {AARPath}");
            }
            
            // Add UPL file for additional Android integration
            if (File.Exists(UPLPath))
            {
                AdditionalPropertiesForReceipt.Add("AndroidPlugin", UPLPath);
                System.Console.WriteLine($"XRDMSDK: Adding UPL file: {UPLPath}");
            }
            else
            {
                System.Console.WriteLine($"XRDMSDK: UPL file not found at: {UPLPath}");
            }
        }
    }
}
