# ABXRLib SDK for Unreal

The name "ABXR" stands for "Analytics Backbone for XR"—a flexible, open-source foundation for capturing and transmitting spatial, interaction, and performance data in XR. When combined with **ArborXR Insights**, ABXR transforms from a lightweight instrumentation layer into a full-scale enterprise analytics solution—unlocking powerful dashboards, LMS/BI integrations, and AI-enhanced insights.

## Table of Contents
1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Configuration](#configuration)
4. [Sending Data](#sending-data)
   - [Events](#events)
   - [Analytics Event Wrappers](#analytics-event-wrappers-essential-for-all-developers)
   - [Timed Events](#timed-events)
   - [Super Properties](#super-properties)
   - [Logging](#logging)
   - [Storage](#storage)
   - [Telemetry](#telemetry)
   - [AI Integration](#ai-integration)
   - [Exit Polls](#exit-polls)
   - [Metadata Formats](#metadata-formats)
5. [Advanced Features](#advanced-features)
   - [Module Targets](#module-targets)
   - [Authentication](#authentication)
   - [Session Management](#session-management)
   - [Mixpanel Compatibility](#mixpanel-compatibility)
6. [Support](#support)
   - [Resources](#resources)
   - [FAQ](#faq)
   - [Troubleshooting](#troubleshooting)

---

## Introduction

### Overview

The **ABXRLib SDK for Unreal** is an open-source analytics and data collection library that provides developers with the tools to collect and send XR data to any service of their choice. This library enables scalable event tracking, telemetry, and session-based storage—essential for enterprise and education XR environments.

> **Quick Start:** Most developers can integrate ABXRLib SDK and log their first event in under **15 minutes**.

**Why Use ABXRLib SDK?**

- **Open-Source** & portable to any backend—no vendor lock-in  
- **Quick integration**—track user interactions in minutes  
- **Secure & scalable**—ready for enterprise use cases  
- **Pluggable with ArborXR Insights**—seamless access to LMS/BI integrations, session replays, AI diagnostics, and more

### Core Features

The ABXRLib SDK provides:
- **Event Tracking:** Monitor user behaviors, interactions, and system events.
- **Spatial & Hardware Telemetry:** Capture headset/controller movement and hardware metrics.
- **Object & System Info:** Track XR objects and environmental state.
- **Storage & Session Management:** Support resumable training and long-form experiences.
- **Logs:** Developer and system-level logs available across sessions.

### Backend Services

The ABXRLib SDK is designed to work with any backend service that implements the ABXR protocol. Currently supported services include:

#### ArborXR Insights
When paired with [**ArborXR Insights**](https://arborxr.com/insights), ABXR becomes a full-service platform offering:
- Seamless data pipeline from headset to dashboard
- End-to-end session tracking, analysis, and replay
- AI-driven insights for content quality, learner performance, and device usage
- One-click LMS and BI integrations for scalable deployments

#### Custom Implementations
Developers can implement their own backend services by following the ABXR protocol specification. This allows for complete control over data storage, processing, and visualization.

---

## Installation

### Unreal Plugin Installation

1. Download the plugin from the GitHub repository.
2. Copy the plugin folder to your project's `Plugins` directory.
3. Open your Unreal project and go to `Edit > Plugins`.
4. Find `ABXRLib SDK` and enable it.
5. Once imported, you will see `ABXRLib SDK` configuration options in your Project Settings.

---

## Configuration

### Using with ArborXR Insights

To use the ABXRLib SDK with ArborXR Insights:

#### Get Your Credentials
1. Go to the ArborXR Insights web app and log in.
2. Grab these three values from the **View Data** screen of the specific app you are configuring:
- App ID
- Organization ID
- Authentication Secret

#### Configure Unreal Project

> **⚠️ Security Note:** For production builds distributed to third parties, avoid compiling `Org ID` and `Auth Secret` directly into your Unreal project. These credentials should only be compiled into builds when creating custom applications for specific individual clients. For general distribution, use ArborXR-managed devices or implement runtime credential provisioning.

1. Open `Edit > Project Settings > Plugins > ABXR Configuration` in the Unreal Editor.
2. **For Development/Testing:** Paste in the App ID, Org ID, and Auth Secret. All 3 are required if you are testing from Unreal itself.
3. **For Production Builds:** Only include the App ID. Leave Org ID and Auth Secret empty for third-party distribution.

#### Alternative for Managed Headsets:
If you're using an ArborXR-managed device, only the App ID is required. The Org ID and Auth Secret auto-fill. 
On any non-managed headset, you must manually enter all three values for testing purposes only.

### Using with Other Backend Services
For information on implementing your own backend service or using other compatible services, please refer to the ABXR protocol specification.

---

## Sending Data

### Events
```cpp
//C++ Event Method Signatures
public static void Event(const FString& Name);
public static void Event(const FString& Name, TMap<FString, FString>& Meta);

// Example Usage - Basic Event
UAbxr::Event(TEXT("button_pressed"));
```
**Parameters:**
- `name` (FString): The name of the event. Use snake_case for better analytics processing.
- `meta` (TMap<FString, FString>): Optional. Additional key-value pairs describing the event.

Logs a named event with optional metadata and spatial context. Timestamps and origin (`user` or `system`) are automatically appended.

### Analytics Event Wrappers (Essential for All Developers)

**These analytics event functions are essential for ALL developers, not just those integrating with LMS platforms.** They provide standardized tracking for key user interactions and learning outcomes that are crucial for understanding user behavior, measuring engagement, and optimizing XR experiences.

**EventAssessmentStart and EventAssessmentComplete should be considered REQUIRED for proper usage** of the ABXRLib SDK, as they provide critical insights into user performance and completion rates.

The Analytics Event Functions are specialized versions of the Event method, tailored for common scenarios in XR experiences. These functions help enforce consistency in event logging across different parts of the application and provide valuable data for analytics, user experience optimization, and business intelligence. While they also power integrations with Learning Management System (LMS) platforms, their benefits extend far beyond educational use cases.

#### Assessments, Objectives & Interactions
Assessments are intended to track the overall performance of a learner across multiple Objectives and 
Interactions. 
* Think of it as the learner's score for a specific course or curriculum.
* When the Assessment is complete, it will automatically record and close out the Assessment in the various LMS 
platforms we support.
Objectives are sub-tasks within an assessment
Interactions are sub-tasks to an assessment or objective

```cpp
// Status enumeration for all analytics events
public enum EEventStatus { Pass, Fail, Complete, Incomplete, Browsed }
public enum EInteractionType { Null, Bool, Select, Text, Rating, Number, Matching, Performance, Sequencing }

// Assessment tracking (overall course/curriculum performance)
UAbxr::EventAssessmentStart(TEXT("final_exam"));
UAbxr::EventAssessmentComplete(TEXT("final_exam"), 92, EEventStatus::Pass);

// Objective tracking (specific learning goals)
UAbxr::EventObjectiveStart(TEXT("open_valve"));
UAbxr::EventObjectiveComplete(TEXT("open_valve"), 100, EEventStatus::Complete);

// Interaction tracking (individual user responses)
UAbxr::EventInteractionStart(TEXT("select_option_a"));
UAbxr::EventInteractionComplete(TEXT("select_option_a"), EInteractionType::Select, TEXT("true"));
```

#### Additional Event Wrappers
```cpp
// Level tracking 
UAbxr::EventLevelStart(TEXT("level_1"));
UAbxr::EventLevelComplete(TEXT("level_1"), 85);

// Critical event flagging (for safety training, high-risk errors, etc.)
UAbxr::EventCritical(TEXT("safety_violation"));
```

**Parameters for all Event Wrapper Functions:**
- `levelName/assessmentName/objectiveName/interactionName` (FString): The identifier for the assessment, objective, interaction, or level.
- `score` (int): The numerical score achieved. While typically between 1-100, any integer is valid. In metadata, you can also set a minScore and maxScore to define the range of scores for this objective.
- `result` (Interactions): The result for the interaction is based on the InteractionType.
- `result_details` (FString): Optional. Additional details about the result. For interactions, this can be a single character or a string. For example: TEXT("a"), TEXT("b"), TEXT("c") or TEXT("correct"), TEXT("incorrect").
- `type` (EInteractionType): Optional. The type of interaction for this event.
- `meta` (TMap<FString, FString>): Optional. Additional key-value pairs describing the event.

**Note:** All complete events automatically calculate duration if a corresponding start event was logged.

### Timed Events

The ABXRLib SDK includes a built-in timing system that allows you to measure the duration of any event. This is useful for tracking how long users spend on specific activities.

```cpp
// C++ Timed Event Method Signature
public static void UAbxr::StartTimedEvent(const FString& EventName);

// Example Usage
UAbxr::StartTimedEvent(TEXT("Table puzzle"));
// ... user performs puzzle activity for 20 seconds ...
UAbxr::Event(TEXT("Table puzzle")); // Duration automatically included: 20 seconds

// Works with all event methods
UAbxr::StartTimedEvent(TEXT("Assessment"));
// ... later ...
UAbxr::EventAssessmentComplete(TEXT("Assessment"), 95, EEventStatus::Pass); // Duration included

// Also works with Mixpanel compatibility methods
UAbxr::StartTimedEvent(TEXT("User Session"));
// ... later ...
UAbxr::Track(TEXT("User Session")); // Duration automatically included
```

**Parameters:**
- `eventName` (FString): The name of the event to start timing. Must match the event name used later.

**Note:** The timer automatically adds a `duration` field (in seconds) to any subsequent event with the same name. The timer is automatically removed after the first matching event.

### Super Properties

Global properties automatically included in all events:

```cpp
// Set persistent properties (included in all events)
UAbxr::Register(TEXT("user_type"), TEXT("premium"));
UAbxr::Register(TEXT("app_version"), TEXT("1.2.3"));

// Set only if not already set
UAbxr::RegisterOnce(TEXT("user_tier"), TEXT("free"));

// Management
UAbxr::Unregister(TEXT("device_type"));  // Remove specific property
UAbxr::Reset();                          // Clear all super properties
```

Perfect for user attributes, app state, and device information that should be included with every event.

---

### Logging
The Log Methods provide straightforward logging functionality, similar to syslogs. These functions are available to developers by default, even across enterprise users, allowing for consistent and accessible logging across different deployment scenarios.

```cpp
//C++ Event Method Signatures
public static void LogDebug(const FString& Message)
public static void LogInfo(const FString& Message)
public static void LogWarn(const FString& Message)
public static void LogError(const FString& Message)
public static void LogCritical(const FString& Message)

// Example usage
UAbxr::LogError(TEXT("Critical error in assessment phase"));
```

---

### Telemetry
The Telemetry Methods provide comprehensive tracking of the XR environment. By default, they capture headset and controller movements, but can be extended to track any custom objects in the virtual space. These functions also allow collection of system-level data such as frame rates or device temperatures. This versatile tracking enables developers to gain deep insights into user interactions and application performance, facilitating optimization and enhancing the overall XR experience.

To log spatial or system telemetry:
```cpp
//C++ Event Method Signatures
public void TelemetryEntry(const FString& Name, TMap<FString, FString>& Meta)

// Example usage
UAbxr::TelemetryEntry(TEXT("headset_position"), TMap<FString, FString> { {TEXT("Host"), TEXT("api.example.com")}, {TEXT("Port"), TEXT("443")}, {TEXT("Scheme"), TEXT("https")} });
```

**Parameters:**
- `Name` (FString): The type of telemetry data (e.g., "headset_position", "frame_rate", "battery_level").
- `Meta` (TMap<FString, FString>): Key-value pairs of telemetry measurements.

---

### Storage
The Storage API enables developers to store and retrieve learner/player progress, facilitating the creation of long-form training content. When users log in using ArborXR's facility or the developer's in-app solution, these methods allow users to continue their progress on different headsets, ensuring a seamless learning experience across multiple sessions or devices.

```cpp
// Save progress data
UAbxr::StorageSetEntry(TEXT("state"), TMap<FString, FString>{{TEXT("progress"), TEXT("75%")}}, EStorageScope::User);
UAbxr::StorageSetDefaultEntry(TMap<FString, FString>{{TEXT("progress"), TEXT("75%")}}, EStorageScope::User);

// Retrieve progress data (requires coroutine)
// Note: Implement coroutine-based retrieval in Unreal as needed
UAbxr::StorageGetEntry(TEXT("state"), EStorageScope::User, /* callback */);
UAbxr::StorageGetDefaultEntry(EStorageScope::User, /* callback */);

// Remove storage entries  
UAbxr::StorageRemoveEntry(TEXT("state"), EStorageScope::User);
UAbxr::StorageRemoveDefaultEntry(EStorageScope::User);
UAbxr::StorageRemoveMultipleEntries(EStorageScope::User); // Clear all entries (use with caution)
```

**Parameters:**
- `name` (FString): The identifier for this storage entry.
- `entry` (TMap<FString, FString>): The key-value pairs to store.
- `scope` (EStorageScope): Store/retrieve from 'Device' or 'User' storage.
- `policy` (EStoragePolicy): How data should be stored - 'KeepLatest' or 'AppendHistory' (defaults to 'KeepLatest').

---

### AI Integration

```cpp
// Access GPT services for AI-powered interactions (requires coroutine)
// Note: Implement coroutine-based AI calls in Unreal as needed
UAbxr::AIProxy(TEXT("How can I help you today?"), TEXT("gpt-4"), /* callback */);

// With previous messages for context
TArray<FString> PastMessages = {TEXT("Hello"), TEXT("Hi there! How can I help?")};
UAbxr::AIProxy(TEXT("What's the weather like?"), PastMessages, TEXT("gpt-4"), /* callback */);
```

**Parameters:**
- `prompt` (FString): The input prompt for the AI.
- `llmProvider` (FString): The LLM provider identifier.
- `pastMessages` (TArray<FString>): Optional. Previous conversation history for context.

**Note:** AIProxy calls are processed immediately and bypass the cache system.

### Exit Polls
Deliver questionnaires to users to gather feedback.
```cpp
// Poll types: Thumbs, Rating (1-5), MultipleChoice (2-8 options)
UAbxr::PollUser(TEXT("How would you rate this training experience?"), EPollType::Rating);
```
**Poll Types:**
- `Thumbs Up/Thumbs Down`
- `Rating (1-5)`
- `Multiple Choice (2-8 string options)`

---

### Metadata Formats

The ABXRLib SDK for Unreal supports metadata through the `TMap<FString, FString>` parameter in all event and log methods. This native Unreal format provides efficient key-value pairs for describing events and context.

#### TMap<FString, FString> (Native Unreal Format)
```cpp
// Native Unreal format - most efficient and recommended
TMap<FString, FString> meta;
meta.Add(TEXT("action"), TEXT("click"));
meta.Add(TEXT("timestamp"), FDateTime::UtcNow().ToString());
meta.Add(TEXT("userId"), TEXT("12345"));
meta.Add(TEXT("completed"), TEXT("true"));

UAbxr::Event(TEXT("user_action"), meta);

// Inline construction
UAbxr::LogInfo(TEXT("User login"), TMap<FString, FString> {
    {TEXT("username"), TEXT("john_doe")},
    {TEXT("loginMethod"), TEXT("oauth")},
    {TEXT("deviceType"), TEXT("quest3")}
});
```

#### Complex Data Types
```cpp
// For complex data, convert to strings before adding to metadata
FVector PlayerPosition = GetActorLocation();
TMap<FString, FString> locationMeta;
locationMeta.Add(TEXT("position_x"), FString::SanitizeFloat(PlayerPosition.X));
locationMeta.Add(TEXT("position_y"), FString::SanitizeFloat(PlayerPosition.Y));
locationMeta.Add(TEXT("position_z"), FString::SanitizeFloat(PlayerPosition.Z));

UAbxr::Event(TEXT("player_moved"), locationMeta);

// For arrays, serialize to JSON-like strings
TArray<FString> Items = {TEXT("sword"), TEXT("shield"), TEXT("potion")};
FString ItemsString = TEXT("[\"") + FString::Join(Items, TEXT("\", \"")) + TEXT("\"]");
TMap<FString, FString> inventoryMeta;
inventoryMeta.Add(TEXT("items"), ItemsString);

UAbxr::Event(TEXT("inventory_updated"), inventoryMeta);
```

#### No Metadata
```cpp
// Events and logs work fine without metadata
UAbxr::Event(TEXT("app_started"));
UAbxr::LogInfo(TEXT("Application initialized"));
```

#### Type Conversion Guidelines

Since ABXR metadata uses string key-value pairs, follow these patterns for common Unreal types:

```cpp
// Numeric types
meta.Add(TEXT("score"), FString::FromInt(PlayerScore));          // int32 → string
meta.Add(TEXT("health"), FString::SanitizeFloat(PlayerHealth));  // float → string
meta.Add(TEXT("timestamp"), FString::FromInt(FDateTime::UtcNow().ToUnixTimestamp())); // int64 → string

// Boolean types  
meta.Add(TEXT("completed"), PlayerCompleted ? TEXT("true") : TEXT("false"));

// Enums
meta.Add(TEXT("difficulty"), UEnum::GetValueAsString(DifficultyLevel));

// Vectors and Rotators
FVector Location = GetActorLocation();
meta.Add(TEXT("location"), FString::Printf(TEXT("%.2f,%.2f,%.2f"), Location.X, Location.Y, Location.Z));

FRotator Rotation = GetActorRotation(); 
meta.Add(TEXT("rotation"), FString::Printf(TEXT("%.2f,%.2f,%.2f"), Rotation.Pitch, Rotation.Yaw, Rotation.Roll));
```

**All event and log methods support TMap<FString, FString> metadata:**
- `UAbxr::Event(name, meta?)`
- `UAbxr::EventAssessmentStart/Complete(..., meta?)`
- `UAbxr::EventObjectiveStart/Complete(..., meta?)`
- `UAbxr::EventInteractionStart/Complete(..., meta?)`
- `UAbxr::EventLevelStart/Complete(..., meta?)`
- `UAbxr::LogDebug/Info/Warn/Error/Critical(message, meta?)`
- `UAbxr::TelemetryEntry(name, meta?)`

---

## Advanced Features

### Module Targets

The **Module Target** feature enables developers to create single applications with multiple modules, where each module can be its own assignment in an LMS. When a learner enters from the LMS for a specific module, the application can automatically direct the user to that module within the application. Individual grades and results are then tracked for that specific assignment in the LMS.

#### Getting Module Target Information

You can process module targets sequentially:

```cpp
// Get the next module target from the queue
FCurrentSessionData NextTarget = UAbxr::GetModuleTarget();
if (!NextTarget.moduleTarget.IsEmpty())
{
    UE_LOG(LogTemp, Log, TEXT("Processing module: %s"), *NextTarget.moduleTarget);
    EnableModuleFeatures(NextTarget.moduleTarget);
    NavigateToModule(NextTarget.moduleTarget);
}
else
{
    UE_LOG(LogTemp, Log, TEXT("All modules completed!"));
    ShowCompletionScreen();
}

// Check remaining module count
int32 Remaining = UAbxr::GetModuleTargetCount();
UE_LOG(LogTemp, Log, TEXT("Modules remaining: %d"), Remaining);

// Get current user information
FString UserId = UAbxr::GetUserId();
FString UserData = UAbxr::GetUserData();
FString UserEmail = UAbxr::GetUserEmail();
```

#### Module Target Management

You can also manage the module target queue directly:

```cpp
// Check how many module targets remain
int32 Count = UAbxr::GetModuleTargetCount();
UE_LOG(LogTemp, Log, TEXT("Modules remaining: %d"), Count);

// Clear all module targets and storage
UAbxr::ClearModuleTargets();
```

**Use Cases:**
- **Reset state**: Clear module targets when starting a new experience
- **Error recovery**: Clear corrupted module target data
- **Testing**: Reset module queue during development
- **Session management**: Clean up between different users

### Authentication

The ABXRLib SDK provides comprehensive authentication completion callbacks that deliver detailed user and module information. This enables rich post-authentication workflows including automatic module navigation and personalized user experiences.

#### Authentication Completion Callback

Subscribe to authentication events to receive user information and module targets:

```cpp
// Basic authentication callback
UAbxr::OnAuthCompleted.AddDynamic(this, &AMyActor::HandleAuthCompleted);

// In your actor's header file (.h):
UFUNCTION()
void HandleAuthCompleted(const FAuthCompletedData& AuthData);

// In your actor's implementation file (.cpp):
void AMyActor::HandleAuthCompleted(const FAuthCompletedData& AuthData)
{
    if (AuthData.bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("Welcome %s!"), *AuthData.UserEmail);
        
        // Handle initial vs reauthentication
        if (AuthData.bIsReauthentication)
        {
            RefreshUserData();
        }
        else
        {
            InitializeUserInterface();
        }
        
        // Navigate to module if specified
        if (!AuthData.ModuleTarget.IsEmpty())
        {
            NavigateToModule(AuthData.ModuleTarget);
        }
    }
}
```

#### Connection Status Check

You can check if AbxrLib has an active connection to the server at any time:

```cpp
// C++ Method Signature
bool UAbxr::ConnectionActive();

// Example usage
// Check app-level connection status  
if (UAbxr::ConnectionActive())
{
    UE_LOG(LogTemp, Log, TEXT("ABXR is connected and ready to send data"));
    UAbxr::Event(TEXT("app_ready"));
}
else
{
    UE_LOG(LogTemp, Warning, TEXT("Connection not active - waiting for authentication"));
    UAbxr::OnAuthCompleted.AddDynamic(this, &AMyActor::HandleAuthCompleted);
}
```

**Returns:** Boolean indicating if the library has an active connection and can communicate with the server

### Session Management

The ABXRLib SDK provides comprehensive session management capabilities that allow you to control authentication state and session continuity. These methods are particularly useful for multi-user environments, testing scenarios, and creating seamless user experiences across devices and time.

#### StartNewSession
Start a new session with a fresh session identifier. This method generates a new session ID and performs fresh authentication, making it ideal for starting new training experiences or resetting user context.

```cpp
// C++ Method Signature
void UAbxr::StartNewSession();

// Example Usage
UAbxr::StartNewSession();
```

#### ReAuthenticate
Trigger manual reauthentication with existing stored parameters. This method is primarily useful for testing authentication flows or recovering from authentication issues.

```cpp
// C++ Method Signature
void UAbxr::ReAuthenticate();

// Example Usage
UAbxr::ReAuthenticate();
```

**Note:** All session management methods work asynchronously and will trigger the `OnAuthCompleted` delegate when authentication completes, allowing you to respond to success or failure states.

### Mixpanel Compatibility

The ABXRLib SDK provides compatibility with Mixpanel's tracking patterns, making migration simple and straightforward. You can replace your existing Mixpanel tracking calls with minimal code changes while gaining access to ABXR's advanced XR analytics capabilities.

#### Why Migrate from Mixpanel?

- **XR-Native Analytics**: Purpose-built for spatial computing and immersive experiences
- **Advanced Session Management**: Resume training across devices and sessions  
- **Enterprise Features**: LMS integrations, SCORM/xAPI support, and AI-powered insights
- **Spatial Tracking**: Built-in support for 3D position data and XR interactions
- **Open Source**: No vendor lock-in, deploy to any backend service

**Migration Steps:**
1. Remove Mixpanel references from your Unreal project
2. Configure ABXRLib SDK credentials in Project Settings
3. Replace Mixpanel tracking calls with UAbxr equivalents throughout codebase

```cpp
// Mixpanel → ABXR migration example
// Before: Mixpanel->Track("Plan Selected", Properties);
// After:  UAbxr::Track(TEXT("Plan Selected"), Properties);
```

#### Super Properties

Global properties automatically included in all events:

```cpp
// Set persistent properties (included in all events)
UAbxr::Register(TEXT("user_type"), TEXT("premium"));
UAbxr::Register(TEXT("app_version"), TEXT("1.2.3"));

// Set only if not already set
UAbxr::RegisterOnce(TEXT("user_tier"), TEXT("free"));

// Management
UAbxr::Unregister(TEXT("device_type"));  // Remove specific property
UAbxr::Reset();                          // Clear all super properties
```

Perfect for user attributes, app state, and device information that should be included with every event.

#### Drop-in Compatibility Methods

```cpp
// ABXR compatibility methods for Mixpanel users
UAbxr::Track(TEXT("user_signup"));
TMap<FString, FString> Properties;
Properties.Add(TEXT("amount"), TEXT("29.99"));
Properties.Add(TEXT("currency"), TEXT("USD"));
UAbxr::Track(TEXT("purchase_completed"), Properties);

// Timed events
UAbxr::StartTimedEvent(TEXT("puzzle_solving"));
// ... later ...
UAbxr::Track(TEXT("puzzle_solving")); // Duration automatically included
```

#### Key Advantages Over Mixpanel

| Feature | Mixpanel | ABXRLib SDK |
|---------|----------|-----------|
| **Basic Event Tracking** | ✅ | ✅ |
| **Custom Properties** | ✅ | ✅ |
| **Super Properties** | ✅ | ✅ (Register/RegisterOnce available) |
| **Timed Events** | ✅ | ✅ (StartTimedEvent available) |
| **3D Spatial Data** | ❌ | ✅ (Built-in FVector support) |
| **XR-Specific Events** | ❌ | ✅ (Assessments, Interactions, Objectives) |
| **Session Persistence** | Limited | ✅ (Cross-device, resumable sessions) |
| **Enterprise LMS Integration** | ❌ | ✅ (SCORM, xAPI, major LMS platforms) |
| **Real-time Collaboration** | ❌ | ✅ (Multi-user session tracking) |
| **Open Source** | ❌ | ✅ |

**Migration:** Simply replace Mixpanel calls with UAbxr equivalents throughout your codebase.

---

## Support

### Resources

- **Docs:** [https://help.arborxr.com/](https://help.arborxr.com/)
- **GitHub:** [https://github.com/ArborXR/abxrlib-for-unreal](https://github.com/ArborXR/abxrlib-for-unreal)

### FAQ

#### How do I retrieve my Application ID and Authorization Secret?
Your Application ID can be found in the Web Dashboard under the application details (you must be sure to use the App ID from the specific application you need data sent through). For the Authorization Secret, navigate to Settings > Organization Codes on the same dashboard.

### Troubleshooting

#### Authentication Issues

**Problem: Library fails to authenticate**
- **Solution**: Verify your App ID, Org ID, and Auth Secret are correct in Project Settings
- **Check**: Ensure all three credentials are entered in `Edit > Project Settings > Plugins > ABXR Configuration`
- **Debug**: Check the Unreal Output Log for detailed ABXR authentication error messages

**Problem: Events not being sent**
- **Solution**: Verify authentication completed successfully during app startup
- **Debug**: Monitor the Output Log for ABXR connection status messages
- **Check**: Ensure your event names use snake_case format for best processing

#### Development vs Production Issues

**Problem: Library works in editor but not in packaged builds**
- **Solution**: For production builds, only include App ID in project settings
- **Check**: Remove Org ID and Auth Secret from packaged builds distributed to third parties
- **Alternative**: Use ArborXR-managed devices where credentials are automatically provided

**Problem: Missing credentials on non-managed devices**
- **Solution**: Ensure all three credentials (App ID, Org ID, Auth Secret) are configured for development/testing
- **Check**: Verify credentials are correctly entered in the Unreal Editor project settings
- **Debug**: Check that the plugin configuration is properly saved with the project

#### Integration Issues

**Problem: Plugin not appearing in Project Settings**
- **Solution**: Verify the ABXR plugin is properly installed and enabled in the Plugins menu
- **Check**: Restart Unreal Editor after plugin installation
- **Manual Fix**: Check that plugin files are correctly placed in the Plugins directory

**Problem: Compilation errors with ABXR methods**
- **Solution**: Ensure you're using the correct C++ syntax with `UAbxr::` prefix
- **Check**: Verify `#include` statements for ABXR headers are present
- **Debug**: Check that the ABXR module is properly linked in your build configuration

#### Data and Event Issues  

**Problem: Metadata not appearing in analytics**
- **Solution**: Verify metadata is being added to `TMap<FString, FString>` correctly
- **Debug**: Use UE_LOG to verify metadata contents before sending events
- **Check**: Ensure complex data types are converted to strings properly

**Problem: Telemetry data missing**
- **Solution**: Check that telemetry calls use valid key-value pairs
- **Debug**: Verify telemetry is called with proper `TMap<FString, FString>` format
- **Check**: Ensure telemetry names follow the expected format (e.g., "headset_position")

#### Common Integration Patterns

**Best Practices:**
```cpp
// Verify authentication before sending events
if (UAbxr::ConnectionActive()) {
    UAbxr::Event(TEXT("game_started"));
} else {
    UE_LOG(LogTemp, Warning, TEXT("ABXR not authenticated - events will be skipped"));
}

// Always use TEXT() macro for string literals
TMap<FString, FString> Meta;
Meta.Add(TEXT("player_id"), FString::FromInt(PlayerId));
UAbxr::Event(TEXT("player_joined"), Meta);
```

**Getting Help:**
- Check Unreal Output Log for ABXR-specific messages
- Verify network connectivity and firewall settings
- Test authentication flow in isolation before adding complex features
- Use UE_LOG statements to debug metadata and event data before sending
