# AI instructions – abxrlib-for-unreal

Team-wide instructions are duplicated in sibling ArborXR repos (abxrlib-for-unity, abxrlib-for-webxr); keep shared guidelines in sync when updating.

---

## This repo: ABXRLib SDK for Unreal

Unreal Engine plugin (C++) for analytics, authentication, and data collection in XR apps. Integration with **ArborInsightService** (device-side Android service) is **still in the works**; when complete, this plugin will be a **consumer** of the service APK and client AAR (supplied separately, not built in this repo), similar to abxrlib-for-unity on Android.

### Project setup

- **Structure:** Unreal plugin under `Source/AbxrLib/` (Public/Private), `Content/` (UI assets), `libs/` (e.g. XRDM AAR), `XRDMShimBundle/` for Android bridge.
- **Installation (for app developers):** Clone into project `Plugins/` folder; project must be C++ (see README). Note: README currently points at abxrlib-for-unity URL by mistake—should be abxrlib-for-unreal.
- **Key areas:** `Private/Services/` (Auth, Config, Data, Platform/XRDM), `Private/Subsystems/` (AbxrSubsystem, TelemetrySubsystem), `Private/UI/`, `Private/Types/`.

### How it works (or will work) with other projects

- **ArborInsightService:** Integration with the Android ArborInsightService is **in progress**. When complete, this plugin will consume the same device-side service (via client AAR/bridge) as abxrlib-for-unity. This repo does not build or host the service; it only consumes the AAR and APK as supplied.
- **abxrlib-for-unity** is the reference client for the service; Unreal behavior and APIs should align where they share the same backend/service.

Until ArborInsightService support is finished, the Unreal SDK can be used in a standalone mode (e.g. cloud-only or custom backend).
