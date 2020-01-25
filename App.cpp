using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Platform;

ref class App sealed : public IFrameworkView, IFrameworkViewSource
{
public:
	virtual void Initialize(CoreApplicationView^ applicationView) {

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
};

[MTAThread]
int main(Array<String^>^)
{
	CoreApplication::Run(ref new App());
	return 0;
}
