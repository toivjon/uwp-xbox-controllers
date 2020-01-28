# uwp-xbox-controllers
A simple tryout about how Xbox One controllers can be used in a UWP application.

## Controller
Xbox One controller contains following kind of buttons and sticks.

![alt text](https://github.com/toivjon/uwp-xbox-controllers/blob/master/Images/xbox-controller.svg "Xbox One Controller")

Inputs are read as simple buttons or/and as float sticks seen from the following list.

| Input            | Type              | Description                                   |
| ---------------- | ----------------- | --------------------------------------------- |
| Left Trigger     | Stick [0.0, 1.0]  | Input based on the pressure given to button.  |
| Right Trigger	   | Stick [0.0, 1.0]  | Input based on the pressure given to button.  |
| X                | Button            | Simple pressed / not pressed toggle button.   |
| Y                | Button            | Simple pressed / not pressed toggle button.   |
| A                | Button            | Simple pressed / not pressed toggle button.   |
| X                | Button            | Simple pressed / not pressed toggle button.   |
| Menu             | Button            | Simple pressed / not pressed toggle button.   |
| View             | Button            | Simple pressed / not pressed toggle button.   |
| Left Shoulder    | Button            | Simple pressed / not pressed toggle button.   |
| Right Shoulder   | Button            | Simple pressed / not pressed toggle button.   |
| D-Pad Up		   | Button            | Simple pressed / not pressed toggle button.   |
| D-Pad Down       | Button            | Simple pressed / not pressed toggle button.   |
| D-Pad Left	   | Button            | Simple pressed / not pressed toggle button.   |
| D-Pad Right	   | Button            | Simple pressed / not pressed toggle button.   |
| Left Thumbstick  | Button            | Thumbstick can be used as a button and stick. |
| Left Thumbstick  | Stick [-1.0, 1.0] | Thumbstick can be used as a button and stick. |
| Right Thumbstick | Button            | Thumbstick can be used as a button and stick. |
| Right Thumbstick | Stick [-1.0, 1.0] | Thumbstick can be used as a button and stick. |

## Features
Here's a small list of the controller API features.

- Enumerate and list all currently attached controllers (Gamepad::Gamepads).
- Listen events about gamepads getting added (Gamepad::GamepadAdded).
- Listen events about gamepads getting removed (Gamepad::GamepadRemoved).
- Listen events about gamepad user being changed (Gamepad::UserChanged).
- Listen events about gamepad headset being connected (Gamepad::HeadsetConnected).
- Listen events about gamepad headset being disconnected (Gamepad::HeadsetDisconnected).
- Get information about the connected headset (Gamepad::Headset).
  - Contains XAudio endpoint ID for audio capture device.
  - Contains XAudio endpoint ID for audia render device.
  - Ability to query info about the headset's current battery state.
- Check whether the gamepad has wireless connection (Gamepad::IsWireless)
- Get information about the attached user profile (Gamepad::User).
- Check the state of the controller buttons and stick readings (Gamepad::GetCurrentReading).
- Check the controller's current batter level.
- Control the controller's vibration.

## Notes

- At application startup Gamepad::Gamepads collection is empty (OS will call Gamepad::GamepadAdded after startup).
- Xbox One controllers have four different vibration motors.
  - Left Motor (strong vibration at lower frequency).
  - Right Motor (gentler vibration at higher frequency).
  - Left impulse trigger.
  - Right impulse trigger.
- Center position thumbstick (0.0) may require small "deadzone" to be correctly detected.
  - https://docs.microsoft.com/en-us/windows/uwp/gaming/gamepad-and-vibration