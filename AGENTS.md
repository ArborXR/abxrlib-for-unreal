# AI instructions – abxrlib-for-unreal

Project-wide instructions are duplicated in sibling ArborXR repos (abxrlib-for-unity, abxrlib-for-webxr); keep shared guidelines in sync when updating.

---

## This repo: ABXRLib SDK for Unreal

Unreal Engine plugin (C++) for analytics, authentication, and data collection in XR apps. Integration with **ArborInsightService** (device-side Android service) is **still in the works**; when complete, this plugin will be a **consumer** of the service APK and client AAR (supplied separately, not built in this repo), similar to abxrlib-for-unity on Android.

### Project setup

- **Structure:** Unreal plugin under `Source/AbxrLib/` (Public/Private), `Content/` (UI assets), `libs/` (e.g. XRDM AAR), `XRDMShimBundle/` for Android bridge.
- **Installation (for app developers):** Clone into project `Plugins/` folder; project must be C++ (see README). Note: README currently points at abxrlib-for-unity URL by mistake—should be abxrlib-for-unreal.
- **Key areas:** `Private/Services/` (Auth, Config, Data, Platform/XRDM), `Private/Subsystems/` (AbxrSubsystem, TelemetrySubsystem), `Private/UI/`, `Private/Types/`.

### How it works (or will work) with other projects

- **ArborInsightService:** Integration with the Android ArborInsightService is **in progress**. When complete, this plugin will consume the same device-side service (via client AAR, e.g. `insights-client-service.aar`, and service APK) as abxrlib-for-unity. This repo does not build or host the service; it only consumes the AAR and APK as supplied. Do not assume access to the ArborInsightService source repo; document only what is exposed to plugins via the AAR and APK.
- **abxrlib-for-unity** is the reference client for the service; Unreal behavior and APIs should align where they share the same backend/service.

Until ArborInsightService support is finished, the Unreal SDK can be used in a standalone mode (e.g. cloud-only or custom backend).

### Build and usage (for app developers)

- **Install:** Clone this plugin into your project’s `Plugins/` folder; project must be C++ (see README). Correct the README if it still points at the abxrlib-for-unity URL—it should reference abxrlib-for-unreal.
- **Android + device service (future):** When ArborInsightService support is complete, obtain the ArborInsightService APK and matching client AAR from your distribution channel and add the AAR to the plugin’s Android libs (e.g. `libs/` or as documented in README).

### Troubleshooting

- **Standalone auth fails:** Confirm configuration (app ID, org ID, auth secret) and backend URLs; check logs for 4xx/5xx and empty credentials.
- **README points to wrong repo:** The README may still mention abxrlib-for-unity; update it to abxrlib-for-unreal for clone/install instructions.
- **Future: AAR/APK mismatch (Android):** When using the device service, ensure the client AAR version matches the installed ArborInsightService APK; mismatches can cause bind or init failures.

### Maintaining this file

Update this file when you make changes that affect the documented architecture, key files, integration with other repos, or troubleshooting. Keep the outline and details in sync with the code.
