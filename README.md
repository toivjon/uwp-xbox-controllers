# uwp-xbox-controllers
A simple tryout about how Xbox One controllers can be used in a Universal Windows Platform (UWP) application.

## Controller Readings
Controller contains following kind of buttons and sticks which can be read with the UWP Gamepad API.

![alt text](https://github.com/toivjon/uwp-xbox-controllers/blob/master/Images/xbox-controller.svg "Xbox One Controller")

Inputs are read as press buttons, pressure detectors or both as seen from the following list.

| Input            | Type              | Description                                   |
| ---------------- | ----------------- | --------------------------------------------- |
| Left Trigger     | Float [0.0, 1.0]  | Value is the given pressure ranging from 0.0 (not pressed) to 1.0 (fully pressed). |
| Right Trigger	   | Float [0.0, 1.0]  | Value is the given pressure ranging from 0.0 (not pressed) to 1.0 (fully pressed). |
| X                | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| Y                | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| A                | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| X                | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| Menu             | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| View             | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| Left Shoulder    | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| Right Shoulder   | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| D-Pad Up		   | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| D-Pad Down       | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| D-Pad Left	   | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| D-Pad Right	   | Button            | Value is a simple bit indicating whether the button is pressed or not.             |
| Left Thumbstick  | Button            | Thumbstick can be used as a button and stick. |
| Left Thumbstick  | Float [-1.0, 1.0] | Thumbstick can be used as a button and stick. |
| Right Thumbstick | Button            | Thumbstick can be used as a button and stick. |
| Right Thumbstick | Float [-1.0, 1.0] | Thumbstick can be used as a button and stick. |

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