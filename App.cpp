#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Gaming.Input.h>
#include <winrt/Windows.UI.Core.h>

#include <algorithm>
#include <concrt.h>
#include <vector>

using namespace concurrency;
using namespace winrt;
using namespace Windows;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Gaming::Input;
using namespace Windows::UI;
using namespace Windows::UI::Core;

struct App : winrt::implements<App, IFrameworkViewSource, IFrameworkView>
{
	// an array of available controllers.
	std::vector<Gamepad> mGamepads;
	critical_section	 mGamepadsLock;

    IFrameworkView CreateView()
    {
        return *this;
    }

    void Initialize(CoreApplicationView const&)
    {
		// add a listener to track whether a gamepad gets attached.
		Gamepad::GamepadAdded([this](auto&, auto& gamepad) {
			critical_section::scoped_lock lock{ mGamepadsLock };
			if (std::find(mGamepads.begin(), mGamepads.end(), gamepad) == mGamepads.end()) {
				mGamepads.push_back(gamepad);
			}
		});

		// add a listener to track whether a gamepad gets detached.
		Gamepad::GamepadRemoved([this](auto&, auto& gamepad) {
			critical_section::scoped_lock lock{ mGamepadsLock };
			auto it = std::find(mGamepads.begin(), mGamepads.end(), gamepad);
			if (it != mGamepads.end()) {
				mGamepads.erase(it);
			}
		});
    }

    void Load(const hstring&)
    {
		// ... nothing
    }

    void Uninitialize()
    {
		// ... nothing
    }

    void Run()
    {
		auto window = CoreWindow::GetForCurrentThread();
        window.Activate();

        auto dispatcher = window.Dispatcher();
        dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
    }

    void SetWindow(CoreWindow const&)
    {
		// ... TODO
    }
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    CoreApplication::Run(make<App>());
}
