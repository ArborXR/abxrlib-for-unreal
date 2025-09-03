# PIN Pad Widget - Usage Examples

This document shows how to use the programmatically generated PIN pad in AbxrLib for Unreal.

## Basic Usage

### 1. Using the AbxrLib PresentKeyboard Method

The simplest way to show a PIN pad is through the existing `PresentKeyboard` method:

```cpp
// C++ Usage
UAbxr::PresentKeyboard(TEXT("Enter your 6-digit PIN"), TEXT("assessmentPin"));

// Blueprint Usage (if exposed)
// Call "Present Keyboard" node with:
// - Prompt Text: "Enter your 6-digit PIN"
// - Keyboard Type: "assessmentPin"
```

### 2. Direct PIN Pad Widget Usage

For more control, you can directly create and show the PIN pad widget:

```cpp
#include "PinPadWidget.h"

// In your function/method:
UWorld* World = GetWorld();
if (World)
{
    UPinPadWidget* PinPad = UPinPadWidget::ShowPinPad(
        World, 
        FText::FromString(TEXT("Enter your security PIN"))
    );
    
    // Handle PIN acceptance
    PinPad->OnPinAccepted.AddLambda([](const FString& EnteredPin)
    {
        UE_LOG(LogTemp, Log, TEXT("User entered PIN: %s"), *EnteredPin);
        
        // Process the PIN (e.g., authenticate)
        Authentication::KeyboardAuthenticate(EnteredPin);
        
        // Or handle it however you need
        // ValidatePIN(EnteredPin);
    });
}
```

## Features

### PIN Pad Layout
The PIN pad creates a standard 3x4 button grid:
```
1  2  3
4  5  6
7  8  9
⌫  0  ✓
```

### Key Features:
- **Visual Masking**: Entered digits are displayed as dots (•••••) for security
- **Mouse/Touch Support**: Click buttons to enter digits
- **Keyboard Support**: Use number keys (0-9) and numpad to enter digits
- **Delete Support**: Backspace/Delete keys or ⌫ button to remove digits
- **Submit Support**: Enter key or ✓ button to submit the PIN
- **Maximum Length**: Configurable (default: 6 digits)
- **Proper Input Mode**: Automatically manages mouse cursor and input focus

### Input Methods:
1. **Mouse/Touch**: Click the on-screen buttons
2. **Keyboard Numbers**: Use 0-9 keys  
3. **Numpad**: Use numpad 0-9 keys
4. **Backspace/Delete**: Remove last digit
5. **Enter**: Submit the current PIN

## Integration with Authentication

The PIN pad integrates seamlessly with the existing AbxrLib authentication system:

```cpp
// When user submits PIN, it automatically calls:
Authentication::KeyboardAuthenticate(EnteredPin);
```

This matches the behavior of the Unity version, making it easy to maintain consistent authentication flows across platforms.

## Customization

### Styling
The PIN pad uses a dark theme matching the existing InputDialogWidget:
- Dark blue background (15, 31, 56)
- Purple border (88, 91, 211)  
- Button styling with hover/pressed states
- White text on colored buttons

### Size and Layout
- Card width: 400px
- Button grid with 10px spacing
- Rounded corners on buttons
- Centered layout with semi-transparent background

### PIN Length
The maximum PIN length is configurable (default: 6 digits):

```cpp
// In PinPadWidget.h, modify:
static constexpr int32 MaxPinLength = 6; // Change this value
```

## Error Handling

The PIN pad includes proper error handling:
- Validates world context before showing
- Gracefully handles missing components
- Restores previous input mode when closed
- Prevents crashes in dedicated server builds

## Comparison with Unity

This Unreal implementation provides the same functionality as the Unity version:

| Feature | Unity | Unreal |
|---------|-------|--------|
| Dynamic Generation | ✅ | ✅ |
| Keyboard Input | ✅ | ✅ |
| Mouse/Touch Input | ✅ | ✅ |
| Visual Masking | ✅ | ✅ |
| Authentication Integration | ✅ | ✅ |
| Customizable Prompts | ✅ | ✅ |

## Next Steps

To extend this further, you could:
1. Add sound effects for button presses
2. Add haptic feedback for VR controllers
3. Implement different PIN pad layouts
4. Add animation transitions
5. Support for different themes/styles
