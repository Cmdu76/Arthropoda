#ifndef OE_APPLICATION_HPP
#define OE_APPLICATION_HPP

#include "StateManager.hpp"

#include "../System/Singleton.hpp"
#include "../System/Time.hpp"
#include "../System/Window.hpp"
#include "../System/Log.hpp"
#include "../System/Localization.hpp"
#include "../System/ResourceHolder.hpp"
#include "../System/SFMLResources.hpp"

#include "Systems/AudioSystem.hpp"

#include "../ExtLibs/imgui/imgui.h"
#include "../ExtLibs/imgui/imgui-SFML.h"

namespace oe
{

class Application
{
	public:
		Application();
		~Application();

		Window& getWindow();
		Localization& getLocalization();

		void run();
		void stop();

		template <typename T, typename ... Args>
		void pushState(Args&& ... args);
		void popState();
		void clearStates();

		TextureHolder& getTextures();
		FontHolder& getFonts();
		AudioSystem& getAudio();

		const U32& getFPSCount() const;
		const U32& getUPSCount() const;

		OeSlot(oe::Window, onWindowClosed, mWindowClosedSlot);

	private:	
		void processEvents();
		void update(Time dt);
		void render();

	private:
		Log mLog;
		Window mWindow;
		StateManager mStates;
		Localization mLocalization;
		TextureHolder mTextures;
		FontHolder mFonts;
		AudioSystem mAudioSystem;
		U32 mFPSCounter;
		U32 mUPSCounter;
		bool mRunning;
};

template <typename T, typename ... Args>
void Application::pushState(Args&& ... args)
{
	mStates.pushState<T>(std::forward<Args>(args)...);
}

} // namespace oe

#endif // OE_APPLICATION_HPP
