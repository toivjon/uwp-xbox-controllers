# uwp-xbox-controllers
A test about how xbox-controllers can be used in a UWP application.

## Features

- A support to manage and track headsets attached to controllers.
  - Gamepad.Headset
  - Gamepad.HeadsetConnected
  - Gamepad.HeadsetDisconnected
- 

## Notes

- At application startup Gamepad::Gamepads collection is empty (OS will call Gamepad::GamepadAdded after startup).
- Add a listener for Gamepad::GamepadAdded to track gamepad additions.
- Add a listener for Gamepad::GamepadRemoved to track gamepad removals.
- Direct access to controller headset with Gamepad.Headset.
  - Contains XAudio endpoint ID for audio capture device.
  - Contains XAudio endpoint ID for audia render device.
  - Ability to query info about the headset's current battery state.
- Add a listener to Gamepad.HeadsetConnected to track headset attachment.
- Add a listener to Gamepad.HeadsetDisconnected to track headset detachment.
- Ability to check whether the gamepad has wireless connection.
- 