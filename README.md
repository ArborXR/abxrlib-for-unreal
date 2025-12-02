# ABXRLib SDK for Unreal

The name "ABXR" stands for "Analytics Backbone for XR"—a flexible, open-source foundation for capturing and transmitting spatial, interaction, and performance data in XR. When combined with **ArborXR Insights**, ABXR transforms from a lightweight instrumentation layer into a full-scale enterprise analytics solution—unlocking powerful dashboards, LMS/BI integrations, and AI-enhanced insights.

## Table of Contents
1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Configuration](#configuration)
4. [Quick Start](#quick-start)
5. [Full Documentation](#full-documentation)
6. [Support](#support)

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

1. Make sure your project can build C++
- If your project is Blueprint-only:
  1. Add C++ Class → pick anything (e.g., Actor) → Create Class
  2. Let Unreal generate the Visual Studio / Rider / Xcode project and compile once
- Now your project is a C++ project, which is required for C++ plugins
2. Close the Unreal Editor
3. In your project folder (where MyGame.uproject is), create a /Plugins directory if it doesn't already exist
4. Into this folder, clone or download the plugin from the GitHub repository - https://github.com/ArborXR/abxrlib-for-unity
5. Right-click your .uproject file → Generate Visual Studio project files (or equivalent)
6. Open the generated .sln file in your IDE of choice and build

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

## Quick Start

### Essential Event Tracking (Required)

**Assessment events are required** to activate grading dashboards and LMS integration. Send these events to track training completion, scores, and pass/fail status.

```cpp
// When training starts
UAbxr::EventAssessmentStart(TEXT("safety_training"));

// When training completes
UAbxr::EventAssessmentComplete(TEXT("safety_training"), 92, EEventStatus::Pass);
// or
UAbxr::EventAssessmentComplete(TEXT("safety_training"), 25, EEventStatus::Fail);
```

**Assessment Complete Parameters:**
- `Score` (second parameter) takes a 0-100 value
- The `EEventStatus` enum has `Pass`, `Fail`, `Complete`, `Incomplete`, `Browsed`, `NotAttempted` options

### Tracking Objectives (Recommended)

For more detailed tracking, you can also track specific objectives within your training:

```cpp
// To mark a specific objective start
UAbxr::EventObjectiveStart(TEXT("open_valve"));

// When the objective is complete
UAbxr::EventObjectiveComplete(TEXT("open_valve"), 100, EEventStatus::Complete);
```

---

## Full Documentation

For comprehensive documentation covering all features, advanced topics, and detailed examples, visit:

- **[ArborXR Insights Documentation](https://developers.arborxr.com/docs/insights)** - Main documentation hub
- **[Complete SDK Documentation](https://developers.arborxr.com/docs/insights/full-documentation/)** - Full API reference and feature documentation

The full documentation includes:
- Complete event tracking API (Events, Analytics Event Wrappers, Timed Events)
- Advanced features (Module Targets, Authentication, Session Management)
- Storage, Telemetry, Logging, and AI Integration
- Mixpanel compatibility guide
- Troubleshooting and best practices
- Platform-specific examples and code samples

---

## Support

### Resources

- **Docs:** [https://help.arborxr.com/](https://help.arborxr.com/)
- **GitHub:** [https://github.com/ArborXR/abxrlib-for-unreal](https://github.com/ArborXR/abxrlib-for-unreal)

### FAQ

#### How do I retrieve my Application ID and Authorization Secret?
Your Application ID can be found in the Web Dashboard under the application details (you must be sure to use the App ID from the specific application you need data sent through). For the Authorization Secret, navigate to Settings > Organization Codes on the same dashboard.

For more troubleshooting help and detailed FAQs, see the [full documentation](https://developers.arborxr.com/docs/insights/full-documentation/).
