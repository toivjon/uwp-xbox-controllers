#include <ppltasks.h>

using namespace Windows::ApplicationModel::Core;
using namespace Windows::Gaming::Input;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Collections;
using namespace Platform;

using namespace Windows::Data::Xml::Dom;
using namespace Windows::UI::Notifications;
using namespace Windows::System;

using namespace concurrency;

ref class App sealed : public IFrameworkView, IFrameworkViewSource
{
public:
	virtual void Initialize(CoreApplicationView^ applicationView) {
		// initialize the support to show toast messages.
		mToastNotifier = ToastNotificationManager::CreateToastNotifier();
		WeakReference wr(this);

		// add a listener to track whenever a gamepad gets added.
		Gamepad::GamepadAdded += ref new EventHandler<Gamepad^>([wr](Object^, Gamepad^ gamepad) {
			auto app = wr.Resolve<App>();

			// let's also the cases where the controller user changes user profile.
			gamepad->UserChanged += ref new TypedEventHandler<IGameController^, UserChangedEventArgs^>([wr](IGameController^, UserChangedEventArgs^ args) {
				auto app = wr.Resolve<App>();
				auto op = args->User->GetPropertyAsync(KnownUserProperties::AccountName);
				create_task(op).then([app](Object^ object) {
					app->ShowToast("User Changed", (String^)object);
				});
			});

			auto op = gamepad->User->GetPropertyAsync(KnownUserProperties::AccountName);
			create_task(op).then([app](Object^ object) {
				app->ShowToast("Gamepad Added", (String^)object);
			});

		});

		// add a listener to track whenever a gamepad gets removed.
		Gamepad::GamepadRemoved += ref new EventHandler<Gamepad^>([=](Object^, Gamepad^ gamepad) {
			auto app = wr.Resolve<App>();
			auto op = gamepad->User->GetPropertyAsync(KnownUserProperties::AccountName);
			create_task(op).then([app](Object^ object) {
				app->ShowToast("Gamepad Removed", (String^)object);
			});
		});
	}
		
	virtual void SetWindow(CoreWindow^ window) {

	}

	virtual void Load(String^ entryPoint) {

	}
		
	virtual void Run() {
		auto window = CoreWindow::GetForCurrentThread();
		window->Activate();

		auto dispathcher = window->Dispatcher;
		dispathcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
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
	ToastNotifier^ mToastNotifier;
};

[MTAThread]
int main(Array<String^>^)
{
	CoreApplication::Run(ref new App());
	return 0;
}
