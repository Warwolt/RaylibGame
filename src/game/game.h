#pragma once

#include "core/util/time.h"
#include "game/debug_overlay.h"
#include "game/resource.h"
#include "game/scene/scene.h"
#include "hot_reload.h"
#include "platform/input.h"
#include "platform/window.h"

// The state storage of the whole game.
//
// NOTE: To support hot reloading, you may NEVER store a function pointer here!
// If you do, the program will likely crash on reload since the function
// pointer will point into the library that was just unloaded.
struct Game {
	bool should_quit = false;
	Timestamped<HotReloadState> hot_reload_state;
	DebugOverlay debug_overlay;
	Window window;
	Input input;
	SceneManager scenes;
	ResourceManager resources;
};
