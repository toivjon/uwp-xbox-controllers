#include <algorithm>
#include <ppltasks.h>
#include <collection.h>
#include <concrt.h>
#include <string>

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Devices::Power;
using namespace Windows::Gaming::Input;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI::Core;

// The minimum value for vibration motor power.
constexpr auto MIN_MOTOR_VALUE = 0.0;
// The maximum value for vibration motor power.
constexpr auto MAX_MOTOR_VALUE = 1.0;

// ============================================================================
// An utility to create a wide-string presentation of a battery report item.
// ============================================================================
inline std::wstring ToString(BatteryReport^ battery) {
	std::wstring str;
	str += L"state: ";
	str += std::to_wstring((int)battery->Status);

	// ... this throws a read access violation if not supported by the controller
	// str += L"\nchargeRateInMilliwats: ";
	// str += std::to_wstring(battery->ChargeRateInMilliwatts->Value);

	// ... this throws a read access violation if not supported by the controller
	// str += L"\nDesignCapacityInMilliwattHours: ";
	// str += std::to_wstring(battery->DesignCapacityInMilliwattHours->Value);
	
	str += L"\nFullChargeCapacityInMilliwattHours: ";
	str += std::to_wstring(battery->FullChargeCapacityInMilliwattHours->Value);
	str += L"\nRemainingCapacityInMilliwattHours: ";
	str += std::to_wstring(battery->RemainingCapacityInMilliwattHours->Value);
	str += L"\n";
	return str;
}

// ============================================================================
// An utility to create a wide-string presentation of a user item.
// ============================================================================
inline std::wstring ToString(User^ user) {
	std::wstring str;
	str += L"NonRoamableId: ";
	str += user->NonRoamableId->Data();
	str += L"\nAccountName: ";
	str += ((String^)create_task(user->GetPropertyAsync(KnownUserProperties::AccountName)).get())->Data();
	str += L"\nDisplayName: ";
	str += ((String^)create_task(user->GetPropertyAsync(KnownUserProperties::DisplayName)).get())->Data();
	str += L"\nDomainName: ";
	str += ((String^)create_task(user->GetPropertyAsync(KnownUserProperties::DomainName)).get())->Data();
	str += L"\nFirstName: ";
	str += ((String^)create_task(user->GetPropertyAsync(KnownUserProperties::FirstName)).get())->Data();
	str += L"\nLastName: ";
	str += ((String^)create_task(user->GetPropertyAsync(KnownUserProperties::LastName)).get())->Data();
	str += L"\nGuestHost: ";
	str += ((String^)create_task(user->GetPropertyAsync(KnownUserProperties::GuestHost)).get())->Data();
	str += L"\nPrincipalName: ";
	str += ((String^)create_task(user->GetPropertyAsync(KnownUserProperties::PrincipalName)).get())->Data();
	str += L"\nSessionInitiationProtocolUri: ";
	str += ((String^)create_task(user->GetPropertyAsync(KnownUserProperties::SessionInitiationProtocolUri)).get())->Data();
	str += L"\nUserType: ";
	switch (user->Type) {
	case UserType::LocalGuest:
		str += L"LocalGuest";
		break;
	case UserType::LocalUser:
		str += L"LocalUser";
		break;
	case UserType::RemoteGuest:
		str += L"RemoteGuest";
		break;
	case UserType::RemoteUser:
		str += L"RemoteUser";
		break;
	}
	str += L"\nAuthenticationStatus: ";
	switch (user->AuthenticationStatus) {
	case UserAuthenticationStatus::LocallyAuthenticated:
		str += L"LocallyAuthenticated";
		break;
	case UserAuthenticationStatus::RemotelyAuthenticated:
		str += L"RemotelyAuthenticated";
		break;
	case UserAuthenticationStatus::Unauthenticated:
		str += L"Unauthenticated";
		break;
	}
	str += L"\n";
	return str;
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

	// Check whether the gamepad is in wireless state.
	bool IsWireless() { return mGamepad->IsWireless; }
	// Get the report about the controllers battery state.
	BatteryReport^ GetPower() { return mGamepad->TryGetBatteryReport(); }

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
	// ========================================================================
	// Listener function for a gamepad added events.
	//
	// This function is called when a gamepad is being added. Here we store the
	// reference to added gamepad into our controller collection which allows
	// us to easily manage and trace our controllers and assign them to actors.
	// ========================================================================
	void OnGamepadAdded(Object^ o, Gamepad^ gamepad) {
		critical_section::scoped_lock lock{ mControllersLock };
		auto it = std::find_if(begin(mControllers), end(mControllers), [gamepad](Controller^ c) {
			return gamepad == c->GetGamepad();
		});
		if (it == end(mControllers)) {
			gamepad->UserChanged += ref new TypedEventHandler<IGameController^, UserChangedEventArgs^>(this, &App::OnUserChanged);
			mControllers->Append(ref new Controller(gamepad));
			OutputDebugString(L"=== Gamepad Added ===\n");
			OutputDebugString(L"User information:\n");
			OutputDebugString(::ToString(gamepad->User).c_str());
			OutputDebugString(L"Wireless: ");
			OutputDebugString(gamepad->IsWireless ? L"yes" : L"no");
			OutputDebugString(L"\nBattery information:\n");
			OutputDebugString(::ToString(gamepad->TryGetBatteryReport()).c_str());
		}
	}

	// ========================================================================
	// Listener function for a gamepad removed events.
	//
	// This function is called when a gamepad is being removed. Here we remove
	// the gamepad reference from our controller collection so we don't want to
	// trace or manage it anymore within our application.
	// ========================================================================
	void OnGamepadRemoved(Object^ o, Gamepad^ gamepad) {
		critical_section::scoped_lock lock{ mControllersLock };
		for (auto i = 0u; i < mControllers->Size; i++) {
			if (gamepad == mControllers->GetAt(i)->GetGamepad()) {
				mControllers->RemoveAt(i);
				OutputDebugString(L"=== Gamepad Removed ===\n");
				break;
			}
		}
	}

	// ========================================================================
	// Listener function for a gamepad user changed events.
	//
	// This function is called when a gamepad user is being changed. Here we're
	// able to perform all kinds of things related to user. For example we can
	// update the name of the player in the game UI or other UX and UI things.
	// ========================================================================
	void OnUserChanged(IGameController^ c, UserChangedEventArgs^ args) {
		OutputDebugString(L"=== User Changed ===\n");
		OutputDebugString(::ToString(args->User).c_str());
	}

	// ========================================================================
	// Listener function for window being closed.
	//
	// This function stores the window closed flag so our main loop will exit.
	// ========================================================================
	void OnWindowClosed(CoreWindow^ window, CoreWindowEventArgs^ args) {
		mWindowClosed = true;
	}

	// ============================================================================
	// Initializes the application view.
	//
	// This function is called when the application is launched by the runtime. We
	// may perform all kinds of initialization logics here before actual execution.
	// ============================================================================
	virtual void Initialize(CoreApplicationView^ applicationView) {
		mControllers = ref new Vector<Controller^>();
		Gamepad::GamepadAdded += ref new EventHandler<Gamepad^>(this, &App::OnGamepadAdded);
		Gamepad::GamepadRemoved += ref new EventHandler<Gamepad^>(this, &App::OnGamepadRemoved);
	}

	// ============================================================================
	// Assigns the window for the application.
	//
	// This function is called after the Initialize function has been called. Here
	// we have opportunity to set up window and display event handling operations.
	// ============================================================================
	virtual void SetWindow(CoreWindow^ window) {
		window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);
	}

	// ============================================================================
	// Loads view related resources.
	//
	// This function is called by the runtime when the application should load all
	// initial scene resources or to restore a previously saved application state.
	// ============================================================================
	virtual void Load(String^ entryPoint) {
		// ... nothing
	}

	// ============================================================================
	// Run the application view.
	//
	// This function is called when the application is set to running state. This
	// function typically contains the main loop for the game application.
	// ============================================================================
	virtual void Run() {
		auto window = CoreWindow::GetForCurrentThread();
		window->Activate();
		while (!mWindowClosed) {
			for (auto controller : mControllers) {
				controller->Tick();

				// make some simple checks whether buttons were just pressed.
				if (controller->ButtonPressed(GamepadButtons::A | GamepadButtons::X)) {
					OutputDebugString(L"Button A and X were just pressed!");
				} else if (controller->ButtonPressed(GamepadButtons::A)) {
					OutputDebugString(L"Button A was Pressed!");
				}

				// show stick values when the Y-button is pressed.
				if (controller->ButtonPressed(GamepadButtons::Y)) {
					OutputDebugString(L"leftStick-x: ");
					OutputDebugString(std::to_wstring(controller->GetLeftThumbstickX()).c_str());
					OutputDebugString(L"\nleftStick-y: ");
					OutputDebugString(std::to_wstring(controller->GetLeftThumbstickY()).c_str());
					OutputDebugString(L"\nrightStick-x: ");
					OutputDebugString(std::to_wstring(controller->GetRightThumbstickX()).c_str());
					OutputDebugString(L"\nrightStick-y: ");
					OutputDebugString(std::to_wstring(controller->GetRightThumbstickY()).c_str());
					OutputDebugString(L"\nrightTrigger: ");
					OutputDebugString(std::to_wstring(controller->GetRightTrigger()).c_str());
					OutputDebugString(L"\nleftTrigger: ");
					OutputDebugString(std::to_wstring(controller->GetLeftTrigger()).c_str());
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

	// ============================================================================
	// Release and dispose reserved resources.
	//
	// Note that this function is not always called when application is exited. It 
	// seems that this function is not commonly used but just kept as a stump.
	// ============================================================================
	virtual void Uninitialize() {
		// ... nothing
	}

	// ============================================================================
	// A factory function to create a view.
	//
	// CoreApplication will use this function to create a view for the application.
	// ============================================================================
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
