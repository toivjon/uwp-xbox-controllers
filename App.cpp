#include <algorithm>
#include <ppltasks.h>
#include <collection.h>
#include <concrt.h>

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
