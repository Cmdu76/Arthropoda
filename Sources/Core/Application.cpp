#include "Application.hpp"

#include <SFML/Window/Event.hpp>

namespace oe
{

Application::Application()
	: mLog()
	, mWindow()
	, mStates(*this)
	, mLocalization()
	, mTextures()
	, mFonts()
	, mAudioSystem()
	, mFPSCounter(0)
	, mUPSCounter(0)
	, mRunning(true)
{
	mWindowClosedSlot.connect(mWindow.onWindowClosed, [this](const Window* window) { stop(); });

	//ImGui::SFML::Init(mWindow.getHandle());
}

Application::~Application()
{
	if (mWindow.isOpen())
	{
		mWindow.close();
	}

	getAudio().stop();

	//ImGui::SFML::Shutdown();

	#ifdef OE_PLATFORM_ANDROID
		std::exit(0);
	#endif
}

void Application::run()
{
	Clock clock;
	Clock clockFPS;
	Clock clockUPS;
	Time timePerFrame(seconds(1.f / 60.f));
	Time timeSinceLastUpdate(Time::Zero);
	Time second(seconds(1.f));
	U32 tempFPS = 0;
	mFPSCounter = 0;
	U32 tempUPS = 0;
	mUPSCounter = 0;
	while (mRunning)
	{
		Time dt = clock.restart();
		timeSinceLastUpdate += dt;
		if (timeSinceLastUpdate >= timePerFrame)
		{
			timeSinceLastUpdate = oe::Time::Zero;

			// Handle event
			processEvents();

			// Update
			update(timePerFrame);

			// UPS
			tempUPS++;
			if (clockUPS.getElapsedTime() >= second)
			{
				clockUPS.restart();
				mUPSCounter = tempUPS;
				tempUPS = 0;
			}
		}

		// Render
		render();

		// FPS
		tempFPS++;
		if (clockFPS.getElapsedTime() >= second)
		{
			clockFPS.restart();
			mFPSCounter = tempFPS;
			tempFPS = 0;
		}
	}
}

void Application::stop()
{
	mRunning = false;
	getAudio().stop();
}

void Application::popState()
{
	mStates.popState();
}

void Application::clearStates()
{
	mStates.clearStates();
}

TextureHolder& Application::getTextures()
{
	return mTextures;
}

FontHolder& Application::getFonts()
{
	return mFonts;
}

AudioSystem& Application::getAudio()
{
	return mAudioSystem;
}

const U32& Application::getFPSCount() const
{
	return mFPSCounter;
}

const U32& Application::getUPSCount() const
{
	return mUPSCounter;
}

void Application::processEvents()
{
	sf::Event event;
	bool cont = true;
	while (mWindow.pollEvent(event))
	{
		cont = mStates.handleEvent(event);
	}
	if (!cont || !mWindow.isOpen())
	{
		stop();
	}
}

void Application::update(Time dt)
{

	// Update state of musics and sounds
	mAudioSystem.update();

	if (!mStates.update(dt))
	{
		stop();
	}
}

void Application::render()
{
	mWindow.clear();

	mStates.render(mWindow.getHandle());

	mWindow.display();
}

Window& Application::getWindow()
{
	return mWindow;
}

Localization& Application::getLocalization()
{
	return mLocalization;
}

} // namespace oe
