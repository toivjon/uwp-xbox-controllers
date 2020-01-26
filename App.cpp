#include <algorithm>
#include <ppltasks.h>
#include <collection.h>
#include <concrt.h>
#include <string>

using namespace Windows::ApplicationModel::Core;
using namespace Windows::Gaming::Input;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;
using namespace Platform::Collections;

using namespace Windows::Data::Xml::Dom;
using namespace Windows::UI::Notifications;
using namespace Windows::System;

using namespace concurrency;

ref class App sealed : public IFrameworkView, IFrameworkViewSource
{
public:
	void OnGamepadAdded(Object^ o, Gamepad^ gamepad) {
		critical_section::scoped_lock lock{ mGamepadsLock };
		if (std::find(begin(mGamepads), end(mGamepads), gamepad) == end(mGamepads)) {
			gamepad->UserChanged += ref new TypedEventHandler<IGameController^, UserChangedEventArgs^>(this, &App::OnUserChanged);
			mGamepads->Append(gamepad);
			auto action = gamepad->User->GetPropertyAsync(KnownUserProperties::AccountName);
			ShowToast("Gamepad added", (String^)create_task(action).get());
		}
	}

	void OnGamepadRemoved(Object^ o, Gamepad^ gamepad) {
		critical_section::scoped_lock lock{ mGamepadsLock};
		auto index = 0u;
		if (mGamepads->IndexOf(gamepad, &index)) {
			mGamepads->RemoveAt(index);
			auto action = gamepad->User->GetPropertyAsync(KnownUserProperties::AccountName);
			ShowToast("Gamepad removed", (String^)create_task(action).get());
		}
	}

	void OnUserChanged(IGameController^ c, UserChangedEventArgs^ args) {
		auto action = args->User->GetPropertyAsync(KnownUserProperties::AccountName);
		ShowToast("User Changed", (String^)create_task(action).get());
	}

	void OnWindowClosed(CoreWindow^ window, CoreWindowEventArgs^ args) {
		mWindowClosed = true;
	}

	virtual void Initialize(CoreApplicationView^ applicationView) {
		WeakReference wr(this);

		// initialize the support to show toast messages.
		mToastNotifier = ToastNotificationManager::CreateToastNotifier();
		mGamepads = ref new Vector<Gamepad^>();

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
			for (auto gamepad: mGamepads) {
				auto reading = gamepad->GetCurrentReading();

				// read thumbstick values which vary between [-1.0, 1.0].
				// note that center position (0.0) may require small "deadzone" to be correctly detected.
				// see https://docs.microsoft.com/en-us/windows/uwp/gaming/gamepad-and-vibration
				auto leftStickX = reading.LeftThumbstickX;
				auto leftStickY = reading.LeftThumbstickY;
				auto rightStickX = reading.RightThumbstickX;
				auto rightStickY = reading.RightThumbstickY;

				// read trigger values which vary between [0.0, 1.0].
				auto leftTrigger = reading.LeftTrigger;
				auto rightTrigger = reading.RightTrigger;

				// controller button states are stored as a bitset so we use bit operations.
				if (GamepadButtons::A == (reading.Buttons & GamepadButtons::A)) {
					if (GamepadButtons::None == (reading.Buttons & GamepadButtons::X)) {
						OutputDebugString(L"Button A Pressed!\r\n");
					} else {
						OutputDebugString(L"Button A and X Pressed!\r\n");
					}
				}

				// show stick values when y button is being pressed.
				if (GamepadButtons::Y == (reading.Buttons & GamepadButtons::Y)) {
					wchar_t buffer[256];
					swprintf_s(buffer, 256, L"leftStick: %0.2f -- %0.2f\r\nrightStick: %0.2f -- %0.2f\r\ntriggers: %0.2f -- %0.2f\r\n",
						leftStickX, leftStickY,
						rightStickX, rightStickY,
						leftTrigger, rightTrigger);
					OutputDebugString(buffer);
				}

				// toggle vibrations based on the directional buttons (DPad) being pressed.
				// Note that XBox One Controller has four vibration motors.
				//   1. Left Motor (strong vibration at lower frequency).
				//   2. Right Motor (gentler vibration at higher frequency).
				//   3. Left Impulse Trigger
				//   4. Right Impultse Trigger
				if (GamepadButtons::DPadDown == (reading.Buttons & GamepadButtons::DPadDown)) {
					GamepadVibration vibration = gamepad->Vibration;
					vibration.LeftMotor = max(0.0, vibration.LeftMotor - 0.1);
					gamepad->Vibration = vibration;
					wchar_t buffer[128];
					swprintf_s(buffer, 128, L"vibration: lMotor: %.1f rMotor: %.1f, lTrigger: %.1f rTrigger: %.1f\r\n",
						vibration.LeftMotor, vibration.RightMotor,
						vibration.LeftTrigger, vibration.RightTrigger);
					OutputDebugString(buffer);
				} else if (GamepadButtons::DPadLeft== (reading.Buttons & GamepadButtons::DPadLeft)) {
					GamepadVibration vibration = gamepad->Vibration;
					vibration.RightMotor = max(0.0, vibration.RightMotor - 0.1);
					gamepad->Vibration = vibration;
					wchar_t buffer[128];
					swprintf_s(buffer, 128, L"vibration: lMotor: %.1f rMotor: %.1f, lTrigger: %.1f rTrigger: %.1f\r\n",
						vibration.LeftMotor, vibration.RightMotor,
						vibration.LeftTrigger, vibration.RightTrigger);
					OutputDebugString(buffer);
				} else if (GamepadButtons::DPadUp == (reading.Buttons & GamepadButtons::DPadUp)) {
					GamepadVibration vibration = gamepad->Vibration;
					vibration.LeftMotor = min(0.75, vibration.LeftMotor + 0.1);
					gamepad->Vibration = vibration;
					wchar_t buffer[128];
					swprintf_s(buffer, 128, L"vibration: lMotor: %.1f rMotor: %.1f, lTrigger: %.1f rTrigger: %.1f\r\n",
						vibration.LeftMotor, vibration.RightMotor,
						vibration.LeftTrigger, vibration.RightTrigger);
					OutputDebugString(buffer);
				} else if (GamepadButtons::DPadRight == (reading.Buttons & GamepadButtons::DPadRight)) {
					GamepadVibration vibration = gamepad->Vibration;
					vibration.RightMotor = min(0.75, vibration.RightMotor + 0.1);
					gamepad->Vibration = vibration;
					wchar_t buffer[128];
					swprintf_s(buffer, 128, L"vibration: lMotor: %.1f rMotor: %.1f, lTrigger: %.1f rTrigger: %.1f\r\n",
						vibration.LeftMotor, vibration.RightMotor,
						vibration.LeftTrigger, vibration.RightTrigger);
					OutputDebugString(buffer);
				} else if (GamepadButtons::LeftShoulder == (reading.Buttons & GamepadButtons::LeftShoulder)) {
					GamepadVibration vibration = gamepad->Vibration;
					vibration.LeftTrigger = min(0.75, vibration.LeftTrigger+ 0.1);
					gamepad->Vibration = vibration;
					wchar_t buffer[128];
					swprintf_s(buffer, 128, L"vibration: lMotor: %.1f rMotor: %.1f, lTrigger: %.1f rTrigger: %.1f\r\n",
						vibration.LeftMotor, vibration.RightMotor,
						vibration.LeftTrigger, vibration.RightTrigger);
					OutputDebugString(buffer);
				} else if (GamepadButtons::RightShoulder == (reading.Buttons & GamepadButtons::RightShoulder)) {
					GamepadVibration vibration = gamepad->Vibration;
					vibration.RightTrigger = min(0.75, vibration.RightTrigger + 0.1);
					gamepad->Vibration = vibration;
					wchar_t buffer[128];
					swprintf_s(buffer, 128, L"vibration: lMotor: %.1f rMotor: %.1f, lTrigger: %.1f rTrigger: %.1f\r\n",
						vibration.LeftMotor, vibration.RightMotor,
						vibration.LeftTrigger, vibration.RightTrigger);
					OutputDebugString(buffer);
				} else if (GamepadButtons::B == (reading.Buttons & GamepadButtons::B)) {
					GamepadVibration vibration;
					gamepad->Vibration = vibration;
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

	void ShowToast(String^ topic, String^ content) {
		// create toast contents in XML.
		auto xml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastText02);
		auto textNodes = xml->GetElementsByTagName("text");
		textNodes->GetAt(0)->AppendChild(xml->CreateTextNode(topic));
		textNodes->GetAt(1)->AppendChild(xml->CreateTextNode(content));

		// create a toast and show it!
		auto toast = ref new ToastNotification(xml);
		mToastNotifier->Show(toast);
	}
private:
	ToastNotifier^	  mToastNotifier;
	critical_section  mGamepadsLock;
	Vector<Gamepad^>^ mGamepads;
	bool			  mWindowClosed;
};

[MTAThread]
int main(Array<String^>^)
{
	CoreApplication::Run(ref new App());
	return 0;
}
