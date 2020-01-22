# uwp-xbox-controllers
A test about how xbox-controllers can be used in a UWP application.

## Notes

- At application startup Gamepad::Gamepads collection is empty (OS will call Gamepad::GamepadAdded after startup).
- Add a listener for Gamepad::GamepadAdded to track gamepad additions.
- Add a listener for Gamepad::GamepadRemoved to track gamepad removals.