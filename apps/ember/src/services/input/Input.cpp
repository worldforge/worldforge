//
// C++ Implementation: Input
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2005
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//

#include "Input.h"
#include "InputCommandMapper.h"
#include "EmberIcon.h"

#include "IInputAdapter.h"

#include "services/config/ConfigListenerContainer.h"

#include "framework/Tokeniser.h"
#include "framework/Log.h"
#include "framework/MainLoopController.h"
#include "framework/utf8.h"

#ifdef _WIN32
#include "platform/platform_windows.h"
#endif

#include <SDL3/SDL.h>

#include <sstream>
#include <spdlog/spdlog.h>

namespace Ember {

const std::string Input::BINDCOMMAND("bind");
const std::string Input::UNBINDCOMMAND("unbind");

Input::Input() :
		mCurrentInputMode(IM_GUI),
		mMouseState(0),
		mTimeSinceLastRightMouseClick(0),
		mMousePosition{},
		mSuppressForCurrentEvent(false),
		mMovementModeEnabled(false),
		mConfigListenerContainer(new ConfigListenerContainer()),
		mMouseGrabbingRequested(false),
		mMouseGrab(false),
		mMainLoopController(nullptr),
		mScreenWidth(0),
		mScreenHeight(0),
		mMainVideoSurface(nullptr),
		mIconSurface(nullptr),
		mInvertMouse(false),
		mHandleOpenGL(false),
		mMainWindowId(0),
		mLastTimeInputProcessingStart(std::chrono::steady_clock::now()),
		mLastTimeInputProcessingEnd(std::chrono::steady_clock::now()) {
	mMousePosition.xPixelPosition = 0;
	mMousePosition.yPixelPosition = 0;
	mMousePosition.xRelativePosition = 0.0f;
	mMousePosition.yRelativePosition = 0.0f;

#if defined(_WIN32) || defined(__APPLE__)
	SDL_InitSubSystem(SDL_INIT_VIDEO);
#endif
	mLastTick = std::chrono::steady_clock::now();

	//this is a failsafe which guarantees that SDL is correctly shut down (returning the screen to correct resolution, releasing mouse etc.) if there's a crash.
	atexit([]() {
		//Flush all logs to disk.
		spdlog::shutdown();
		SDL_Quit();
	});

}

Input::~Input() {
	shutdownInteraction();
	if (mIconSurface) {
		SDL_DestroySurface(mIconSurface);
	}
}

std::string Input::createWindow(unsigned int width, unsigned int height, bool fullscreen, bool resizable, bool handleOpenGL) {


	mHandleOpenGL = handleOpenGL;
	unsigned int flags = 0;

	if (resizable) {
		flags |= SDL_WINDOW_RESIZABLE;
	}

	if (mHandleOpenGL) {
		flags |= SDL_WINDOW_OPENGL;
	}

	if (fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	if (mMainVideoSurface) {
		SDL_DestroyWindow(mMainVideoSurface);
		mMainVideoSurface = nullptr;
		mMainWindowId = 0;
	}
	mMainVideoSurface = SDL_CreateWindow("Ember",
										 (int) width,
										 (int) height,
										 flags); // create an SDL window
	SDL_SetWindowPosition(mMainVideoSurface, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	createIcon();

	mMainWindowId = SDL_GetWindowID(mMainVideoSurface);
	SDL_GetWindowSize(mMainVideoSurface, &mScreenWidth, &mScreenHeight);

	//Perhaps the centering code below isn't needed with SDL3?
// #ifdef _WIN32
// 	// When SDL is centering the window, it doesn't take into account the tray bar.
// 	// This approach will center the window to the work area.
// 	HWND hwnd = info.info.win.window;
// 	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
// 	if (hMonitor) {
// 		MONITORINFO mon;
// 		memset(&mon, 0, sizeof(MONITORINFO));
// 		mon.cbSize = sizeof(MONITORINFO);
// 		GetMonitorInfo(hMonitor, &mon);
//
// 		RECT r;
// 		GetWindowRect(hwnd, &r);
//
// 		int winWidth = r.right - r.left;
// 		int winLeft = mon.rcWork.left + (mon.rcWork.right - mon.rcWork.left) / 2 - winWidth / 2;
//
// 		int winHeight = r.bottom - r.top;
// 		int winTop = mon.rcWork.top + (mon.rcWork.bottom - mon.rcWork.top) / 2 - winHeight / 2;
// 		winTop = std::max(winTop, 0);
//
// 		SetWindowPos(hwnd, 0, winLeft, winTop, 0, 0, SWP_NOSIZE);
// 	}
// #endif


	SDL_HideCursor();
//	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	setGeometry((int) width, (int) height);

	std::string handle;
#ifdef __APPLE__
	//On OSX we'll tell Ogre to use the current OpenGL context; thus we don't need to return the window id
#elif defined(_WIN32)
	HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(mMainVideoSurface), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	if (hwnd) {
		handle = std::to_string((unsigned long long)hwnd);
	}
#else
	auto xwindow = (unsigned long)SDL_GetNumberProperty(SDL_GetWindowProperties(mMainVideoSurface), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
	handle = std::to_string(xwindow);
#endif




	return handle;
}

void Input::shutdownInteraction() {

	if (mMainVideoSurface) {
		SDL_DestroyWindow(mMainVideoSurface);
		mMainVideoSurface = nullptr;
	}

	//Release the mouse for safety's sake.
	SDL_SetWindowMouseGrab(mMainVideoSurface, false);
}

void Input::createIcon() {
	//set the icon of the window
	Uint32 rmask, gmask, bmask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
#endif

	//We'll use the emberIcon struct. This isn't needed on WIN32 or OSX as the icon is provided through different means.
	if (!mIconSurface) {
		auto pixelFormat = SDL_GetPixelFormatForMasks(24,  rmask, gmask, bmask, 0);
		mIconSurface = SDL_CreateSurfaceFrom(64, 64, pixelFormat, emberIcon.pixel_data, 64 * 3);
	}
	if (mIconSurface) {
		SDL_SetWindowIcon(mMainVideoSurface, mIconSurface);
	}

}

void Input::setGeometry(int width, int height) {
	if (mScreenHeight != height || mScreenWidth != width) {
		mScreenWidth = width;
		mScreenHeight = height;
		SDL_SetWindowSize(mMainVideoSurface, width, height);
		EventSizeChanged.emit(width, height);
	}
}


void Input::suppressFurtherHandlingOfCurrentEvent() {
	mSuppressForCurrentEvent = true;
}

bool Input::getMovementModeEnabled() const {
	return mMovementModeEnabled;
}

void Input::setMovementModeEnabled(bool value) {
	mMovementModeEnabled = value;
}

void Input::registerCommandMapper(InputCommandMapper* mapper) {
	mInputCommandMappers[mapper->getState()] = mapper;
}

void Input::deregisterCommandMapper(InputCommandMapper* mapper) {
	mInputCommandMappers.erase(mInputCommandMappers.find(mapper->getState()));
}

InputCommandMapper* Input::getMapperForState(const std::string& state) {
	auto I = mInputCommandMappers.find(state);
	if (I != mInputCommandMappers.end()) {
		return I->second;
	}
	return nullptr;
}


bool Input::isApplicationVisible() {
	return !(SDL_GetWindowFlags(mMainVideoSurface) & SDL_WINDOW_MINIMIZED);
}

bool Input::isApplicationFocused() {
	return SDL_GetWindowFlags(mMainVideoSurface) & SDL_WINDOW_MOUSE_FOCUS;
}

void Input::startInteraction() {
	mConfigListenerContainer->registerConfigListenerWithDefaults("input",
																 "catchmouse",
																 sigc::mem_fun(*this, &Input::Config_CatchMouse),
																 true);
	mConfigListenerContainer->registerConfigListenerWithDefaults("input",
																 "invertcamera",
																 sigc::mem_fun(*this, &Input::Config_InvertCamera),
																 true);
}

void Input::processInput(const std::chrono::steady_clock::time_point currentTime) {

	mMainLoopController->EventBeforeInputProcessing.emit((float) (currentTime - mLastTimeInputProcessingStart).count() / 1000000000.0f);
	mLastTimeInputProcessingStart = currentTime;

	auto newTick = currentTime;
	float secondsSinceLast = std::chrono::duration_cast<std::chrono::duration<float>>(newTick - mLastTick).count();

	mLastTick = newTick;
	pollMouse(secondsSinceLast);
	pollEvents(secondsSinceLast);

	//currentTime = std::chrono::steady_clock::now();
	mMainLoopController->EventAfterInputProcessing.emit((float) (currentTime - mLastTimeInputProcessingEnd).count() / 1000000000.0f);
	mLastTimeInputProcessingEnd = currentTime;
}

void Input::pollMouse(float secondsSinceLast) {
	SDL_PumpEvents(); // Loop through all pending system events to get the latest mouse position.
	float mouseX, mouseY, mouseRelativeX, mouseRelativeY;
	mMouseState = SDL_GetMouseState(&mouseX, &mouseY);
	if (mCurrentInputMode == IM_GUI) {
		mouseRelativeX = (mMousePosition.xPixelPosition - mouseX);
		mouseRelativeY = (mMousePosition.yPixelPosition - mouseY);
	} else {
		SDL_GetRelativeMouseState(&mouseRelativeX, &mouseRelativeY);
		//Due to legacy code we need to flip the sign on the relative mouse movement when using the SDL2 method.
		mouseRelativeX = -mouseRelativeX;
		mouseRelativeY = -mouseRelativeY;
	}

	//has the mouse moved?
	auto appState = SDL_GetWindowFlags(mMainVideoSurface);
	if (appState & SDL_WINDOW_MOUSE_FOCUS) {
		//Wait with grabbing the mouse until the app has input focus.
		if (mMouseGrabbingRequested && (appState & SDL_WINDOW_INPUT_FOCUS)) {
			setMouseGrab(true);
		}
		if (mouseRelativeX != 0 || mouseRelativeY != 0) {


			//we'll calculate the mouse movement difference and send the values to those
			//listening to the MouseMoved event
			float diffX = (float) mouseRelativeX / (float) mScreenWidth;
			float diffY = (float) mouseRelativeY / (float) mScreenHeight;
			MouseMotion motion{};
			motion.xPosition = mouseX;
			motion.yPosition = mouseY;
			motion.xRelativeMovement = mInvertMouse ? -diffX : diffX;
			motion.yRelativeMovement = mInvertMouse ? -diffY : diffY;
			motion.xRelativeMovementInPixels = mInvertMouse ? -mouseRelativeX : mouseRelativeX;
			motion.yRelativeMovementInPixels = mInvertMouse ? -mouseRelativeY : mouseRelativeY;
			motion.timeSinceLastMovement = secondsSinceLast;

			EventMouseMoved.emit(motion, mCurrentInputMode);

			bool freezeMouse = false;
			//if we're in gui mode, we'll just send the mouse movement on to CEGUI
			if (mCurrentInputMode == IM_GUI) {

				for (auto I = mAdapters.begin(); I != mAdapters.end();) {
					IInputAdapter* adapter = *I;
					++I;
					if (!(adapter)->injectMouseMove(motion, freezeMouse)) {
						break;
					}
				}

			} else {
				freezeMouse = true;
			}

			//Only warp if we want to freeze and hasn't put the mouse into relative mode.
			if (freezeMouse && mCurrentInputMode == IM_GUI) {
				SDL_WarpMouseInWindow(nullptr, mMousePosition.xPixelPosition, mMousePosition.yPixelPosition);
			} else {
				mMousePosition.xPixelPosition = mouseX;
				mMousePosition.yPixelPosition = mouseY;
				mMousePosition.xRelativePosition = mouseX / static_cast<float>(mScreenWidth);
				mMousePosition.yRelativePosition = mouseY / static_cast<float>(mScreenHeight);
			}

		}
	}

}

void Input::pollEvents(float secondsSinceLast) {
	mTimeSinceLastRightMouseClick += secondsSinceLast;
	static SDL_Event event;
	while (SDL_PollEvent(&event)) {
		EventSDLEventReceived(event);
		switch (event.type) {
			/* Look for a keypress */
			case SDL_EVENT_KEY_DOWN:
			case SDL_EVENT_KEY_UP:
				keyChanged(event.key);
				break;
			case SDL_EVENT_TEXT_INPUT:
				textInput(event.text);
				break;
			case SDL_EVENT_QUIT:
				if (mMainLoopController) {
					mMainLoopController->quit();
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			case SDL_EVENT_MOUSE_BUTTON_UP:
				if (event.button.down == false) {
					if (event.button.button == SDL_BUTTON_RIGHT) {
						//right mouse button released
						mTimeSinceLastRightMouseClick = 0.0f;
						//toggleInputMode();
						EventMouseButtonReleased.emit(MouseButtonRight, mCurrentInputMode);

					} else if (event.button.button == SDL_BUTTON_LEFT) {
						//left mouse button released
						for (auto I = mAdapters.begin(); I != mAdapters.end();) {
							IInputAdapter* adapter = *I;
							++I;
							if (!(adapter)->injectMouseButtonUp(Input::MouseButtonLeft))
								break;
						}

						EventMouseButtonReleased.emit(MouseButtonLeft, mCurrentInputMode);

					} else if (event.button.button == SDL_BUTTON_MIDDLE) {
						//middle mouse button released
						for (auto I = mAdapters.begin(); I != mAdapters.end();) {
							IInputAdapter* adapter = *I;
							++I;
							if (!(adapter)->injectMouseButtonUp(Input::MouseButtonMiddle))
								break;
						}

						EventMouseButtonReleased.emit(MouseButtonMiddle, mCurrentInputMode);
					}
				} else {

					if (event.button.button == SDL_BUTTON_RIGHT) {
						//right mouse button pressed

						//if the right mouse button is pressed, switch from gui mode

						if (mMovementModeEnabled) {
							toggleInputMode();
						}
						EventMouseButtonPressed.emit(MouseButtonRight, mCurrentInputMode);

					} else if (event.button.button == SDL_BUTTON_LEFT) {
						//left mouse button pressed

						for (auto I = mAdapters.begin(); I != mAdapters.end();) {
							IInputAdapter* adapter = *I;
							++I;
							if (!(adapter)->injectMouseButtonDown(Input::MouseButtonLeft))
								break;
						}

						EventMouseButtonPressed.emit(MouseButtonLeft, mCurrentInputMode);
					} else if (event.button.button == SDL_BUTTON_MIDDLE) {
						//middle mouse button pressed
						for (auto I = mAdapters.begin(); I != mAdapters.end();) {
							IInputAdapter* adapter = *I;
							++I;
							if (!(adapter)->injectMouseButtonDown(Input::MouseButtonMiddle))
								break;
						}

						EventMouseButtonPressed.emit(MouseButtonMiddle, mCurrentInputMode);

					}
				}
				break;
			case SDL_EVENT_MOUSE_WHEEL:
				if (event.wheel.y > 0) {
					EventMouseButtonPressed.emit(MouseWheelUp, mCurrentInputMode);
					for (auto I = mAdapters.begin(); I != mAdapters.end();) {
						IInputAdapter* adapter = *I;
						++I;
						if (!(adapter)->injectMouseButtonDown(Input::MouseWheelUp))
							break;
					}
				} else if (event.wheel.y < 0) {
					EventMouseButtonPressed.emit(MouseWheelDown, mCurrentInputMode);
					for (auto I = mAdapters.begin(); I != mAdapters.end();) {
						IInputAdapter* adapter = *I;
						++I;
						if (!(adapter)->injectMouseButtonDown(Input::MouseWheelDown))
							break;
					}
				}
				break;

			case SDL_EVENT_WINDOW_SHOWN: {
				EventWindowActive.emit(true);
				break;
			}
			case SDL_EVENT_WINDOW_HIDDEN: {
				EventWindowActive.emit(false);
				//On Windows we get a corrupted screen if we just switch to non-fullscreen here.
#ifndef _WIN32
				lostFocus();
#endif
				break;
			}
			case SDL_EVENT_WINDOW_RESIZED: {
				setGeometry(event.window.data1, event.window.data2);
				break;
			}
			case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
				SDL_EnableScreenSaver();
				break;
			}
			case SDL_EVENT_WINDOW_MOUSE_ENTER: {
				SDL_DisableScreenSaver();
				break;
			}

			default:
				break;
		}
	}
}

void Input::textInput(const SDL_TextInputEvent& textEvent) {
	if (mCurrentInputMode == IM_GUI) {

		const char* text = textEvent.text;
		size_t len = 0;
		for (; len < 4; len++) {
			if (text[len] == 0) {
				break;
			}
		}

		std::vector<int> utf32result;
		utf8::utf8to32(text, text + len, std::back_inserter(utf32result));
		if (!utf32result.empty()) {
			int character = utf32result.front();
			for (auto I = mAdapters.begin(); I != mAdapters.end() && !mSuppressForCurrentEvent;) {
				IInputAdapter* adapter = *I;
				++I;
				if (!(adapter)->injectChar(character)) {
					break;
				}
			}
		}
	}
}

void Input::keyChanged(const SDL_KeyboardEvent& keyEvent) {
//On windows the OS will handle alt-tab independently, so we don't need to check here
#ifndef _WIN32
	if ((keyEvent.key & SDL_KMOD_LALT) && (keyEvent.key == SDLK_TAB)) {
		if (keyEvent.type == SDL_EVENT_KEY_DOWN) {
			lostFocus();
		}
#else
		if (false) {
#endif
	} else {
		if (keyEvent.type == SDL_EVENT_KEY_DOWN) {
			mKeysPressed.insert(keyEvent.scancode);
			keyPressed(keyEvent);
		} else {
			mKeysPressed.erase(keyEvent.scancode);
			keyReleased(keyEvent);
		}
	}

}

bool Input::isKeyDown(const SDL_Scancode& key) const {
	return mKeysPressed.find(key) != mKeysPressed.end();
}

void Input::keyPressed(const SDL_KeyboardEvent& keyEvent) {
	mSuppressForCurrentEvent = false;
	if (mCurrentInputMode == IM_GUI) {
		// do event injection
		// key down
		for (auto I = mAdapters.begin(); I != mAdapters.end() && !mSuppressForCurrentEvent;) {
			IInputAdapter* adapter = *I;
			++I;
			if (!(adapter)->injectKeyDown(keyEvent.scancode))
				break;
		}
	}
	if (!mSuppressForCurrentEvent) {
		EventKeyPressed(keyEvent, mCurrentInputMode);
	}
	mSuppressForCurrentEvent = false;

}

void Input::keyReleased(const SDL_KeyboardEvent& keyEvent) {
	mSuppressForCurrentEvent = false;
	if (mCurrentInputMode == IM_GUI) {
		for (auto I = mAdapters.begin(); I != mAdapters.end() && !mSuppressForCurrentEvent;) {
			IInputAdapter* adapter = *I;
			++I;
			if (!(adapter)->injectKeyUp(keyEvent.scancode))
				break;
		}
	}
	if (!mSuppressForCurrentEvent) {
		EventKeyReleased(keyEvent, mCurrentInputMode);
	}
	mSuppressForCurrentEvent = false;
}

void Input::setInputMode(InputMode mode) {
	setMouseGrab(mode == IM_MOVEMENT);
	mCurrentInputMode = mode;
	EventChangedInputMode.emit(mode);
}

Input::InputMode Input::getInputMode() const {
	return mCurrentInputMode;
}

Input::InputMode Input::toggleInputMode() {
	if (mCurrentInputMode == IM_GUI) {
		setInputMode(IM_MOVEMENT);
		return IM_MOVEMENT;
	} else {
		setInputMode(IM_GUI);
		return IM_GUI;
	}
}

void Input::addAdapter(IInputAdapter* adapter) {
	mAdapters.push_front(adapter);
}

void Input::removeAdapter(IInputAdapter* adapter) {
	mAdapters.remove(adapter);
}

const MousePosition& Input::getMousePosition() const {
	return mMousePosition;
}

void Input::setMainLoopController(MainLoopController* mainLoopController) {
	mMainLoopController = mainLoopController;
}

void Input::enableTextInput() const {
	SDL_StartTextInput(mMainVideoSurface);
}

void Input::disableTextInput() const {
	SDL_StopTextInput(mMainVideoSurface);
}

MainLoopController* Input::getMainLoopController() const {
	return mMainLoopController;
}

void Input::Config_CatchMouse(const std::string& section, const std::string& key, varconf::Variable& variable) {
	try {
		if (variable.is_bool()) {
			bool enabled = static_cast<bool>(variable);
			if (enabled) {
				mMouseGrabbingRequested = true;
			} else {
				setMouseGrab(false);
			}
		}
	} catch (const std::exception& ex) {
		logger->error("Error when changing mouse grabbing: {}", ex.what());
	}
}

void Input::Config_InvertCamera(const std::string& section, const std::string& key, varconf::Variable& variable) {
	if (variable.is_bool()) {
		mInvertMouse = static_cast<bool>(variable);
	}
}

void Input::setFullscreen(bool enabled) {
	SDL_SetWindowFullscreen(mMainVideoSurface, enabled);
}

void Input::toggleFullscreen() {
	setFullscreen((SDL_GetWindowFlags(mMainVideoSurface) & SDL_WINDOW_FULLSCREEN) == 0);
}


bool Input::hasWindow() const {
	return mMainVideoSurface != nullptr;
}

void Input::lostFocus() {
	setInputMode(Input::IM_GUI);
	setMouseGrab(false);
	setFullscreen(false);
}

void Input::setMouseGrab(bool enabled) {
	logger->debug("mouse grab: {}", enabled);

	if (auto result = SDL_SetWindowRelativeMouseMode(mMainVideoSurface, enabled); !result) {
		logger->warn("Setting relative mouse mode doesn't work.");
	}

	//We must reset the relative mouse state reporting.
	SDL_GetRelativeMouseState(nullptr, nullptr);

	if (!enabled) {
		//When we return back to not grabbing the mouse after having grabbed it we need to warp back the mouse position.
		SDL_WarpMouseInWindow(nullptr, mMousePosition.xPixelPosition, mMousePosition.yPixelPosition);
	}
}

}

