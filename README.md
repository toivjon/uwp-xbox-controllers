# uwp-xbox-controllers
A test about how xbox-controllers can be used in a UWP application.

![alt text](https://github.com/toivjon/uwp-xbox-controllers/blob/master/Images/xbox-controller.svg "Xbox One Controller")

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