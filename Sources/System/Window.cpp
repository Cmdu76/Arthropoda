#include "Window.hpp"
#include "Log.hpp"

namespace oe
{

Window::Window()
	: mWindow(*this)
{
	init();
}

Window::Window(sf::VideoMode mode, const std::string& title, sf::Uint32 style, const sf::ContextSettings& settings)
	: mWindow(*this)
{
	init();
	create(mode, title, style, settings);
}

void Window::create()
{
	if (isFullscreen())
	{
		create(mFullscreenVideoMode, mTitle, sf::Style::Fullscreen, mSettings);
	}
	else
	{
		create(mNonFullscreenVideoMode, mTitle, mNonFullscreenStyle, mSettings);
	}
}

void Window::create(sf::VideoMode mode, const std::string& title, sf::Uint32 style, const sf::ContextSettings& settings)
{
	#ifdef SFML_SYSTEM_WINDOWS
		if ((style & sf::Style::Fullscreen) != 0)
		{
			mFullscreen = true;
			if (sf::VideoMode::getFullscreenModes().size() > 0)
			{
				mFullscreenVideoMode = sf::VideoMode::getFullscreenModes()[0];
			}
			else
			{
				error("No fullscreen mode available");
				mFullscreenVideoMode = sf::VideoMode();
			}
		}
		else
		{
			mFullscreen = false;
			if (mode.isValid())
			{
				mNonFullscreenVideoMode = mode;
			}
			else
			{
				error("Invalid video mode");
				mNonFullscreenVideoMode = sf::VideoMode::getDesktopMode();
			}
		}
		mTitle = title;
		if (!mFullscreen)
		{
			mNonFullscreenStyle = style;
		}
		mSettings = settings;
	#else
		mFullscreen = true;
		mFullscreenVideoMode = sf::VideoMode::getDesktopMode();
		mTitle = "";
		mSettings = settings;
	#endif

	if (mFullscreen)
	{
		mWindow.create(mFullscreenVideoMode, mTitle, sf::Style::Fullscreen, settings);
	}
	else
	{
		mWindow.create(mNonFullscreenVideoMode, mTitle, mNonFullscreenStyle, settings);
	}
	mWindow.init();
	onWindowResized(this, (U32)getSize().x, (U32)getSize().y);
	if (mIcon.getSize().x > 0 && mIcon.getSize().y > 0)
	{
		mWindow.setIcon(mIcon.getSize().x, mIcon.getSize().y, mIcon.getPixelsPtr());
	}

	mWindow.setMouseCursorVisible(mCursor == Cursor::Default);

	mVisible = true;
}

void Window::toFullscreen()
{
	create(sf::VideoMode(), mTitle, sf::Style::Fullscreen, mSettings);
}

void Window::toNonFullscreen()
{
	create(mNonFullscreenVideoMode, mTitle, mNonFullscreenStyle, mSettings);
}

void Window::toggleFullscreen()
{
	if (mFullscreen)
	{
		toNonFullscreen();
	}
	else
	{
		toFullscreen();
	}
}

void Window::close()
{
	mVisible = false;
	mWindow.close();
	onWindowClosed(this);
}

bool Window::isFullscreen() const
{
	return mFullscreen;
}

const sf::VideoMode& Window::getVideoMode() const
{
	return (mFullscreen) ? mFullscreenVideoMode : mNonFullscreenVideoMode;
}

sf::Uint32 Window::getStyle() const
{
	return (mFullscreen) ? sf::Style::Fullscreen : mNonFullscreenStyle;
}

const std::string& Window::getTitle() const
{
	return mTitle;
}

bool Window::isVerticalSyncEnabled() const
{
	return mVerticalSyncEnabled;
}

bool Window::isVisible() const
{
	return mVisible;
}

bool Window::isKeyRepeatEnabled() const
{
	return mKeyRepeatEnabled;
}

float Window::getJoystickThreshold() const
{
	return mJoystickThreshold;
}

bool Window::isMouseCursorGrabbed() const
{
	return mMouseCursorGrabbed;
}

const sf::Image& Window::getIcon() const
{
	return mIcon;
}

const std::string& Window::getIconPath() const
{
	return mIconPath;
}

Vector2 Window::getPosition() const
{
	return toOE(mWindow.getPosition());
}

const sf::ContextSettings& Window::getSettings() const
{
	return mSettings;
}

Vector2 Window::getSize() const
{
	return toOE(mWindow.getSize());
}

sf::WindowHandle Window::getSystemHandle() const
{
	return mWindow.getSystemHandle();
}

bool Window::hasFocus() const
{
	return mWindow.hasFocus();
}

void Window::setTitle(const std::string& title)
{
	mTitle = title;
	mWindow.setTitle(title);
}

void Window::setVerticalSyncEnabled(bool enabled)
{
	mVerticalSyncEnabled = enabled;
	mWindow.setVerticalSyncEnabled(enabled);
}

void Window::setVisible(bool visible)
{
	mVisible = visible;
	mWindow.setVisible(visible);
}

void Window::setKeyRepeatEnabled(bool enabled)
{
	mKeyRepeatEnabled = enabled;
	mWindow.setKeyRepeatEnabled(enabled);
}

void Window::setJoystickThreshold(float threshold)
{
	mJoystickThreshold = threshold;
	mWindow.setJoystickThreshold(threshold);
}

void Window::setMouseCursorGrabbed(bool grabbed)
{
	mMouseCursorGrabbed = grabbed;
	mWindow.setMouseCursorGrabbed(grabbed);
}

void Window::setIcon(const std::string& icon)
{
	if (mIcon.loadFromFile(icon))
	{
		mIconPath = icon;
		sf::Vector2u s = mIcon.getSize();
		if (s.x > 0 && s.y > 0)
		{
			mWindow.setIcon(s.x, s.y, mIcon.getPixelsPtr());
		}
	}
	else
	{
		error("Can't load the icon from : " + icon);
		mIconPath.clear();
	}
}

void Window::setIcon(const sf::Image& icon)
{
	mIcon = icon;
	mIconPath.clear();
	sf::Vector2u s = mIcon.getSize();
	if (s.x > 0 && s.y > 0)
	{
		mWindow.setIcon(s.x, s.y, mIcon.getPixelsPtr());
	}
}

void Window::setPosition(const Vector2& position)
{
	mWindow.setPosition({ (I32)position.x, (I32)position.y });
}

void Window::setSize(const Vector2& size)
{
	mWindow.setSize({ (U32)size.x, (U32)size.y });
}

void Window::setActive(bool active)
{
	mWindow.setActive(active);
}

void Window::requestFocus()
{
	mWindow.requestFocus();
}

bool Window::isOpen() const
{
	return mWindow.isOpen();
}

bool Window::pollEvent(sf::Event& event)
{
	#ifdef SFML_SYSTEM_ANDROID
		if (!hasFocus())
		{
			if (mWindow.waitEvent(event))
			{
				targetSignals(event);
				return true;
			}
			return false;
		}
	#endif
	if (mWindow.pollEvent(event))
	{
		targetSignals(event);
		return true;
	}
	return false;
}

void Window::clear()
{
	mWindow.clear();
}

void Window::draw(const sf::Drawable& drawable, const sf::RenderStates& states)
{
	mWindow.draw(drawable, states);
}

void Window::draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates& states)
{
	mWindow.draw(vertices, vertexCount, type, states);
}

void Window::display()
{
	if (mCursor == Cursor::Custom)
	{
		sf::View oldView = mWindow.getView();
		
		applyMainView();

		mCursorSprite.setPosition(toSF(getCursorPosition()));
		mWindow.draw(mCursorSprite);

		mWindow.setView(oldView);
	}

	mWindow.display();
}

void Window::setView(const View& view)
{
	mWindow.setView(view.getHandle());
}

View Window::getView() const
{
	return View(mWindow.getView());
}

void Window::setMainView(const View& view)
{
	mMainView = view;
}

const View& Window::getMainView() const
{
	return mMainView;
}

void Window::applyMainView()
{
	mWindow.setView(mMainView.getHandle());
}

sf::IntRect Window::getViewport(const View& view) const
{
	return mWindow.getViewport(view.getHandle());
}

Vector2 Window::mapPixelToCoords(const Vector2& point, const View& view)
{
	if (view.getSize() == Vector2())
	{	
		return toOE(mWindow.mapPixelToCoords({ (I32)point.x, (I32)point.y }));
	}
	else
	{
		return toOE(mWindow.mapPixelToCoords({ (I32)point.x, (I32)point.y }, view.getHandle()));
	}
}

Vector2 Window::mapCoordsToPixel(const Vector2& point, const View& view)
{
	if (view.getSize() == Vector2())
	{
		return toOE(mWindow.mapCoordsToPixel(toSF(point)));
	}
	else
	{
		return toOE(mWindow.mapCoordsToPixel(toSF(point), view.getHandle()));
	}
}

void Window::setCursorPosition(const Vector2& position)
{
	#ifndef SFML_SYSTEM_ANDROID
		sf::Mouse::setPosition(mWindow.mapCoordsToPixel(toSF(position), mMainView.getHandle()));
	#endif
}

void Window::setCursorPositionView(const Vector2& position, const View& view)
{
	#ifndef SFML_SYSTEM_ANDROID
		sf::Mouse::setPosition(mWindow.mapCoordsToPixel(toSF(position), view.getHandle()));
	#endif
}

Vector2 Window::getCursorPosition(U32 touchIndex) const
{
	#ifdef SFML_SYSTEM_ANDROID
		return toOE(mWindow.mapPixelToCoords(sf::Touch::getPosition(touchIndex), mMainView.getHandle()));
	#else
		return toOE(mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow), mMainView.getHandle()));
	#endif
}

Vector2 Window::getCursorPositionView(const View& view, U32 touchIndex)
{
	#ifdef SFML_SYSTEM_ANDROID
		return toOE(mWindow.mapPixelToCoords(sf::Touch::getPosition(touchIndex), view.getHandle()));
	#else
		return toOE(mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow), view.getHandle()));
	#endif
}

Window::Cursor Window::getCursor() const
{
	return mCursor;
}

void Window::setCursor(Cursor mouseCursor)
{
	mCursor = mouseCursor;
	mWindow.setMouseCursorVisible(mCursor == Cursor::Default);
}

void Window::hideCursor()
{
	setCursor(Cursor::None);
}

void Window::useDefaultCursor()
{
	setCursor(Cursor::Default);
}

void Window::useCustomCursor(const std::string& texture, const sf::IntRect& textureRect, const sf::Vector2f& origin)
{
	setCursorTexture(texture);
	if (textureRect != sf::IntRect())
	{
		setCursorTextureRect(textureRect);
	}
	setCursorOrigin(origin);
}

void Window::useCustomCursor(sf::Texture* texture, const std::string& textureData, const sf::IntRect& textureRect, const sf::Vector2f& origin)
{
	setCursorTexture(texture, textureData);
	if (textureRect != sf::IntRect())
	{
		setCursorTextureRect(textureRect);
	}
	setCursorOrigin(origin);
}

sf::Texture* Window::getCursorTexture() const
{
	return mCursorTexture;
}

const std::string& Window::getCursorTextureData() const
{
	return mCursorTextureData;
}

const sf::IntRect& Window::getCursorTextureRect() const
{
	return mCursorSprite.getTextureRect();
}

const sf::Vector2f& Window::getCursorOrigin() const
{
	return mCursorSprite.getOrigin();
}

const sf::Vector2f& Window::getCursorScale() const
{
	return mCursorSprite.getScale();
}

float Window::getCursorRotation() const
{
	return mCursorSprite.getRotation();
}

const sf::Color& Window::getCursorColor() const
{
	return mCursorSprite.getColor();
}

void Window::setCursorTexture(const std::string& texture)
{
	if (!mCursorTextureCreated)
	{
		mCursorTexture = new sf::Texture();
		mCursorTextureCreated = true;
	}
	if (mCursorTexture->loadFromFile(texture))
	{
		mCursorTextureData = texture;
		setCursor(Cursor::Custom);
		mCursorSprite.setTexture(*mCursorTexture);
	}
	else
	{
		error("Can't load cursor from : " + texture);
		mCursorTextureData.clear();
		setCursor(Cursor::Default);
	}
}

void Window::setCursorTexture(sf::Texture* texture, const std::string& textureData)
{
	if (mCursorTextureCreated)
	{
		delete mCursorTexture;
		mCursorTexture = nullptr;
		mCursorTextureCreated = false;
	}
	mCursorTexture = texture;
	mCursorTextureData = textureData;
	mCursorSprite.setTexture(*mCursorTexture);
}

void Window::setCursorTextureRect(const sf::IntRect& textureRect)
{
	mCursorSprite.setTextureRect(textureRect);
}

void Window::setCursorOrigin(const sf::Vector2f& origin)
{
	mCursorSprite.setOrigin(origin);
}

void Window::setCursorScale(const sf::Vector2f& scale)
{
	mCursorSprite.setScale(scale);
}

void Window::setCursorRotation(float rotation)
{
	mCursorSprite.setRotation(rotation);
}

void Window::setCursorColor(const sf::Color& color)
{
	mCursorSprite.setColor(color);
}

void Window::screenshot()
{
	Date date;
	sf::Texture texture;
	texture.create((U32)getSize().x, (U32)getSize().y);
	texture.update(mWindow);
	texture.copyToImage().saveToFile(mScreenshotPath + date.toString("%Y-%m-%d_%H-%M-%S") +".png");
}

const std::string& Window::getScreenshotPath() const
{
	return mScreenshotPath;
}

void Window::setScreenshotPath(const std::string& screenshotPath)
{
	mScreenshotPath = screenshotPath;
}

sf::RenderWindow& Window::getHandle()
{
	return mWindow;
}

void Window::init()
{
	// Init render window
	mTitle.clear();
	mVisible = false;
	mVerticalSyncEnabled = false;
	mKeyRepeatEnabled = true;
	mJoystickThreshold = 0.1f;
	mMouseCursorGrabbed = false;
	mSettings = sf::ContextSettings();
	mFullscreen = false;
	mFullscreenVideoMode = sf::VideoMode::getFullscreenModes()[0];
	mNonFullscreenVideoMode = sf::VideoMode::getDesktopMode();
	mNonFullscreenStyle = sf::Style::Close;
	mMainView.reset(0.0f, 0.0f, F32(sf::VideoMode::getDesktopMode().width), F32(sf::VideoMode::getDesktopMode().height));
	mIconPath.clear();

	// Init mouse cursor
	mCursor = Cursor::Default;
	mCursorTexture = nullptr;
	mCursorTextureCreated = false;
	mCursorTextureData.clear();

	// Init screenshot
	mScreenshotPath.clear();
}

void Window::targetSignals(const sf::Event& event)
{
	// Close
	if (event.type == sf::Event::Closed)
	{
		close();
	}

	// Lost Focus
	if (event.type == sf::Event::LostFocus)
	{
		onWindowLostFocus(this);
	}

	// Gained Focus
	if (event.type == sf::Event::GainedFocus)
	{
		onWindowGainedFocus(this);
	}
}

Window::RWindow::RWindow(oe::Window& handle) : mHandle(handle)
{
}

void Window::RWindow::onResize()
{
	mHandle.onWindowResized(&mHandle, getSize().x, getSize().y);
}

void Window::RWindow::init()
{
	sf::RenderTarget::initialize();
}

} // namespace oe