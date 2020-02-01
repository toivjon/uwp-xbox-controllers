# uwp-xbox-controllers
A simple tryout about how Xbox One controllers can be used in a Universal Windows Platform (UWP) application.

## Controller Readings
Controller contains following kind of buttons and sticks which can be read with the UWP Gamepad API.

![alt text](https://github.com/toivjon/uwp-xbox-controllers/blob/master/Images/xbox-controller.svg "Xbox One Controller")

Inputs are read as press buttons, pressure detectors or both as seen from the following list.

| Input            | Type     | Value       | Description                                                   |
| ---------------- | -------- | ----------- | ------------------------------------------------------------- |
| Left Trigger     | Pressure | [0.0, 1.0]  | Reading varies from 0.0 (not pressed) to 1.0 (fully pressed). |
| Right Trigger	   | Pressure | [0.0, 1.0]  | Reading varies from 0.0 (not pressed) to 1.0 (fully pressed). |
| X                | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| Y                | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| A                | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| X                | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| Menu             | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| View             | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| Left Shoulder    | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| Right Shoulder   | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| D-Pad Up		   | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| D-Pad Down       | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| D-Pad Left	   | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| D-Pad Right	   | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| Left Thumbstick  | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| Left Thumbstick  | Pressure | [-1.0, 1.0] | Readings include X-coordinate and Y-coordinates.				|
| Right Thumbstick | Button   |   0 / 1     | Reading is either 0 (not pressed) or 1 (pressed).             |
| Right Thumbstick | Pressure | [-1.0, 1.0] | Readings include X-coordinate and Y-coordinates.              |

As seen from the table, both thumbsticks can be also pressed which allows them to be as buttons as well.

## Features
Here is a small list of the features offered by the UWP Gamepad API.

- Enumerate all currently attached controllers.
- Listen events about gamepads being added or removed.
- Listen events about gamepad user changes.
- Listen events about headset being connected or disconnected.
- Get information about the current user.
  - The (non-roamable) identifier of the user.
  - The authentication status of the user (local/remote/unauthenticated).
  - The type of the user (local-guest/local-user/remote-guest/remote-user).
  - Following values also require "User Account Information" capability.    
	- The user's account name.
	- The user's display name.
	- The user's domain name.
	- The user's first name.
	- The user's last name.
	- The user's principal name.
	- The user's provider name.
	- The user's session initiation protocol URI.
	- The user's picture.
- Get information about the current input readings.
  - 4 pressure detectors.
  - 14 buttons.
- Get information about the gamepad battery state.
  - The rate that the battery is charging (warning: see the Notes section).
  - The design capacity of the battery (warning: see the Notes section).
  - The fully charged capacity of the battery.
  - The remaining capacity of the battery.
  - The status of the battery (charging/discharging/idle/not-present).
- Get information about the connected headset.
  - XAudio endpoint ID for audio capture device.
  - XAudio endpoint ID for audio render device.
  - Headset battery state.
- Check whether the gamepad has a wireless connection.
- Control vibration.

## Notes

- At application startup Gamepad::Gamepads collection is empty (OS will call Gamepad::GamepadAdded after startup).
- Xbox One controllers have four different vibration motors.
  - Left Motor (strong vibration at lower frequency).
  - Right Motor (gentler vibration at higher frequency).
  - Left impulse trigger.
  - Right impulse trigger.
- Center position thumbstick (0.0) may require small "deadzone" to be correctly detected.
- Applications with multi-user support should add <uap:SupportedUsers>multiple</uap:SupportedUsers> to manifest.
- Battery charge rate and design capacity cannot be fully queried from all controllers.