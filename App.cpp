#include <algorithm>
#include <ppltasks.h>
#include <collection.h>
#include <concrt.h>
#include <string>

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Gaming::Input;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI::Core;

// The minimum value for vibration motor power.
constexpr auto MIN_MOTOR_VALUE = 0.0;
// The maximum value for vibration motor power.
constexpr auto MAX_MOTOR_VALUE = 1.0;

// a utility to output debug stuff into Visual Studio console.
inline void Debug(const wchar_t* fmt, ...) {
	wchar_t buffer[256];
	va_list vargs;
	__crt_va_start(vargs, fmt);
	swprintf(buffer, 256, fmt, vargs);
	__crt_va_end(vargs);
	OutputDebugString(buffer);
}

ref struct Controller sealed
{
public:
	Controller(Gamepad^ gamepad) : mGamepad(gamepad) {
		mOldReading = gamepad->GetCurrentReading();
		mNewReading = mOldReading;
	}

	// Store previous readings and read new state into memory.
	void Tick() {
		mOldReading = mNewReading;
		mNewReading = mGamepad->GetCurrentReading();
	}

	// Check whether the target button(s) were just pressed during the tick.
	bool ButtonPressed(GamepadButtons buttons) {
		auto newState = (buttons == (mNewReading.Buttons & buttons));
		auto oldState = (buttons == (mOldReading.Buttons & buttons));
		return newState && !oldState;
	}

	// Check whether the target button(s) were just released during the tick.
	bool ButtonReleased(GamepadButtons buttons) {
		auto newState = (GamepadButtons::None == (mNewReading.Buttons & buttons));
		auto oldState = (GamepadButtons::None == (mOldReading.Buttons & buttons));
		return newState && !oldState;
	}

	// Get a reference on the wrapped gamepad instance.
	Gamepad^ GetGamepad() { return mGamepad; }

	// Get the current state of the controller vibration motors.
	GamepadVibration GetVibration() { return mGamepad->Vibration; }
	// Set the new state for the controller vibration motors.
	void SetVibration(GamepadVibration vibration) { mGamepad->Vibration = vibration; }

	// The position of the left thumbstick in X-axis. Value is between range -1.0 and 1.0.
	double GetLeftThumbstickX() { return mNewReading.LeftThumbstickX; }
	// The position of the left thumbstick in Y-axis. Value is between range -1.0 and 1.0.
	double GetLeftThumbstickY() { return mNewReading.LeftThumbstickY; }
	// The position of the right thumbstick in X-axis. Value is between range -1.0 and 1.0.
	double GetRightThumbstickX() { return mNewReading.RightThumbstickX; }
	// The position of the right thumbstick in Y-axis. Value is between range -1.0 and 1.0.
	double GetRightThumbstickY() { return mNewReading.RightThumbstickY; }
	// The position of the left trigger. Value is between 0.0 (not-pressed) and 1.0 (fully pressed).
	double GetLeftTrigger()	{ return mNewReading.LeftTrigger; }
	// The position of the right trigger. Value is between 0.0 (not-pressed) and 1.0 (fully pressed).
	double GetRightTrigger() { return mNewReading.RightTrigger; }
private:
	Gamepad^		mGamepad;
	GamepadReading	mOldReading;
	GamepadReading	mNewReading;
};

ref class App sealed : public IFrameworkView, IFrameworkViewSource
{
public:
	void OnGamepadAdded(Object^ o, Gamepad^ gamepad) {
		critical_section::scoped_lock lock{ mControllersLock };
		auto it = std::find_if(begin(mControllers), end(mControllers), [gamepad](Controller^ c) {
			return gamepad == c->GetGamepad();
		});
		if (it == end(mControllers)) {
			gamepad->UserChanged += ref new TypedEventHandler<IGameController^, UserChangedEventArgs^>(this, &App::OnUserChanged);
			auto controller = ref new Controller(gamepad);
			mControllers->Append(controller);
			auto action = gamepad->User->GetPropertyAsync(KnownUserProperties::AccountName);
			Debug(L"Gamepad added: %s\r\n", (String^)create_task(action).get());
		}
	}

	void OnGamepadRemoved(Object^ o, Gamepad^ gamepad) {
		critical_section::scoped_lock lock{ mControllersLock };
		for (auto i = 0u; i < mControllers->Size; i++) {
			if (gamepad == mControllers->GetAt(i)->GetGamepad()) {
				mControllers->RemoveAt(i);
				auto action = gamepad->User->GetPropertyAsync(KnownUserProperties::AccountName);
				Debug(L"Gamepad removed: %s\r\n", (String^)create_task(action).get());
				break;
			}
		}
	}

	void OnUserChanged(IGameController^ c, UserChangedEventArgs^ args) {
		auto action = args->User->GetPropertyAsync(KnownUserProperties::AccountName);
		Debug(L"User Changed: %s\r\n", (String^)create_task(action).get());
	}

	void OnWindowClosed(CoreWindow^ window, CoreWindowEventArgs^ args) {
		mWindowClosed = true;
	}

	virtual void Initialize(CoreApplicationView^ applicationView) {
		mControllers = ref new Vector<Controller^>();
		Gamepad::GamepadAdded += ref new EventHandler<Gamepad^>(this, &App::OnGamepadAdded);
		Gamepad::GamepadRemoved += ref new EventHandler<Gamepad^>(this, &App::OnGamepadRemoved);
	}
		
	virtual void SetWindow(CoreWindow^ window) {
		window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);
	}

	virtual void Load(String^ entryPoint) {

	}
		
	virtual void Run() {
		auto window = CoreWindow::GetForCurrentThread();
		window->Activate();
		while (!mWindowClosed) {
			for (auto controller : mControllers) {
				controller->Tick();

				// make some simple checks whether buttons were just pressed.
				if (controller->ButtonPressed(GamepadButtons::A | GamepadButtons::X)) {
					Debug(L"Button A and X were just pressed!");
				} else if (controller->ButtonPressed(GamepadButtons::A)) {
					Debug(L"Button A was Pressed!");
				}

				// show stick values when the Y-button is pressed.
				if (controller->ButtonPressed(GamepadButtons::Y)) {
					Debug(L"leftStick: %0.2f -- %0.2f\r\nrightStick: %0.2f -- %0.2f\r\ntriggers: %0.2f -- %0.2f\r\n",
						controller->GetLeftThumbstickX(), controller->GetLeftThumbstickY(),
						controller->GetRightThumbstickX(), controller->GetRightThumbstickY(),
						controller->GetLeftTrigger(), controller->GetRightTrigger());
				}

				// use direction arrow buttons to adjust vibration motor power.
				if (controller->ButtonPressed(GamepadButtons::DPadDown)) {
					auto vibration = controller->GetVibration();
					vibration.LeftMotor = max(MIN_MOTOR_VALUE, vibration.LeftMotor - 0.1);
					controller->SetVibration(vibration);
				}
				if (controller->ButtonPressed(GamepadButtons::DPadLeft)) {
					auto vibration = controller->GetVibration();
					vibration.RightMotor = max(MIN_MOTOR_VALUE, vibration.RightMotor - 0.1);
					controller->SetVibration(vibration);
				}
				if (controller->ButtonPressed(GamepadButtons::DPadUp)) {
					auto vibration = controller->GetVibration();
					vibration.LeftMotor = min(MAX_MOTOR_VALUE, vibration.LeftMotor + 0.1);
					controller->SetVibration(vibration);
				}
				if (controller->ButtonPressed(GamepadButtons::DPadRight)) {
					auto vibration = controller->GetVibration();
					vibration.RightMotor = min(MAX_MOTOR_VALUE, vibration.RightMotor + 0.1);
					controller->SetVibration(vibration);
				}

				// use should buttons to increase vibration on the triggers.
				if (controller->ButtonPressed(GamepadButtons::LeftShoulder)) {
					auto vibration = controller->GetVibration();
					vibration.LeftTrigger = min(MAX_MOTOR_VALUE, vibration.LeftTrigger + 0.1);
					controller->SetVibration(vibration);
				}
				if (controller->ButtonPressed(GamepadButtons::RightShoulder)) {
					auto vibration = controller->GetVibration();
					vibration.RightTrigger = min(MAX_MOTOR_VALUE, vibration.RightTrigger + 0.1);
					controller->SetVibration(vibration);
				}

				// stop all vibration when the X-button is pressed.
				if (controller->ButtonPressed(GamepadButtons::X)) {
					controller->SetVibration(GamepadVibration());
				}
			}
			window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
		}
	}

	virtual void Uninitialize() {

	}

	virtual IFrameworkView^ CreateView() {
		return ref new App();
	}
private:
	bool				 mWindowClosed;
	critical_section	 mControllersLock;
	Vector<Controller^>^ mControllers;
};

[MTAThread]
int main(Array<String^>^)
{
	CoreApplication::Run(ref new App());
	return 0;
}
