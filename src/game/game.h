#pragma once

#include "platform/window.h"

struct Game {
	bool should_quit = false;
	Window window;
};

extern "C" __declspec(dllexport) Game* Game_initialize(int argc, char** argv);
extern "C" __declspec(dllexport) void Game_update(Game* game);
extern "C" __declspec(dllexport) void Game_render(const Game& game);
extern "C" __declspec(dllexport) void Game_shutdown(Game* game);
