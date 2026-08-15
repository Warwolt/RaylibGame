#pragma once

#include "game/animation.h"
#include "game/debug.h"
#include "game/resource.h"
#include "game/scene/scene.h"
#include "game/sprite_sheet.h"
#include "platform/input.h"
#include "platform/window.h"

// The state storage of the whole game.
//
// NOTE: To support hot reloading, you may NEVER store a function pointer here!
// If you do, the program will likely crash on reload since the function
// pointer will point into the library that was just unloaded.
struct Game {
	bool should_quit = false;
	Debug debug;
	Window window;
	Input input;
	SceneManager scenes;
	ResourceManager resources;
	SpriteSheetManager sprites;
};

extern "C" __declspec(dllexport) Game* Game_initialize(int argc, char** argv);
extern "C" __declspec(dllexport) void Game_update(Game* game);
extern "C" __declspec(dllexport) void Game_render(const Game& game);
extern "C" __declspec(dllexport) void Game_shutdown(Game* game);
