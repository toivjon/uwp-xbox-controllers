using namespace Windows::ApplicationModel::Core;
using namespace Windows::Gaming::Input;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Collections;
using namespace Platform;

using namespace Windows::Data::Xml::Dom;
using namespace Windows::UI::Notifications;

ref class App sealed : public IFrameworkView, IFrameworkViewSource
{
public:
	virtual void Initialize(CoreApplicationView^ applicationView) {
		// initialize the support to show toast messages.
		mToastNotifier = ToastNotificationManager::CreateToastNotifier();

		// add a listener to track whenever a gamepad gets added.
		Gamepad::GamepadAdded += ref new EventHandler<Gamepad^>([=](Object^, Gamepad^ gamepad) {
			int foobar = 0;
		});

		// add a listener to track whenever a gamepad gets removed.
		Gamepad::GamepadRemoved += ref new EventHandler<Gamepad^>([=](Object^, Gamepad^ gamepad) {

		});
	}
		
	virtual void SetWindow(CoreWindow^ window) {

	}

	virtual void Load(String^ entryPoint) {

	}
		
	virtual void Run() {
		auto window = CoreWindow::GetForCurrentThread();
		window->Activate();

		ShowToast("topic!", "description!");

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
