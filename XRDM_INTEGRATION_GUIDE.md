# XRDM SDK Integration Guide for Unreal Engine

This guide explains how to integrate the XRDM SDK AAR file into your Unreal Engine AbxrLib plugin to communicate with ArborXR device management services on VR headsets.

## Overview

The XRDM SDK enables communication with ArborXR's device management system, providing access to:
- Device information (ID, serial, title, tags)
- Organization data (ID, title, slug)  
- Network information (MAC addresses)
- Authentication tokens and status
- Device fingerprinting

## Why Unreal vs Unity Integration is Different

**Unity Approach:**
- Uses `AndroidJavaObject` for automatic JNI wrapping
- Automatic garbage collection of Java objects
- Built-in Unity-specific Android integration

**Unreal Approach:**
- Direct JNI (Java Native Interface) calls
- Manual memory management of Java objects
- UPL (Unreal Plugin Language) for build system integration
- Component-based architecture with Blueprint support

## File Structure

```
abxrlib-for-unreal/
├── Source/
│   ├── AbxrLib/
│   │   ├── AbxrLib.Build.cs                    # Updated with XRDM dependencies
│   │   ├── Public/
│   │   │   └── XRDMServiceClient.h             # Service client header
│   │   └── Private/
│   │       └── XRDMServiceClient.cpp           # Service client implementation
│   └── ThirdParty/
       └── XRDMSDK/
           ├── XRDMSDK.Build.cs                 # XRDM module build configuration
           └── Android/
               ├── XRDM.SDK.External.Native.aar # The actual AAR file
               └── AbxrLib_XRDMSDK_UPL.xml      # Android build integration
```

## Integration Components

### 1. AAR File Placement
The AAR file is placed in `Source/ThirdParty/XRDMSDK/Android/` following Unreal's third-party library conventions.

### 2. Build System Configuration

**XRDMSDK.Build.cs:**
- Defines the XRDM SDK as an external module
- Configures AAR file inclusion for Android builds
- Sets up UPL integration

**AbxrLib.Build.cs:**
- Adds XRDMSDK dependency for Android platforms
- Includes Launch module for JNI support

### 3. UPL (Unreal Plugin Language) File
`AbxrLib_XRDMSDK_UPL.xml` handles:
- AAR file copying to build directory
- Gradle dependencies configuration
- Android manifest permissions
- Build-time Android-specific configuration

### 4. JNI Service Client

**XRDMServiceClient.h/cpp:**
- Unreal Component that mirrors Unity's `ArborServiceClient`
- Direct JNI calls to XRDM SDK methods
- Proper memory management of Java objects
- Blueprint-exposed functionality
- Event-driven connection handling

## Usage Examples

### Basic Setup

```cpp
// In your game mode or actor
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XRDM")
class UXRDMServiceClient* XRDMClient;

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Create XRDM service client component
    XRDMClient = CreateDefaultSubobject<UXRDMServiceClient>(TEXT("XRDMClient"));
    
    // Bind to connection events
    XRDMClient->OnConnected.AddDynamic(this, &AMyGameMode::OnXRDMConnected);
    XRDMClient->OnDisconnected.AddDynamic(this, &AMyGameMode::OnXRDMDisconnected);
}

UFUNCTION()
void AMyGameMode::OnXRDMConnected()
{
    UE_LOG(LogTemp, Log, TEXT("XRDM Connected!"));
    
    // Now you can get device information
    FString DeviceId = XRDMClient->GetDeviceId();
    FString OrgId = XRDMClient->GetOrgId();
    
    UE_LOG(LogTemp, Log, TEXT("Device ID: %s, Org ID: %s"), *DeviceId, *OrgId);
}
```

### Using Static Instance (Similar to Unity)

```cpp
// Get device information from anywhere in your code
if (UXRDMServiceClient* XRDM = UXRDMServiceClient::GetInstance())
{
    if (XRDM->IsConnected())
    {
        FString DeviceTitle = XRDM->GetDeviceTitle();
        TArray<FString> DeviceTags = XRDM->GetDeviceTags();
        bool IsAuthenticated = XRDM->GetIsAuthenticated();
        
        // Use the information as needed
    }
}
```

### Blueprint Integration

All methods are Blueprint-callable:
- Device information nodes
- Organization data nodes  
- Authentication status nodes
- Connection events

## API Reference

### Connection Methods
- `Connect()` - Establish connection to XRDM service
- `Disconnect()` - Close connection
- `IsConnected()` - Check connection status

### Device Information
- `GetDeviceId()` - ArborXR device UUID
- `GetDeviceSerial()` - OEM device serial number
- `GetDeviceTitle()` - Admin-assigned device name
- `GetDeviceTags()` - Admin-assigned device tags

### Organization Information
- `GetOrgId()` - Organization UUID
- `GetOrgTitle()` - Organization name
- `GetOrgSlug()` - Organization identifier

### Network Information
- `GetMacAddressFixed()` - Physical MAC address
- `GetMacAddressRandom()` - Randomized WiFi MAC address

### Authentication Information
- `GetIsAuthenticated()` - SSO authentication status
- `GetAccessToken()` - SSO access token
- `GetRefreshToken()` - SSO refresh token
- `GetExpiresDateUtc()` - Token expiration date
- `GetFingerprint()` - Device fingerprint

### System Information
- `GetIsInitialized()` - SDK initialization status

## Comparison with Unity Implementation

| Feature | Unity | Unreal |
|---------|--------|---------|
| **Integration Method** | AndroidJavaObject | Direct JNI |
| **Memory Management** | Automatic | Manual (Global Refs) |
| **Build Configuration** | .meta files | Build.cs + UPL |
| **Architecture** | MonoBehaviour | UActorComponent |
| **Event System** | C# Events | UE Multicast Delegates |
| **Blueprint Support** | No | Full support |
| **Error Handling** | Exceptions | Return values + logging |

## Troubleshooting

### Common Issues

**1. AAR Not Found Errors:**
```
Solution: Ensure AAR file is in correct location:
Source/ThirdParty/XRDMSDK/Android/XRDM.SDK.External.Native.aar
```

**2. JNI Class Not Found:**
```
Solution: Check that UPL file is properly configured and AAR is included in build.
Verify package name: app.xrdm.sdk.external.Sdk
```

**3. Connection Timeouts:**
```
Solution: Similar to Unity, XRDM service may need time to initialize.
The implementation includes retry logic with exponential backoff.
```

**4. Build Failures on Non-Android Platforms:**
```
Solution: All XRDM code is wrapped in #if PLATFORM_ANDROID blocks.
Non-Android builds will compile but return empty values.
```

### Debugging Tips

1. **Enable JNI Logging:**
   ```cpp
   UE_LOG(LogTemp, Log, TEXT("JNI operation result"));
   ```

2. **Check Android Logs:**
   ```bash
   adb logcat | grep -i xrdm
   ```

3. **Verify AAR Contents:**
   ```bash
   unzip -l XRDM.SDK.External.Native.aar
   ```

## Differences from Unity Version

### Advantages of Unreal Implementation:
- **Blueprint Integration**: Full Blueprint support for designers
- **Component Architecture**: Better integration with Unreal's actor system
- **Memory Control**: More control over JNI object lifecycle
- **Build System**: More flexible build configuration with UPL

### Considerations:
- **Complexity**: More manual JNI management required
- **Platform-Specific**: More explicit platform handling
- **Testing**: Requires Android device for full functionality testing

## Next Steps

1. **Test on Target Hardware**: Deploy to actual VR headsets
2. **Implement Callbacks**: Add proper JNI callback mechanism for connection events
3. **Error Handling**: Enhance error reporting and recovery
4. **Integration**: Connect with existing Authentication system
5. **Performance**: Optimize JNI calls and memory usage

## Migration from Unity

If migrating from Unity AbxrLib:

1. **Replace References**: Change `ArborServiceClient` to `UXRDMServiceClient`
2. **Update Event Binding**: Use UE delegates instead of C# events
3. **Blueprint Exposure**: Take advantage of Blueprint functionality
4. **Error Handling**: Adapt to Unreal's logging and error systems

## Building for Android

1. **Prerequisites:**
   - Android SDK/NDK configured in Unreal
   - Target Android platform in project settings
   - XRDM SDK AAR file in correct location

2. **Build Process:**
   ```bash
   # From Unreal Editor
   File -> Package Project -> Android
   
   # Or command line
   UnrealBuildTool.exe -projectfiles -project="YourProject.uproject" -game -rocket -progress
   ```

3. **Verification:**
   - Check build logs for AAR inclusion
   - Verify permissions in AndroidManifest.xml
   - Test on actual Android VR device

The integration is now complete and should provide equivalent functionality to the Unity version with the benefits of Unreal Engine's architecture and Blueprint system.
