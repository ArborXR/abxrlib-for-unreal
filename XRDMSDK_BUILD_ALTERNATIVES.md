# XRDMSDK Build Configuration Alternatives

If you're experiencing build issues with the AAR integration, try these alternative approaches:

## Alternative 1: Simplified Build Configuration

Replace the content of `XRDMSDK.Build.cs` with this simpler version:

```csharp
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
            string SimpleUPLPath = Path.Combine(AndroidPath, "AbxrLib_XRDMSDK_Simple_UPL.xml");
            
            // Only add the AAR file via AndroidPlugin
            if (File.Exists(AARPath))
            {
                AdditionalPropertiesForReceipt.Add("AndroidPlugin", AARPath);
                System.Console.WriteLine($"XRDMSDK: Adding AAR file: {AARPath}");
            }
            
            // Use simple UPL for permissions only
            if (File.Exists(SimpleUPLPath))
            {
                AdditionalPropertiesForReceipt.Add("AndroidPlugin", SimpleUPLPath);
            }
        }
    }
}
```

## Alternative 2: Manual AAR Integration

If the above doesn't work, you can manually integrate the AAR:

1. **Copy AAR to Project:**
   ```
   Copy XRDM.SDK.External.Native.aar to:
   [YourProject]/Build/Android/gradle/app/libs/
   ```

2. **Modify Project Build.gradle:**
   Add to your project's `Build/Android/gradle/app/build.gradle`:
   ```gradle
   dependencies {
       implementation files('libs/XRDM.SDK.External.Native.aar')
   }
   ```

3. **Add Permissions to AndroidManifest.xml:**
   ```xml
   <uses-permission android:name="android.permission.INTERNET" />
   <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
   <uses-permission android:name="android.permission.ACCESS_WIFI_STATE" />
   ```

## Alternative 3: Disable XRDM for Testing

To test if XRDM is causing the build issues, temporarily disable it:

1. **Comment out XRDM dependency in AbxrLib.Build.cs:**
   ```csharp
   // if (Target.Platform == UnrealTargetPlatform.Android)
   // {
   //     PublicDependencyModuleNames.AddRange(new string[]
   //     {
   //         "XRDMSDK",
   //         "Launch"
   //     });
   // }
   ```

2. **Wrap XRDM code in preprocessor directives:**
   Add `#define ENABLE_XRDM_SDK 0` at the top of XRDMServiceClient.h and wrap all XRDM code with:
   ```cpp
   #if ENABLE_XRDM_SDK
   // XRDM code here
   #endif
   ```

## Common Build Issues and Solutions

### Issue 1: "Module 'XRDMSDK' not found"
**Solution:** Ensure the XRDMSDK.Build.cs file is in the correct location:
```
Source/ThirdParty/XRDMSDK/XRDMSDK.Build.cs
```

### Issue 2: AAR file not found during build
**Solution:** Check the console output for the path being used and verify the AAR file exists:
```bash
ls -la Source/ThirdParty/XRDMSDK/Android/XRDM.SDK.External.Native.aar
```

### Issue 3: JNI compilation errors
**Solution:** These might occur on non-Android platforms. Ensure all JNI code is wrapped in `#if PLATFORM_ANDROID` blocks.

### Issue 4: Gradle build failures
**Solution:** The UPL might be causing conflicts. Try the simplified version or manual integration.

## Debug Steps

1. **Check Console Output:**
   Look for our debug messages during build:
   - "XRDMSDK: Adding AAR file: [path]"
   - "AbxrLib: Adding Android-specific dependencies"

2. **Verify File Existence:**
   ```bash
   ls -la Source/ThirdParty/XRDMSDK/Android/
   ```

3. **Check Generated Files:**
   After building, check if the AAR was copied to the build directory:
   ```bash
   find [ProjectDir]/Intermediate/Android -name "*.aar"
   ```

4. **Build Log Analysis:**
   Look for any mention of "XRDM" or "app.xrdm.sdk.external" in the build logs.

## Recommended Approach

Start with **Alternative 1 (Simplified Build Configuration)** as it removes potential conflicts while maintaining functionality. If that doesn't work, proceed to **Alternative 3 (Disable XRDM for Testing)** to confirm if XRDM is the source of the build issues.
