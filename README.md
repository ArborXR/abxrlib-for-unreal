# ABXR SDK for Unreal

The name "ABXR" stands for "Analytics Backbone for XR"—a flexible, open-source foundation for capturing and transmitting spatial, interaction, and performance data in XR. When combined with **ArborXR Insights**, ABXR transforms from a lightweight instrumentation layer into a full-scale enterprise analytics solution—unlocking powerful dashboards, LMS/BI integrations, and AI-enhanced insights.

## Table of Contents
1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Configuration](#configuration)
4. [Sending Data](#sending-data)
   - [Events](#events)
   - [Analytics Event Wrappers](#analytics-event-wrappers-essential-for-all-developers)
   - [Logging](#logging)
   - [Telemetry](#telemetry)
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

The **ABXR SDK for Unreal** is an open-source analytics and data collection library that provides developers with the tools to collect and send XR data to any service of their choice. This library enables scalable event tracking, telemetry, and session-based storage—essential for enterprise and education XR environments.

> 💡 **Quick Start:** Most developers can integrate ABXR SDK and log their first event in under **15 minutes**.

**Why Use ABXR SDK?**

- **Open-Source** & portable to any backend—no vendor lock-in  
- **Quick integration**—track user interactions in minutes  
- **Secure & scalable**—ready for enterprise use cases  
- **Pluggable with ArborXR Insights**—seamless access to LMS/BI integrations, session replays, AI diagnostics, and more

### Core Features

The ABXR SDK provides:
- **Event Tracking:** Monitor user behaviors, interactions, and system events.
- **Spatial & Hardware Telemetry:** Capture headset/controller movement and hardware metrics.
- **Object & System Info:** Track XR objects and environmental state.
- **Storage & Session Management:** Support resumable training and long-form experiences.
- **Logs:** Developer and system-level logs available across sessions.

### Backend Services

The ABXR SDK is designed to work with any backend service that implements the ABXR protocol. Currently supported services include:

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

1. TBD...

---

## Configuration

### Using with ArborXR Insights Early Access

To use the ABXR SDK with ArborXR Insights Early Access program:

#### Get Early Access Credentials
1. Go to the ArborXR Insights Early Access web app and log in (will require [official Early Access sign up](https://arborxr.com/insights-early-access) & onboarding process to access).
2. Grab these three values from the **View Data** screen of the specific app you are configuring:
- App ID
- Organization ID
- Authentication Secret

#### Configure Unreal Project
1. Open `Edit > Project Settings > Plugins > AbxrLib Configuration` in the Unreal Editor.
2. Paste in the Early Access App ID, Org ID, and Auth Secret. All 3 are required if you are testing from Unreal itself.

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

**EventAssessmentStart and EventAssessmentComplete should be considered REQUIRED for proper usage** of the ABXR SDK, as they provide critical insights into user performance and completion rates.

The Analytics Event Functions are specialized versions of the Event method, tailored for common scenarios in XR experiences. These functions help enforce consistency in event logging across different parts of the application and provide valuable data for analytics, user experience optimization, and business intelligence. While they also power integrations with Learning Management System (LMS) platforms, their benefits extend far beyond educational use cases.

#### Assessments
Assessments are intended to track the overall performance of a learner across multiple Objectives and Interactions. 
* Think of it as the learner's score for a specific course or curriculum.
* When the Assessment is complete, it will automatically record and close out the Assessment in the various LMS platforms we support.

```cpp
//C++ List Definition
enum EEventStatus
{
    Pass,
    Fail,
    Complete,
    Incomplete,
    Browsed
}

//C++ Event Method Signatures
public static void EventAssessmentStart(const FString& AssessmentName, TMap<FString, FString>& Meta)

public static void EventAssessmentComplete(const FString& AssessmentName, int Score, EEventStatus Status, TMap<FString, FString>& Meta)

// Example Usage
UAbxr::EventAssessmentStart(TEXT("final_exam"));
UAbxr::EventAssessmentComplete(TEXT("final_exam"), 92, EEventStatus.Pass);
```

#### Objectives
```cpp
//C++ Event Method Signatures
public static void EventObjectiveStart(const FString& ObjectiveName, TMap<FString, FString>& Meta)

public static void EventObjectiveComplete(const FString& ObjectiveName, int Score, EEventStatus Status, TMap<FString, FString> Meta)

// Example Usage
UAbxr::EventObjectiveStart(TEXT("open_valve"));
UAbxr::EventObjectiveComplete(TEXT("open_valve"), 100, EventStatus.Complete);
```

#### Interactions
```cpp
//C++ List Definition
public enum EInteractionType
{
   Null, 
   Bool, // 1 or 0
   Select, // true or false and the result_details value should be a single letter or for multiple choice a,b,c
   Text, // a string 
   Rating, // a single digit value
   Number, // integer
   Matching,
   Performance,
   Sequencing
}

//C++ Event Method Signatures
public static void EventInteractionStart(const FString& InteractionName, TMap<FString, FString>& Meta)

public static void EventInteractionComplete(const FString& InteractionName, EInteractionType InteractionType, const FString& Response, TMap<FString, FString> Meta)

// Example Usage
UAbxr::EventInteractionStart(TEXT("select_option_a"));
UAbxr::EventInteractionComplete(TEXT("select_option_a"), EInteractionType.Select, TEXT("true"));
```

### Other Event Wrappers
#### Levels
```cpp
//C++ Event Method Signatures
public static void EventLevelStart(const FString& AssessmentName) 

public static void EventLevelComplete(const FString& LevelName, int Score)
public static void EventLevelComplete(const FString& LevelName, int Score, TMap<FString, FString> Meta)

// Example Usage
UAbxr::EventLevelStart(TEXT("level_1"));
UAbxr::EventLevelComplete(TEXT("level_1"), 85);

// For flagging critical training events (e.g., skipped safety checks, high-risk errors) for auto-inclusion in the Critical Choices Chart
public static void EventCritical(const FString& Label)
public static void EventCritical(const FString& Label, TMap<FString, FString>& Meta)
```

**Parameters for all Event Wrapper Functions:**
- `AssessmentName/ObjectiveName/InteractionName/LevelName` (FString): The identifier for the assessment, objective, interaction, or level.
- `Score` (int): The numerical score achieved. While typically between 1-100, any integer is valid.
- `Status` (EEventStatus): The basic result of the assessment or objective (Pass, Fail, Complete, Incomplete, Browsed).
- `InteractionType` (EInteractionType): The type of interaction for interaction events.
- `Response` (FString): The user's response for interaction events. For example: TEXT("a"), TEXT("true"), TEXT("correct").
- `Meta` (TMap<FString, FString>): Optional. Additional key-value pairs describing the event.

**Note:** All complete events automatically calculate duration if a corresponding start event was logged.

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
- `Name` (string): The type of telemetry data (e.g., "OS_Version", "Battery_Level", "RAM_Usage").
- `Meta` (TMap<FString, FString>): Key-value pairs of telemetry data.

---

### Metadata Formats

The ABXR SDK for Unreal supports metadata through the `TMap<FString, FString>` parameter in all event and log methods. This native Unreal format provides efficient key-value pairs for describing events and context.

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
The **Module Target** feature enables developers to create single applications with multiple modules, where each module can be its own assignment in an LMS. 
(!!Feature coming soon!!)

### Authentication
Advanced authentication and user management features.
(!!Feature coming soon!!)

### Session Management
Comprehensive session management capabilities for multi-user environments and session continuity.
(!!Feature coming soon!!)

### Mixpanel Compatibility
Full compatibility with Mixpanel's SDK for easy migration.
(!!Feature coming soon!!)

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
- **Check**: Ensure all three credentials are entered in `Edit > Project Settings > Plugins > AbxrLib Configuration`
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
if (UAbxr::IsAuthenticated()) {
    UAbxr::Event(TEXT("game_started"));
} else {
    UE_LOG(LogTemp, Warning, TEXT("ABXR not authenticated - events will be skipped"));
}

// Always use TEXT() macro for string literals
TMap<FString, FString> meta;
meta.Add(TEXT("player_id"), FString::FromInt(PlayerId));
UAbxr::Event(TEXT("player_joined"), meta);
```

**Getting Help:**
- Check Unreal Output Log for ABXR-specific messages
- Verify network connectivity and firewall settings
- Test authentication flow in isolation before adding complex features
- Use UE_LOG statements to debug metadata and event data before sending
