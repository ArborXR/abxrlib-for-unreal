# ABXR SDK for Unreal

The name "ABXR" stands for "Analytics Backbone for XR"—a flexible, open-source foundation for capturing and transmitting spatial, interaction, and performance data in XR. When combined with **ArborXR Insights**, ABXR transforms from a lightweight instrumentation layer into a full-scale enterprise analytics solution—unlocking powerful dashboards, LMS/BI integrations, and AI-enhanced insights.

## Table of Contents
1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Configuration](#configuration)
4. [Sending Data](#sending-data)
5. [FAQ](#faq)
6. [Troubleshooting](#troubleshooting)
7. [Contact](#contact)

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

### Event Methods
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

### Event Wrappers (for LMS Compatibility)
-The LMS Event Functions are specialized versions of the Event method, tailored for common scenarios in XR experiences. These functions help enforce consistency in event logging across different parts of the application and are crucial for powering integrations with Learning Management System (LMS) platforms. By using these standardized wrapper functions, developers ensure that key events like starting or completing levels, assessments, or interactions are recorded in a uniform format. This consistency not only simplifies data analysis but also facilitates seamless communication with external educational systems, enhancing the overall learning ecosystem.

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

## FAQ

### Q: How do I retrieve my Application ID and Authorization Secret?
A: Your Application ID can be found in the Web Dashboard under the application details (you must be sure to use the App ID from the specific application you need data sent through). For the Authorization Secret, navigate to Settings > Organization Codes on the same dashboard.

## Troubleshooting

---

## Persisting User State with ArborXR Insights

The ABXR SDK includes a built-in storage interface that enables persistent session data across XR devices. This is ideal for applications with long-form content, resumable training, or user-specific learning paths.

When integrated with **ArborXR Insights**, session state data is securely stored and can be retrieved from any device, enabling users to resume exactly where they left off. 

### Benefits of Using ArborXR Insights for Storage:
- Cross-device continuity and resuming sessions
- Secure, compliant storage (GDPR, HIPAA-ready)
- Configurable behaviors (e.g., `keepLatest`, append history)
- Seamless AI and analytics integration for stored user states

To use this feature, simply call the storage functions provided in the SDK (`SetStorageEntry`, `GetStorageEntry`, etc.). These entries are automatically synced with ArborXR’s cloud infrastructure, ensuring consistent data across sessions.

---

## ArborXR Insights Web Portal & API

For dashboards, analytics queries, impersonation, and integration management, use the **ArborXR Insights User API**, accessible through the platform's admin portal.

Example features:
- Visualize training completion & performance by cohort
- Export SCORM/xAPI-compatible results
- Query trends in interaction data

Endpoints of note:
- `/v1/analytics/dashboard`
- `/v1/admin/system/organization/{org_id}`
- `/v1/analytics/data`

---

## Support

- **Docs:** [https://help.arborxr.com/](https://help.arborxr.com/)
- **GitHub:** [https://github.com/ArborXR/abxrlib-for-unreal](https://github.com/ArborXR/abxrlib-for-unreal)
